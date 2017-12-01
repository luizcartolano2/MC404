.global ajudaORobinson

.data

output_buffer:  .skip 32

.text
.align 4

ajudaORobinson:
  @ primeiro devemos inicilizar a matriz de posicoes visitadas com zero
  bl inicializaVisitados

  bl posicaoXLocal
  mov r8, r0 @ r8 = posicao XLocal
  bl posicaoYLocal
  mov r9, r0 @ r9 = posicao YLocal
  bl posicaoXRobinson
  mov r10, r0 @ r10 = posicao XRobinson
  bl posicaoYRobinson
  mov r11, r0 @ r11 = posicao YRobinson

  bl DFS

  mov pc, lr

DFS:


@
@ Parametros
@   r0: posicao X
@   r1: posicao Y
@ Retorno:
@   r0: 0 - se invalido
@
verificaPosicao:
  push {r4, lr}
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
