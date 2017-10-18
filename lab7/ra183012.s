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

@ Funcao inicial
_start:

@ Chama a funcao "read" para ler 3 caracteres da entrada padrao
    ldr r0, =input_buffer
    mov r1, #4             @ 3 caracteres + '\n'
    bl  read

@ Chama a funcao "atoi" para converter a string para um numero
    mov r1, r0
    ldr r0, =input_buffer
    bl atoi

@ LOOP

@ r0 := n
@ r1 := i
@ r2 := j
@ r3 := c

	mov r1, #0

	for_i:
  cmp r1, r0
	bge end_for_i
		push {r1}

		mov r3, #1
		mov r2, #0

		for_j:
    cmp r2, r1
		bgt end_for_j
			push {r2}

			@ print c
			bl print_c

			sub r5, r1, r2
			mul r4, r5, r3

			mov r5, #0
			add r2, r2, #1

			div:
        cmp r4, r2
			  subge r4, r4, r2
			  addge r5, r5, #1
			  bge div

			sub r2, r2, #1
			mov r3, r5

			pop {r2}
			add r2, r2, #1
			b for_j
		end_for_j:

		@ print '\n'
		bl print_new_line

		pop {r1}
		add r1, r1, #1
		b for_i
	end_for_i:

	mov r0, #0
	mov r7, #1
	svc 0x0


@ Le uma sequencia de bytes da entrada padrao.
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de bytes.
@  r1: numero maximo de bytes que pode ser lido (tamanho do buffer).
@ retorno:
@  r0: numero de bytes lidos.
read:
    push {r1-r11, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #0         @ stdin file descriptor = 0
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho maximo.
    mov r7, #3         @ read
    svc 0x0
    pop {r1-r11, lr}
    mov pc, lr


@ Escreve uma sequencia de bytes na saida padrao.
@ parametros:
@  r0: endereco do buffer de memoria que contem a sequencia de bytes.
@  r1: numero de bytes a serem escritos
write:
    push {r0-r11, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #1         @ stdout file descriptor = 1
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho do buffer.
    mov r7, #4         @ write
    svc 0x0
    pop {r0-r11, lr}
    mov pc, lr

@ Converte uma sequencia de caracteres hexadecimais em um numero binario
@ parametros:
@  r0: endereco do buffer de memoria que armazena a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@ retorno:
@  r0: numero binario
atoi:
    push {r1-r11, lr}
    mov r4, r0         @ r4 == endereco do buffer de caracteres
    mov r5, r1         @ r5 == numero de caracteres a ser considerado
    mov r0, #0         @ number = 0
    mov r1, #0         @ loop indice
atoi_loop:
    cmp r1, r5         @ se indice == tamanho maximo
    beq atoi_end       @ finaliza conversao

    mov r0, r0, lsl #4
    ldrb r2, [r4, r1]

    cmp r2, #'0'
    orreq r0, r0, #0
    cmp r2, #'1'
    orreq r0, r0, #1
    cmp r2, #'2'
    orreq r0, r0, #2
    cmp r2, #'3'
    orreq r0, r0, #3
    cmp r2, #'4'
    orreq r0, r0, #4
    cmp r2, #'5'
    orreq r0, r0, #5
    cmp r2, #'6'
    orreq r0, r0, #6
    cmp r2, #'7'
    orreq r0, r0, #7
    cmp r2, #'8'
    orreq r0, r0, #8
    cmp r2, #'9'
    orreq r0, r0, #9
    cmp r2, #'A'
    orreq r0, r0, #10
    cmp r2, #'B'
    orreq r0, r0, #11
    cmp r2, #'C'
    orreq r0, r0, #12
    cmp r2, #'D'
    orreq r0, r0, #13
    cmp r2, #'E'
    orreq r0, r0, #14
    cmp r2, #'F'
    orreq r0, r0, #15

    add r1, r1, #1     @ indice++
    b atoi_loop
atoi_end:
    pop {r1-r11, lr}
    mov pc, lr

@ Print new line
@ imprime '\n'
print_new_line:
	push {r0-r11, lr}

	ldr r0, =output_buffer
	mov r1, #'\n'
	strb r1, [r0, #0]
	mov r1, #1
	bl write

	pop {r0-r11, lr}
	mov pc, lr

@ Print c
@ imprime valor em r3
print_c:
	push {r0-r11, lr}

  mov r4, r1
  mov r5, r2

	ldr r0, =output_buffer
	mov r1, #8
	mov r2, r3
	bl itoa

  cmp r4, r5

	ldr r0, =output_buffer
	movne r1, #' '
	strneb r1, [r0, #8]
	movne r1, #9
  moveq r1, #8
	bl write

	pop {r0-r11, lr}
	mov pc, lr

@ Converte um numero binario em uma sequencia de caracteres hexadecimais
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@  r2: numero binario
itoa:
    push {r0-r11, lr}
    mov r4, r0
itoa_loop:
    sub r1, r1, #1         @ decremento do indice
    cmp r1, #0          @ verifica se ainda ha bits a serem lidos
    blt itoa_end

    and r3, r2, #0xF

    cmp r3, #0
    moveq r3, #'0'
    cmp r3, #1
    moveq r3, #'1'
    cmp r3, #2
    moveq r3, #'2'
    cmp r3, #3
    moveq r3, #'3'
    cmp r3, #4
    moveq r3, #'4'
    cmp r3, #5
    moveq r3, #'5'
    cmp r3, #6
    moveq r3, #'6'
    cmp r3, #7
    moveq r3, #'7'
    cmp r3, #8
    moveq r3, #'8'
    cmp r3, #9
    moveq r3, #'9'
    cmp r3, #10
    moveq r3, #'A'
    cmp r3, #11
    moveq r3, #'B'
    cmp r3, #12
    moveq r3, #'C'
    cmp r3, #13
    moveq r3, #'D'
    cmp r3, #14
    moveq r3, #'E'
    cmp r3, #15
    moveq r3, #'F'

    mov r2, r2, lsr #4  @ prepara o proximo bit

    strb r3, [r4, r1]   @ escreve caractere na memoria
    b itoa_loop
itoa_end:
    pop {r0-r11, lr}
    mov pc, lr
