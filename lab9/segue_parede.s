.text
.align 4
.globl _start

_start:                   @ main

  ldr r6, =1200           @ r6 <- 1200 (Limiar para parar o robo).

  bl acha_parede
  bl vira_direita
  main_loop:
    bl verifica_esquerda
    cmp r0, #0
    beq acha_parede
    beq vira_direita
    bl anda_frente
    b main_loop

acha_parede:
  push {lr}
  while:
    mov r0, #3              @ Define em r0 o identificador do sonar a ser consultado.
    mov r7, #125            @ Identifica a syscall 125 (read_sonar).
    svc 0x0
    mov r5, r0              @ Armazena o retorno da syscall.

    mov r0, #4              @ Define em r0 o sonar.
    mov r7, #125
    svc 0x0

    cmp r5, r0              @ Compara o retorno (em r0) com r5.
    movlt r0, r5            @ Se r5 < r0, r0 <-- r5
    cmp r0, r6
    blt para                @ Se r0 < r6, para o robo
                            @ Senao anda ele pra frente
    mov r0, #36
    mov r1, #36
    mov r7, #124
    svc 0x0
    b while

para:
    mov r0, #0
    mov r1, #0
    mov r7, #124
    svc 0x0
    pop {pc}

vira_direita:
    push {lr}
    while_dir:
      mov r0, #3              @ Define em r0 o identificador do sonar a ser consultado.
      mov r7, #125            @ Identifica a syscall 125 (read_sonar).
      svc 0x0
      mov r5, r0              @ Armazena o retorno da syscall.

      mov r0, #4              @ Define em r0 o sonar.
      mov r7, #125
      svc 0x0

      cmp r5, r0              @ Compara o retorno (em r0) com r5.
      movlt r0, r5            @ Se r5 < r0, r0 <-- r5
      cmp r0, r6
      bge para                @ se r0 >= r6, a frente ta livre entao pode ir pra frente
      mov r0, #0              @ Define uma velocidade igual a zero para o motor direito.
      mov r1, #6              @ Define uma velocidade diferente de zero para o motor esquerdo.
      mov r7, #124            @ Identifica a syscall 124 (write_motors).
      svc 0x0
      b while_dir

verifica_esquerda:
      push {lr}
      mov r0, #1              @ Define em r0 o identificador do sonar a ser consultado.
      mov r7, #125            @ Identifica a syscall 125 (read_sonar).
      svc 0x0
      mov r5, r0              @ Salva o retorno da syscall em r5.

      mov r0, #0              @ Define em r0 o identificador do sonar a ser consultado.
      mov r7, #125            @ Identifica a syscall 125 (read_sonar).
      svc 0x0

      cmp r5, r0              @ Compara os valores lidos pelos leitores 0 e 1, a fim de Identificar possiveis
                              @ obstaculos que estejam somente na diagonal do robo.

      movlt r0, r5            @ Armazena o menor valor lido (obstaculo mais proximo) em r0.
      cmp r0, r6
      movlt r0, #1            @ Se tiver alguma coisa obstruindo os sensores da esquerda ele ta certo
                              @ Senao, achamos uma parede a esquerda pra ele seguir
      movge r0, #0
      pop {pc}

anda_frente:
  push {lr}
  while_frente:
    mov r0, #3              @ Define em r0 o identificador do sonar a ser consultado.
    mov r7, #125            @ Identifica a syscall 125 (read_sonar).
    svc 0x0
    mov r5, r0              @ Armazena o retorno da syscall.

    mov r0, #4              @ Define em r0 o sonar.
    mov r7, #125
    svc 0x0

    cmp r5, r0              @ Compara o retorno (em r0) com r5.
    movlt r0, r5            @ Se r5 < r0, r0 <-- r5
    cmp r0, r6
    blt para                @ se r0 < r6, a frente ta obstruida entao para
    mov r0, #36
    mov r1, #36
    mov r7, #124            @ Identifica a syscall 124 (write_motors).
    svc 0x0
    b while_frente
