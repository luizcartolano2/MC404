@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@  Aluno: Luiz Eduardo Cartolano   @
@  RA: 183012                      @
@  Turma: B                        @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global set_motor_speed
.global set_motors_speed
.global read_sonar
.global read_sonars
.global set_time
.global get_time
.global add_alarm
.global register_proximity_callback
.text
.align 4


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@                          MOTORS                             @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@
@ Parametros:
@   r0: ponteiro para uma struct que tem o id do Motor e a velocidade
@ Retorno:
@   void
set_motor_speed:
  push {r7, lr}

  @ como recebemos um endereco com os valores que nos interessam
  @ precisamos carregar o conteudo dos mesmos nos registradores
  ldrb r1,[r0,#1]     @ carrega em r1 a velocidade do motor
  ldrb r0,[r0]        @ carrega em r0 o id do motor

  @ fazemos a chamada do sistema
  mov r7, #18
  svc 0x0

  pop {r7, lr}
  mov pc,lr

@
@ Parametros:
@   r0: ponteiro para uma struct que tem o id do Motor e a velocidade
@   r1: ponteiro para uma struct que tem o id do Motor e a velocidade
@ Retorno:
@   void
set_motors_speed:
  push {r4, r7, lr}

  @ carrega em r4 o id do motor em r0
  ldrb r4, [r0]

  @ verifica a ordem dos motores
  cmp r4, #0
  movne r4, r0    @ salva em r4 o endereco que estava em r0 (que eh o do segundo motor)
  movne r0, r1    @ salva em r0 o endereco que estava em r1 (que eh o do primeiro motor)
  movne r1, r4    @ salva em r1 o endereco que estava em r0 (que eh o do segundo motor)

  ldrb r0, [r0, #1]   @ carrega a velocidade do primeiro motor em r0
  ldrb r1, [r1, #1]   @ carrega a velocidade do segundo motor em r0

  @ faz a chamada do sistema
  mov r7, #19
  svc 0x0

  pop {r4, r7, lr}
  mov pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@                        SONARS                               @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@
@ Parametros:
@   r0: o id do sonar a ser lido (0 a 15)
@ Retorno:
@   r0: distancia lida pelo sonar escolhido
read_sonar:
  push {r7, lr}

  @ fazemos a chamada do sistema
  mov r7, #16
  svc 0x0

  pop {r7, lr}
  mov pc, lr

@
@ Parametros:
@   r0: o id do primeiro sonar a ser lido (BEGIN)
@   r1: o id do ultimo sonar a ser lido (END)
@   r2: endereco da primeira posicao pro vetor onde serao salvos os sonares
@ Retorno:
@   void
read_sonars:
  push {r4-r7, lr}
  @ salva os valores iniciais
  mov r4, r0
  mov r5, r1
  mov r6, r2

  loop_inicio:
    mov r0, r4

    @ Identifica a syscall
    mov r7, #16
    svc 0x0

    str r0, [r6]

    add r4, r4, #1  @ atualiza o sonar a ser lido
    add r6, r6, #4  @ atualiza a posicao a ser salva no vetor
    cmp r4, r5      @ verifica se chegamos ao ultimo sonars

    bls loop_inicio @ se nao passamos do ultimo sonar a ser lido voltamos ao loop

  pop {r4-r7, lr}
  mov pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@                        TIME                                 @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@
@ Parametros:
@   r0: ponteiro para a variavel que ira receber o tempo do sistema
@ Retorno:
@   void
get_time:

  push {r4, r7, lr}

  @ passa para r4 o valor do endereco de memoria da variavel
  mov r4, r0

  mov r7, #20
  svc 0x0

  @ salva o retorno do r0 na posicao salva em r4
  str r0, [r4]

  pop {r4, r7, lr}
  mov pc, lr

@
@ Parametros:
@   r0: novo tempo do sistema
@ Retorno:
@   void
set_time:
  push {r7, lr}

  mov r7, #21
  svc 0x0

  pop {r7, lr}
  mov pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@                        CALLBACKS                            @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@
@ Parametros:
@   r0: ponteiro para a funcao a ser invocada quando o alarme der o trigger
@   r1: tempo para quando o alarme deve tocar
@ Retorno:
@   void
add_alarm:
  push {r7, lr}

  @ faz a chamada do sistema
  mov r7, #22
  svc 0x0

  pop {r7, lr}
  mov pc, lr

@
@ Parametros:
@   r0: id do sonar a ser monitorado
@   r1: distancia limite
@   r2: ponteiro para a funcao a ser chamada quando o robo estiver perto de um objeto
@ Retorno:
@   void
register_proximity_callback:
  push {r7, lr}

  @ executa a chamada de sistema
  mov r7, #17
  svc 0x0

  pop {r7, lr}
  mov pc, lr
