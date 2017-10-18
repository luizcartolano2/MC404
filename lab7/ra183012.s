@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@  Aluno: Luiz Eduardo Cartolano   @
@  RA: 183012                      @
@  Turma: B                        @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.globl _start

.data

input_buffer:   .skip 32
output_buffer:  .skip 32

.text
.align 4

_start: @ funcao que calcula o triangulo de Pascal

  @ Chama a funcao "read" para ler 4 caracteres da entrada padrao
  ldr r0, =input_buffer
  mov r1, #4             @ 3 caracteres + '\n'
  bl  read
  mov r4, r0             @ copia o retorno para r4.

  @ chama a funcao "atoi" para converter a string lida em um numero
  ldr r0, =input_buffer
  mov r4, r1
  bl atoi

  mov r1, #0 @ i = 0
  for_i:
    cmp r1, r0 @ r0 = numero de linhas do triangulo de Pascal
    bge end_pascal
    mov r2, #0 @ j = 0
    mov r3, #1 @r3 = c = 1

    for_j:
      cmp r2, r1 @ j <= i
      bgt end_for

      @ printa c (descobrir como fazer isso)

      ldr r7, =output_buffer
      mov r8, #8
      mov r9, r3
      bl  itoa @ Chama a funcao "itoa" para converter o valor codificado

      ldr r7, =output_buffer
      mov r8, #' ' @ Adiciona o caractere '\n' ao final da sequencia (byte 7)
      strb r8, [r7, #8]

      ldr r7, =output_buffer
      mov r8, #9         @ 8 caracteres + '\n'
      bl  write @ Chama a funcao write

      sub r4, r1, r2 @ r4 = i - j
      add r5, r5, #1 @ r5 = j + 1
      mul r6, r3, r4 @ r6 = c * (i - j)
      bl divisao @ divide r6 por r5
      mov r3, r7 @ salva o resultado da divisao em r3
      add r2, r2, #1 @ j = j + 1
      b for_j

    end_for:
    @ printa um "\n" sabe se Deus como

    @ Adiciona o caractere '\n' ao final da sequencia (byte 7)
    ldr r7, =output_buffer
    mov r8, #'\n'

    ldr r7, =output_buffer
    mov r8, #1         @ '\n'
    bl  write

    add r1, r1, #1 @ i = i + 1
    b for_i

  end_pascal: @ ja temos todo o triangulo de Pascal

  @ Chama a funcao exit para finalizar processo.
  mov r0, #0
  bl  exit

@ funcao que divide dois numeros
@ parametros:
@   r6: numero a ser dividido
@   r5: numero pelo qual r6 vai ser dividido
@ retorno:
@   r7: resultado da divisao
divisao:
  mov r8, r6 @ divide r6
  mov r9, r5 @ divide por r5
  mov r7, #0 @ resultado da divisao

  subtract:
  subs r8, r8, r9 @ r8 = r8 - r9
  add r7, r7, #1 @ r7 = r7 + 1
  bhi subtract

  mov pc, lr

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
@  r7: endereco do buffer de memoria que contem a sequencia de bytes.
@  r8: numero de bytes a serem escritos
write:
  push {r4,r5, lr}
  mov r4, r7
  mov r5, r8
  mov r7, #1         @ stdout file descriptor = 1
  mov r8, r4         @ endereco do buffer
  mov r9, r5         @ tamanho do buffer.
  mov r10, #4         @ write
  svc 0x0
  pop {r4, r5, lr}
  mov pc, lr

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
    mov r0, r0, lsl #4
    ldrb r2, [r4, r1]

    cmp r2, #'0'      @ identifica bit 0
      addeq r0, r0, #0x0
    cmp r2, #'1'      @ identifica bit 1
      addeq r0, r0, #0x1
    cmp r2, #'2'      @ identifica bit 2
      addeq r0, r0, #0x2
    cmp r2, #'3'      @ identifica bit 3
      addeq r0, r0, #0x3
    cmp r2, #'4'      @ identifica bit 4
      addeq r0, r0, #0x4
    cmp r2, #'5'      @ identifica bit 5
      addeq r0, r0, #0x5
    cmp r2, #'6'      @ identifica bit 6
      addeq r0, r0, #0x6
    cmp r2, #'7'      @ identifica bit 7
      addeq r0, r0, #0x7
    cmp r2, #'8'      @ identifica bit 8
      addeq r0, r0, #0x8
    cmp r2, #'9'      @ identifica bit 9
      addeq r0, r0, #0x9
    cmp r2, #'A'      @ identifica bit A
      addeq r0, r0, #0xA
    cmp r2, #'B'      @ identifica bit B
      addeq r0, r0, #0xB
    cmp r2, #'C'      @ identifica bit C
      addeq r0, r0, #0xC
    cmp r2, #'D'      @ identifica bit D
      addeq r0, r0, #0xD
    cmp r2, #'E'      @ identifica bit E
      addeq r0, r0, #0xE
    cmp r2, #'F'      @ identifica bit F
      addeq r0, r0, #0xF

    add r1, r1, #1     @ indice++
    b atoi_loop
atoi_end:
    pop {r4, r5, lr}
    mov pc, lr

@ Converte um numero binario em uma sequencia de caracteres '0' e '1'
@ parametros:
@  r7: endereco do buffer de memoria que recebera a sequencia de caracteres.
@  r8: numero de caracteres a ser considerado na conversao
@  r9: numero binario
itoa:
    push {r3, r4, r5, lr}
    mov r4, r7
itoa_loop:
    sub r8, r8, #1         @ decremento do indice
    cmp r8, #0          @ verifica se ainda ha bits a serem lidos
    blt itoa_end

    and r10, r9, #1
    cmp r10, #0
    moveq r10, #'0'      @ identifica o bit

    cmp r10, #'0'      @ identifica bit 0
      moveq r10, #0x0
    cmp r10, #'1'      @ identifica bit 1
      moveq r10, #0x1
    cmp r10, #'2'      @ identifica bit 2
      moveq r10, #0x2
    cmp r10, #'3'      @ identifica bit 3
      moveq r10, #0x3
    cmp r10, #'4'      @ identifica bit 4
      moveq r10, #0x4
    cmp r10, #'5'      @ identifica bit 5
      moveq r10, #0x5
    cmp r10, #'6'      @ identifica bit 6
      moveq r10, #0x6
    cmp r10, #'7'      @ identifica bit 7
      moveq r10, #0x7
    cmp r10, #'8'      @ identifica bit 8
      moveq r10, #0x8
    cmp r10, #'9'      @ identifica bit 9
      moveq r10, #0x9
    cmp r10, #'A'      @ identifica bit A
      moveq r10, #0xA
    cmp r10, #'B'      @ identifica bit B
      moveq r10, #0xB
    cmp r10, #'C'      @ identifica bit C
      moveq r10, #0xC
    cmp r10, #'D'      @ identifica bit D
      moveq r10, #0xD
    cmp r10, #'E'      @ identifica bit E
      moveq r10, #0xE
    cmp r10, #'F'      @ identifica bit F
      moveq r10, #0xF

    mov r9, r9, lsr #4  @ prepara o proximo bit
    strb r10, [r4, r8]   @ escreve caractere na memoria
    b itoa_loop
itoa_end:
    pop {r3, r4, r5, lr}
    mov pc, lr

@ Finaliza a execucao de um processo.
@  r0: codigo de finalizacao (Zero para finalizacao correta)
exit:
    mov r7, #1         @ syscall number for exit
    svc 0x0
