.globl _start

.data

input_buffer:   .skip 32
output_buffer:  .skip 32

.text
.align 4

@ Funcao inicial
_start:
    @ Chama a funcao "read" para ler 4 caracteres da entrada padrao
    ldr r0, =input_buffer
    mov r1, #5             @ 4 caracteres + '\n'
    bl  read
    mov r4, r0             @ copia o retorno para r4.

    @ Chama a funcao "atoi" para converter a string para um numero
    ldr r0, =input_buffer
    mov r1, r4
    bl  atoi

    @ Chama a funcao "encode" para codificar o valor de r0 usando
    @ o codigo de hamming.
    bl  encode
    mov r4, r0             @ copia o retorno para r4.

    @ Chama a funcao "itoa" para converter o valor codificado
    @ para uma sequencia de caracteres '0's e '1's
    ldr r0, =output_buffer
    mov r1, #7
    mov r2, r4
    bl  itoa

    @ Adiciona o caractere '\n' ao final da sequencia (byte 7)
    ldr r0, =output_buffer
    mov r1, #'\n'
    strb r1, [r0, #7]

    @ Chama a funcao write para escrever os 7 caracteres e
    @ o '\n' na saida padrao.
    ldr r0, =output_buffer
    mov r1, #8         @ 7 caracteres + '\n'
    bl  write

    @ Chama a funcao exit para finalizar processo.
    mov r0, #0
    bl  exit

@ Codifica o valor de entrada usando o codigo de hamming.
@ parametros:
@  r0: valor de entrada (4 bits menos significativos)
@ retorno:
@  r0: valor codificado (7 bits como especificado no enunciado).
encode:
      push {r4-r11, lr}

      @ aqui separamos os bits em cada registrador
      @r1 recebe o d4
      and r1, r0, #1
      @r2 recebe o d3
      and r2, r0, #2
      @ desloca d3 uma posicoes para direita
      lsr r2, #1
      @r3 recebe o d2
      and r3, r0, #4
      @ desloca d2 duas posicoes para direita
      lsr r3, #2
      @r4 recebe o d1
      and r4, r0, #8
      @ desloca d1 tres posicoes para direita
      lsr r4, #3

      @ agora encontramos os valores para p1, p2 e p3
      @ r5(p1), recebe o resultado de XOR entre r4(d1) e r3(d2)
      xor r5, r4, r3
      @ r5(p1), recebe o resultado de XOR entre r5(p1) e r1(d4)
      xor r5, r5, r1
      @ r6(p2), recebe o resultado de XOR entre r4(d1) e r2(d3)
      xor r6, r4, r2
      @ r6(p2), recebe o resultado de XOR entre r6(p2) e r1(d4)
      xor r6, r6, r4
      @ r7(p3), recebe o resultado de XOR entre r3(d2) e r2(d3)
      xor r7, r3, r2
      @ r7(p3), recebe o resultado de XOR entre r7(p3) e r1(d4)
      xor r7, r7, r1

      @ agora iremos deslocar os bits para que eles ocupem as posicoes que queremos
      @ d3(r2) deve ser deslocado em uma posicao
      lsl r2, #1
      @ d2(r3) deve ser deslocado em duas posicoes
      lsl r3, #2
      @ p3(r7) deve ser deslocado em tres posicoes
      lsl r7, #3
      @ d1(r4) deve ser deslocado em quatro posicoes
      lsl r4, #4
      @ p2(r6) deve ser deslocado em cinco posicoes
      lsl r6, #5
      @ p1(r5) deve ser deslocado em seis posicoes
      lsl r5, #6

      @ antes de recolocar os bits em r0 irei garantir que todos os bits em r0 estao zerados
      and r0, r0, #0

      @agora, para colocar os bits em r0, iremos fazer um OU entre r0 e todos os registradores
      orr r0, r1, r2
      orr r0, r0, r3
      orr r0, r0, r7
      orr r0, r0, r4
      orr r0, r0, r6
      orr r0, r0, r5

      pop  {r4-r11, lr}
      mov  pc, lr

@ Decodifica o valor de entrada usando o codigo de hamming.
@ parametros:
@  r0: valor de entrada (7 bits menos significativos)
@ retorno:
@  r0: valor decodificado (4 bits como especificado no enunciado).
@  r1: 1 se houve erro e 0 se nao houve.
decode:
      push {r4-r11, lr}

      @ primeiro iremos separar p1,p2,p3,d1,d2,d3 e d4 em registradores
      @ r1 recebera p1
      and r1, r0, #64
      @ deslocamos p1(r1) 6 posicoes para a direita
      lsr r1, #6
      @ r2 recebera p2
      and r2, r0, #32
      @ deslocamos p2(r2) 5 posicoes para a direita
      lsr r2, #5
      @ r3 recebera d1
      and r3, r0, #16
      @ deslocamos d1(r3) 4 posicoes para a direita
      lsr r3, #4
      @ r4 recebera p3
      and r4, r0, #8
      @ deslocamos p3(r4) 3 posicoes para a direita
      lsr r4, #3
      @ r5 recebera d2
      and r5, r0, #4
      @ deslocamos d2(r5) 2 posicoes para a direita
      lsr r4, #2
      @ r6 recebera d3
      and r6, r0, #2
      @ deslocamos d3(r6) 1 posicao para a direita
      lsr r4, #1
      @ r7 recebera d4
      and r7, r0, #1

      @ agora verificamos se a paridade dos bits esta correta
      @ primeiro verificamos a corretude da paridade de p1, fazendo: p1 XOR d1 XOR d2 XOR d4
      eor r1, r1, r3
      eor r1, r1, r5
      eor r1, r1, r7
      @ depois verificamos a corretude da paridade de p2, fazendo: p2 XOR d1 XOR d3 XOR d4
      eor r2, r2, r3
      eor r2, r2, r6
      eor r2, r2, r7
      @ por fim verificamos a corretude da paridade de p3, fazendo: p3 XOR d2 XOR d3 XOR d4
      eor r4, r4, r5
      eor r4, r4, r6
      eor r4, r4, r7
      @ em r8 teremos o resultado final entre todas as paridades
      and r8, #0
      orr r8, r1, r2
      orr r8, r8, r4
      @ agora passamos o resultado em r8 para r1 (pois eh o pedido no enunciado)
      and r1, #0
      orr r1, r1, r8

      @ uma vez que ja temos o resultado da verificacao da paridade em r1, iremos decodificar o numero e colocalo em r0
      @ primeiro temos que deslocar os bits para a posicao que eles devem ocupar
      lsl r3, #3
      lsl r5, #2
      lsl r6, #1
      @ uma vez que os bits ja estao na posicao correta, iremos setar 0 para todos os bits em r0
      and r0, r0, #0
      @ zerados os bits de r0, faremos um OR entre r0 e os registradores com os bits decodificados
      orr r0, r0, r3
      orr r0, r0, r5
      orr r0, r0, r6
      orr r0, r0, r7

      pop  {r4-r11, lr}
      mov  pc, lr

@ Le uma sequencia de bytes da entrada padrao.
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de bytes.
@  r1: numero maximo de bytes que pode ser lido (tamanho do buffer).
@ retorno:
@  r0: numero de bytes lidos.
read:
    push {r4,r5, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #0         @ stdin file descriptor = 0
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho maximo.
    mov r7, #3         @ read
    svc 0x0
    pop {r4, r5, lr}
    mov pc, lr

@ Escreve uma sequencia de bytes na saida padrao.
@ parametros:
@  r0: endereco do buffer de memoria que contem a sequencia de bytes.
@  r1: numero de bytes a serem escritos
write:
    push {r4,r5, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #1         @ stdout file descriptor = 1
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho do buffer.
    mov r7, #4         @ write
    svc 0x0
    pop {r4, r5, lr}
    mov pc, lr

@ Finaliza a execucao de um processo.
@  r0: codigo de finalizacao (Zero para finalizacao correta)
exit:
    mov r7, #1         @ syscall number for exit
    svc 0x0

@ Converte uma sequencia de caracteres '0' e '1' em um numero binario
@ parametros:
@  r0: endereco do buffer de memoria que armazena a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@ retorno:
@  r0: numero binario
atoi:
    push {r4, r5, lr}
    mov r4, r0         @ r4 == endereco do buffer de caracteres
    mov r5, r1         @ r5 == numero de caracteres a ser considerado
    mov r0, #0         @ number = 0
    mov r1, #0         @ loop indice
atoi_loop:
    cmp r1, r5         @ se indice == tamanho maximo
    beq atoi_end       @ finaliza conversao
    mov r0, r0, lsl #1
    ldrb r2, [r4, r1]
    cmp r2, #'0'       @ identifica bit
    orrne r0, r0, #1
    add r1, r1, #1     @ indice++
    b atoi_loop
atoi_end:
    pop {r4, r5, lr}
    mov pc, lr

@ Converte um numero binario em uma sequencia de caracteres '0' e '1'
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@  r2: numero binario
itoa:
    push {r4, r5, lr}
    mov r4, r0
itoa_loop:
    sub r1, r1, #1         @ decremento do indice
    cmp r1, #0          @ verifica se ainda ha bits a serem lidos
    blt itoa_end
    and r3, r2, #1
    cmp r3, #0
    moveq r3, #'0'      @ identifica o bit
    movne r3, #'1'
    mov r2, r2, lsr #1  @ prepara o proximo bit
    strb r3, [r4, r1]   @ escreve caractere na memoria
    b itoa_loop
itoa_end:
    pop {r4, r5, lr}
    mov pc, lr
