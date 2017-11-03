.data
  detect: .skip 64
  braitenbergL: .byte 20
                .byte 40
                .byte 60
                .byte 80
                .byte 100
                .byte 120
                .byte 140
                .byte 160
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
  braitenbergR: .byte 160
                .byte 140
                .byte 120
                .byte 100
                .byte 80
                .byte 60
                .byte 40
                .byte 20
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
                .byte 0
.text
.align 4
.globl _start


  ldr r9, =detect        @ armazena a posicao inicial do vetor dos sensores
  ldr r10, =braitenbergL  @ armazena a posicao inicial do vetor braitenbergL
  ldr r11, =braitenbergR  @ armazena a posicao inicial do vetor braitenbergR

  loop:
    push {r9}               @ salv na pilha a posicao inicial do sensor de memoria
    mov r3, #0 @i = 0
    for_i_sensor:
      cmp r3, #16 @ i < 16
      bge fim_i_sensor
      @ leio os sensores
      mov r0, r3              @ Define em r0 o identificador do sonar a ser consultado.
      mov r7, #125            @ Identifica a syscall 125 (read_sonar).
      svc 0x0

      cmp r0, #500           @ comparo a distancia com minha no_Detection_dist
      bge else_NoDetection
        cmp r0, #200         @ comparo a distancia com minha distancia maxima
        movlt r8, #0
        strlt r8, [r9]        @ se a distancia for menor q a maxima salvamos zero no vetor detect
        subge r4, r0, #200   @ (dist-maxDetectionDist)
        movge r5, #150       @ (noDetectionDist-maxDetectionDist)
        bl div
        movge r8, #1          @ r8 = 1
        subge r8, r8, r6      @ r8 = r8 - r6 = 1-((dist-maxDetectionDist)/(noDetectionDist-maxDetectionDist))
        strge r8, [r9]        @ salva detect[i]
        add r9, r9, #4
        add r3, r3, #1
        b for_i_sensor
      else_NoDetection:
        mov r8, #0            @ r8 = 0
        strge r9, [r8]        @ salva detect[i]
        add r9, r9, #4
        add r3, r3, #1
        b for_i_sensor
    fim_i_sensor:
    pop {r9}

    mov r0, #2                 @ v0_d = 2
    mov r1, #2                 @ v0_e = 2

    mov r3, #0                 @ i = 0
    for_i_velocidade:
      cmp r3, #16              @ i < 16
      ldr r8, [r9]             @ carrega em r8 o sensor i
      add r9, r9, #4           @ atualiza o apontador do vetor detect

      ldr r4, [r10]            @ carrega braitenbergL da memoria
      push {r6}
      add r6, r4, r4
      sub r4, r4, r6
      pop {r6}
      add r10, r10, #1

      ldr r5, [r11]            @ carrega braitenbergR da memoria
      push {r6}
      add r6, r5, r5
      sub r5, r5, r6
      pop {r6}
      add r11, r11, #1

      add r0, r0, r5
      mov r5, r0
      mul r0, r5, r8

      add r1, r1, r4
      mov r4, r1
      mul r1, r4, r8

      add r3, r3, #1
      b for_i_velocidade

    fim_i_velocidade:
    mov r7, #124            @ Identifica a syscall 124 (write_motors).
    svc 0x0

  b loop

div:
  push {r4-r6, lr}
  div_for:
    cmp r4, r5
    subge r4, r4, r5
    addge r6, r6, #1
    bge div_for
  pop {r4-r6, lr}
  mov pc, lr
