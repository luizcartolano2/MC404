.global ajudaORobinson

.data

output_buffer:  .skip 32

.text
.align 4

ajudaORobinson:
  @ primeiro devemos inicilizar a matriz de posicoes visitadas com zero
  bl inicializaVisitados

  @ salva a posicao final para que possamos compara-la depois
  bl posicaoXLocal
  mov r9, r0
  bl posicaoYLocal
  mov r9, r0

  @ chama a funcao q encontra a Pos Y do Robinson
  bl posicaoYRobinson
  mov r1, r0 @ r1 = y
  @ chama a funcao q encontra a Pos X do Robinson
  bl posicaoXRobinson
  @ coloca na pilha os valores da posicao onde o Robinson esta
  push{r0-r1}

  bl DFS_First_Search_Kickoff


@ Parametros:
@   r0 :
@
DFS_First_Search_Kickoff:
  push {lr}

  bl visitaCelula

  @ verifica se estamos no X local
  cmp r0, r9
    @ se a posicao x for diferente pulamos para a funcao que verifica as posicoes adjacentes
    bne DFS_First_Search
  cmp r1, r10
    @ se a posicao y for diferente pulamos para a funcao que verifica as posicoes adjacentes
    bne DFS_First_Search

  mov r0, #1

  pop {lr}
  mov pc, lr


@
@ Parametros:
@   r0 : posicaoXRobinson
@   r1 : posicaoYRobinson
@
DFS_First_Search:
  push {r4, lr}

  mov r2, r0 @ r2 = x
  mov r3, r1 @ r3 = y

  @ chama a recursao para a posicao dada por (x-1,y-1)
  sub r0, r2, #1
  sub r0, r3, #1
  @ chama a funcao que verifica a validade da celula a ser visitada
  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  @ se a celula ainda n foi visitada, visitamos ela
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  @ chama a recursao para a posicao dada por (x-1,y)
  sub r0, r2, #1

  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  @ chama a recursao para a posicao dada por (x-1,y+1)
  sub r0, r2, #1
  add r1, r3, #1

  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  @ chama a recursao para a posicao dada por (x,y-1)
  sub r1, r3, #1

  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  @ chama a recursao para a posicao dada por (x,y+1)
  add r1, r3, #1

  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  @ chama a recursao para a posicao dada por (x+1,y-1)
  add r0, r2, #1
  sub r1, r3, #1

  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  @ chama a recursao para a posicao dada por (x+1,y)
  add r0, r2, #1

  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  @ chama a recursao para a posicao dada por (x+1,y+1)
  add r0, r2, #1
  add r1, r3, #1

  push {r0}
  bl verificaPosicao
  mov r4, r0 @ salva o retorno em r4
  pop {r0}
  cmp r4, #0
  push {r2,r3}
  bne DFS_First_Search_Kickoff
  pop {r2,r3}
  cmpne r0, #1
  @ achamos um caminho, entao chamamos a funcao que printa a celula


  mov r0, #0
  pop {r4, lr}
  mov pc, lr

@
@ Parametros
@   r0: posicao X
@   r1: posicao Y
@ Retorno:
@   r0: 0 - se invalido
@
verificaPosicao:
  push {r0, r1, r4, lr}
  mov r2, r0 @r2 = x, r1 = y

  @ se a posica sai dos limites do mapa nao visitamos a mesma
  bl daParaPassar
  mov r4, #0
  cmp r0, r4
  mov r0, #0
  pop {r4, pc}

  bl foiVisitado
  mov r4, #1

  @ se a posicao ja foi visitada retornamos 0
  cmp r0, r4
  moveq r0, #0
  pop {r4, pc}

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


@ Converte um numero binario em uma sequencia de caracteres hexadecimais
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@  r2: numero binario
itoa:
    push {r0-r11, lr}
    mov r4, r0
itoa_loop:
    sub r1, r1, #1      @ decremento do indice
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

    mov r2, r2, lsr #1  @ prepara o proximo bit

    strb r3, [r4, r1]   @ escreve caractere na memoria
    b itoa_loop
itoa_end:
    pop {r0-r11, lr}
    mov pc, lr
