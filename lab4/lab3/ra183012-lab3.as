.org 0x000
  laco:
    LOAD M(var_vetor_um)
    STA M(mem1)
    LOAD M(var_vetor_dois)
    STA M(mem2)
    mem1:
      LOAD MQ,M(000)
    mem2:
      MUL M(000)
    LOAD MQ
    ADD M(var_somatorio)
    STOR M(var_somatorio)
    LOAD M(var_vetor_um)
    ADD M(constate_um)
    STOR M(var_vetor_um)
    LOAD M(var_vetor_dois)
    ADD M(constate_um)
    STOR M(var_vetor_dois)
    LOAD M(var_contador)
    SUB M(constate_um)
    STOR M(var_contador)
    JUMP+ M(laco)
  LOAD M(var_somatorio)
  JUMP M(0x400)

.org 0x064
  var_somatorio:
    .word 0000000000
  constate_um:
    .word 0000000001

.org 0x3FD
  var_vetor_um:
    .word 0000000000
  var_vetor_dois:
    .word 0000000000
  var_contador:
    .word 0000000000
