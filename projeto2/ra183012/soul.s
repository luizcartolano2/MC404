.org 0x0
.section .iv,"a"

interrupt_vector:

.org 0x0
    b RESET_HANDLER
.org 0x08
    b SVC_HANDLER
.org 0x18
    b IRQ_HANDLER

.data

@Tempo do sistema e flag do irq
CONTADOR: .skip 16
FLAG_IRQ: .skip 4

.set TIME_SZ, 100

@Valore absolutos registradores GPT_CR
.set GPT_BASE,  0x53FA0000
.set GPT_CR,    0x0
.set GPT_PR,    0x4
.set GPT_SR,    0x8
.set GPT_OCR1,  0x10
.set GPT_IR,    0xC

@Inicio do código do usuário
.set INICIO, 0x77812000

@Vetores de armazenamento das callbacks
callback_sonar: .skip 16
callback_distance: .skip 32
callback_func: .skip 32
CALLBACK_COUNTER: .skip 4

@Vetores de armazenamento dos alarmes
alarm_func: .skip 32
alarm_time: .skip 32
ALARM_COUNTER: .skip 4

@Pilhas dos modos
.skip 1024
pilha_supervisor:

.skip 1024
pilha_irq:

.set pilha_usuario, 0x77804000

.text

RESET_HANDLER:

    @Faz o registrador que aponta para a tabela de interrupções apontar para a tabela interrupt_vector
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

    @ entra no supervisor
    msr CPSR_c, #0xD3

    @ Zera o contador
    ldr r2, =CONTADOR  @lembre-se de declarar esse contador em uma secao de dados!
    mov r0, #0
    str r0, [r2]

    @Zera o contador de callback
    ldr r2, =CALLBACK_COUNTER
    mov r0, #0
    str r0, [r2]

    @Zera o contador de alarme
    ldr r2, =ALARM_COUNTER
    mov r0, #0
    str r0, [r2]

    @Inicializa a flag do irq
    ldr r2, =FLAG_IRQ
    mov r0, #0
    str r0, [r2]

@Configurando o registrador GPT
SET_GPT:
    ldr r0, =GPT_BASE

    @Configurando o registrador G_CR
    mov r1, #0x41
    str r1, [r0, #GPT_CR]

    @Zerando o valor de GPT_PR
    mov r1, #0
    str r1, [r0, #GPT_PR]

    @Salvando o tempo de interrupção do GPT em GPT_OCR1
    ldr r1,=TIME_SZ
    str r1, [r0, #GPT_OCR1]

    @Gravando 1 em GPT_IR
    mov r1, #1
    str r1, [r0, #GPT_IR]

@Configurar o GPIO
SET_GPIO:
    @Enderecos absolutos de GPIO
    .set GPIO_DR,    0x53F84000
    .set GPIO_GDIR,  0x53F84004
    .set GPIO_PSR,	 0x53F84008
    .set MASCARA_GDIR, 0xFFFC003E

    @Escrevendo a máscara para saida e entrada em GDIR
    ldr r0, =GPIO_GDIR
    ldr r1, =MASCARA_GDIR
    str r1, [r0]

@Configurar TZIC
SET_TZIC:
    @ Constantes para os enderecos do TZIC
    .set TZIC_BASE,             0x0FFFC000
    .set TZIC_INTCTRL,          0x0
    .set TZIC_INTSEC1,          0x84
    .set TZIC_ENSET1,           0x104
    .set TZIC_PRIOMASK,         0xC
    .set TZIC_PRIORITY9,        0x424

    @ Liga o controlador de interrupcoes
    @ R1 <= TZIC_BASE

    ldr	r1, =TZIC_BASE

    @ Configura interrupcao 39 do GPT como nao segura
    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_INTSEC1]

    @ Habilita interrupcao 39 (GPT)
    @ reg1 bit 7 (gpt)

    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_ENSET1]

    @ Configure interrupt39 priority as 10x13
    @ reg9, byte 3

    ldr r0, [r1, #TZIC_PRIORITY9]
    bic r0, r0, #0xFF000000
    mov r2, #1
    orr r0, r0, r2, lsl #24
    str r0, [r1, #TZIC_PRIORITY9]

    @ Configure PRIOMASK as 0
    eor r0, r0, r0
    str r0, [r1, #TZIC_PRIOMASK]

    @ Habilita o controlador de interrupcoes
    mov	r0, #1
    str	r0, [r1, #TZIC_INTCTRL]

    ldr sp, =pilha_supervisor

      @Configura a pilha irq
      msr CPSR_c, #0xD2
      ldr sp, =pilha_irq

      @Configura a pilha supervisor
      msr  CPSR_c, #0xD3
      ldr sp, =pilha_supervisor

      @configura pilha usuário
      msr CPSR_c, #0x10
      ldr sp, =pilha_usuario

    @Move para o código do usuários
    ldr lr, =INICIO
    mov pc, lr

@r0-r4 os parâmetros da função
@r7, o tipo da system call
SVC_HANDLER:

    push {lr}

    @read_sonar
    cmp r7, #16
        bleq func_read_sonar

    @register_proximity_callback
    cmp r7, #17
        bleq func_register_proximity_callback

    @set_motor_speed
    cmp r7, #18
        bleq func_set_motor_speed

    @set_motors_speed
    cmp r7, #19
        bleq func_set_motors_speed

    @func_get_time
    cmp r7, #20
        bleq func_get_time

    @func_set_time
    cmp r7, #21
        bleq func_set_time

    @set_alarm
    cmp r7, #22
        bleq func_set_alarm

    @troca_modo_alarm
    cmp r7, #23
      beq func_troca_modo_alarm

    @troca_modo
    cmp r7, #24
      beq func_troca_modo_callback

    pop {lr}
    movs pc, lr

@Troca o modo do processador
func_troca_modo_alarm:

  msr CPSR_c, #0xD2

  bl return_troca_modo_alarm

@Troca o modo do processador
func_troca_modo_callback:

  msr CPSR_c, #0xD2

  bl return_troca_modo_callback

@Sleep para esperar retorno de GPIO
func_sleep:

    push {lr}

    ldr r0, =1
    inicio_for:
        sub r0, r0, #1
        cmp r0, #0
        bne inicio_for

    pop {lr}

    mov pc, lr

@Realiza a leitura da distância do sonar, e retorna em r0
@r0, id do sonar
func_read_sonar:
  push {r4-r11, lr}

 @ verifica se o sonar a ser lido esta dentro dos limites permitidos
   cmp r0, #0
   movlt r0, #-1
   blt fim_leitura_sonar

   cmp r0, #15
   movhi r0, #-1
   bhi fim_leitura_sonar

   @ temos um sonar valido, devemos le-lo
   ldr r4, =GPIO_DR

   @ seta o sonar a ser lido
   ldr r5, [r4]
   bic r5, r5, #0b00111110
   orr r5, r5, r0, lsl #2
   ldr r6, =0b00000010000001000000000000000000
   orr r5, r5, r6
   str r5, [r4]

   push {r0}
   bl func_sleep
   pop {r0}

   @ seta o trigger = 1
   ldr r0, [r4]
   orr r0, r0, #0b00000010
   str r0, [r4]

   push {r0}
   bl func_sleep
   pop {r0}

   @ seta o trigger = 0
   ldr r0,[r4]
   eor r0, r0, #0b00000010
   str r0, [r4]

   @ checa a flag
   checa_flag:
     ldr r0, [r4]
     mov r2, r0
     and r2, r2, #1

     cmp r2, #1
     beq flag_definida

     push {r0}
     bl func_sleep
     pop {r0}

     b checa_flag

   flag_definida:
     @Salva em r0 a leitura do sonar
        mov r0, r0, lsr #6
        ldr r1, =0b111111111111
        and r0, r0, r1

   fim_leitura_sonar:
     pop {r4-r11, lr}
     mov pc, lr

@Seta uma velociade para o motor do robo
@r0, id do motor
@r1, velocidade a ser escrita
@ nao parar o outro robo
func_set_motor_speed:

    push {r4, r5, r6, lr}

    mov r4, r0
    mov r5, r1

    @Verifica se o motor é válido
    cmp r4, #0
    beq executa_teste_velocidade
    cmp r4, #1
        movne r0, #-1
        bne motor_invalido

        @Verifica se a velocidade é válida
        executa_teste_velocidade:
            cmp r5, #0
                movmi r0, #-2
                bmi velocidade_invalida
                    cmp r5, #63
                    movhi r0, #-2
                    bhi velocidade_invalida

                        @Escreve em GPIO para setar a velociade
                        @Desloca 1 bit para setra motor_writte = 0
                        mov r5, r5, lsl #1

                        @Descola para o motor 0
                        cmp r4, #0
                            moveq r4, r5, lsl #18
                            ldreq r6, =0b00000010000000000000000000000000
                            orr r4, r6

                        @Desloca para o motor 1
                        cmp r4, #1
                            moveq r4, r5, lsl #25
                            ldreq r6, =0b00000000000001000000000000000000
                            orr r4, r6

                        @Escreve no GPIO
                        ldr r0, =GPIO_DR
                        str r4, [r0]
                        mov r0, #0

    motor_invalido:
    velocidade_invalida:

    pop {r4, r5, r6, lr}
    mov pc, lr

@Seta velocidade para os dois motores
@r0, valor do motor 0
@r1, valor do motor 1
func_set_motors_speed:

    push {r4, r5, lr}

    mov r4, r0
    mov r5, r1

    @Verifica se a velocidade dos motores é válida
    cmp r4, #0
        movmi r0, #-1
        bmi velocidade_invalida2
        cmp r5, #0
            movmi r0, #-2
            bmi velocidade_invalida2
                cmp r4, #63
                    movhi r0, #-1
                    bhi velocidade_invalida2
                        cmp r5, #63
                            movhi r0, #-2
                            bhi velocidade_invalida2
                                bhi velocidade_invalida2
                                @Realiza a escrita das velocidades nos motores
                                mov r5, r5, lsl #1
                                mov r5, r5, lsl #7
                                mov r4, r4, lsl #1
                                add r4, r4, r5
                                mov r4, r4, lsl #18
                                ldr r0, =GPIO_DR
                                str r4, [r0]
                                str r4, [r0]
                                str r4, [r0]
                                str r4, [r0]
                                str r4, [r0]
                                mov r0, #0


    velocidade_invalida2:

    pop {r4, r5, lr}
    mov pc, lr

@Retorna o tempo do sistema em r0
func_get_time:
    push {lr}

    ldr r0,=CONTADOR
    ldr r0, [r0]

    pop {lr}
    mov pc, lr

@Seta o tempo do sistema
@r0, tempo a ser setado
func_set_time:

    push {lr}

    ldr r1,=CONTADOR
    str r0,[r1]

    pop {lr}
    mov pc, lr

@Registra as callbacks na memoria
@r0 identificador do sonar
@r1 liminar de distância
@r2 ponteiro para a chamada da função na ocorrência do alarme
func_register_proximity_callback:

    push {r4, r5, lr}

    @Carrega o counter no registrador
    ldr r3, =CALLBACK_COUNTER
    ldr r5, [r3]

    @Verifica se o vetor de callback está cheio
    cmp r5, #8
    moveq r0, #-1
    beq fim_callback_registro

    @@@@@@@@@@@@@@@@@@@@@ TESTAR @@@@@@@@@@@@@@@@@@@@@
    @Verifica se o valor do sonar é válido
    cmp r0, #0
    movmi r0, #-2
    bmi fim_callback_registro

    cmp r0, #15
    movhi r0, #-2
    bhi fim_callback_registro
    @@@@@@@@@@@@@@@@@@@@@ TESTAR @@@@@@@@@@@@@@@@@@@@@

    @Salva na memoria o id do sonar
    ldr r4, =callback_sonar
    strb r0, [r4, r5]

    @Salva na memoria a distancia
    ldr r4, =callback_distance
    str r1, [r4, r5, lsl #2]

    @Salva na memoria o ponteiro para a função
    ldr r4, =callback_func
    str r2, [r4, r5, lsl #2]

    @Incrementa o contador
    add r5, r5, #1
    str r5, [r3]

    mov r0, #0

    fim_callback_registro:

    pop {r4, r5, lr}

    mov pc, lr

@Carrega uma callback nos registradores
@r0, posição da callback no vetor
func_load_callback:

    push {r4, lr}

    mov r3, r0

    @Carrega sonar no registrador
    ldr r4, =callback_sonar
    ldrb r0, [r4, r3]

    @Carrega distância no registrador
    ldr r4, =callback_distance
    ldr r1, [r4, r3, lsl #2]

    @Carrega função no registrador
    ldr r4, =callback_func
    ldr r2, [r4, r3, lsl #2]

    pop {r4, lr}

    mov pc, lr


@Regsistra uma callback na memoria
@r0 identificador do sonar
@r1 liminar de distância
@r2 ponteiro para a chamada da função na ocorrência do alarme
@r3, posição a escrever a callback
func_register_callback:

    push {r4, lr}

    @Salva o sonar na memória
    ldr r4, =callback_sonar
    strb r0, [r4, r3]

    @Salva a distância na memória
    ldr r4, =callback_distance
    str r1, [r4, r3, lsl #2]

    @Salva a função na memória
    ldr r4, =callback_func
    str r2, [r4, r3, lsl #2]

    pop {r4, lr}

    mov pc, lr

@Adiciona uma alarme
@r0 ponteiro para a funcao que sera ativada
@r1 tempo do alarme
func_set_alarm:
    push {lr}

    @Carrega o contador no registrador r4
    ldr r4, =ALARM_COUNTER
    ldr r4, [r4]

    @Verifica se não atingiu o máximo de alarm
    cmp r4, #8
    moveq r0, #-1
    beq fim_set_alarm

    @Move os parametros para outro registrador
    mov r2, r0
    mov r3, r1

    @Verifica se o tempo do sonar não é menor que o tempo atual
    bl func_get_time
    cmp r3, r0
    beq executa_set_alarm
    movls r0, #-2
    bls fim_set_alarm
        executa_set_alarm:

        @Salva a função e o tempo na memória
        ldr r5, =alarm_func
        str r2, [r5, r4, lsl #2]

        ldr r5, =alarm_time
        str r3, [r5, r4, lsl #2]

        @Atualiza o número de alarmes
        add r4, r4, #1
        ldr r5, =ALARM_COUNTER
        str r4, [r5]

        mov r0, #0

    fim_set_alarm:

    pop {lr}
    mov pc, lr

IRQ_HANDLER:

    push {r0-r12, lr}

    mrs r0, spsr

    push {r0}

    @Informando o GPT que o processador está ciente de uma interrupção
    ldr r0, =GPT_BASE
    mov r1, #1
    str r1, [r0, #GPT_SR]

    @Incrementando contador
    ldr r0, =CONTADOR
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]

    @@@@@@@@@@@@@@@@@@@@@ TESTAR @@@@@@@@@@@@@@@@@@@@@
    @Verifica a flag, se for 2 não irá executar alarmes nem callback
    ldr r0, =FLAG_IRQ
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]

    cmp r1, #2
    beq fim_irq
    @@@@@@@@@@@@@@@@@@@@@ TESTAR @@@@@@@@@@@@@@@@@@@@@

    @Reabilitando as interrupções

    msr CPSR_c, #0b00010010

    @Tratamento da callback
    mov r4, #0
    ldr r5, =CALLBACK_COUNTER
    ldr r5, [r5]
    inicio_callback_verifica:
    cmp r4, r5
        beq fim_callback_verifica

        @Verifica o sonar da callback
        ldr r6, =callback_sonar
        ldrb r0, [r6, r4]
        bl func_read_sonar

        @Carrega a distância na memória
        ldr r6, =callback_distance
        ldr r1, [r6, r4, lsl #2]

        @Verifica se a leitura do sonar é menor que a distância da callback
        cmp r0, r1

            @Carrega e chama a função
            bhi fim_nao_remove_callback
            ldr r6, =callback_func
            ldr r0, [r6, r4, lsl #2]

            msr CPSR_c, #0x10
            blx r0
            mov r7, #24
            svc 0x0
            return_troca_modo_callback:

            @Remove call back se foi chamada
            mov r0, r5
            bl func_load_callback
            mov r3, r4
            bl func_register_callback
            sub r5, r5, #1

            @Subtrai 1 do contador de callbacks
            ldr r6, =CALLBACK_COUNTER
            str r5, [r6]
            sub r4, r4, #1

            fim_nao_remove_callback:

        add r4, r4, #1
        b inicio_callback_verifica

    fim_callback_verifica:

  @ Tratamento de alarme
    mov r4, #0
    ldr r5, =ALARM_COUNTER
    ldr r5, [r5]

    inicio_verifica_alarm:
    cmp r4, r5
    beq fim_alarm_verifica

        @Carrega o tempo do alarme atual
        ldr r0, =alarm_time
        ldr r1, [r0, r4, lsl #2]

        @Compara o tempo com o tempo do sistema
        bl func_get_time

        cmp r1, r0
        bgt fim_nao_remove_alarme

            @Carrega a função no registrador
            ldr r0, =alarm_func
            ldr r1, [r0, r4, lsl #2]

             msr CPSR_c, #0x10
             blx r1

             @ volta para o modo irq
             mov r7, #23
             svc 0x0
             return_troca_modo_alarm:

            @Remove o alarme
            ldr r0, =alarm_func
            ldr r1, [r0, r5, lsl #2]
            str r1,[r0, r4, lsl #2]

            ldr r0, =alarm_time
            ldr r1, [r0, r5, lsl #2]
            str r1,[r0, r4, lsl #2]

            sub r5, r5, #1
            ldr r0, =ALARM_COUNTER
            str r5, [r0]

            sub r4, r4, #1

        fim_nao_remove_alarme:
          add r4, r4, #1
          b inicio_verifica_alarm

    fim_alarm_verifica:

    fim_irq:

    @@@@@@@@@@@@@@@@@@@@@ TESTAR @@@@@@@@@@@@@@@@@@@@@
    @Decremeta flag
    ldr r0, =FLAG_IRQ
    ldr r1, [r0]
    sub r1, r1, #1
    str r1, [r0]

    @@@@@@@@@@@@@@@@@@@@@ TESTAR @@@@@@@@@@@@@@@@@@@@@

    pop {r0}

    msr spsr, r0

    pop {r0-r12, lr}

    @Corrigindo lr
    sub lr, lr, #4

    movs pc, lr
