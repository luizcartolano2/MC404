.org 0x000
  LOAD MQ,M(var_g)
  MUL M(var_v)
  LOAD MQ
  STOR M(var_x)
  RSH
  STOR M(var_k)
  laco:
    LOAD M(var_x)
    DIV M(var_k)
    LOAD MQ
    ADD M(var_k)
    RSH
    STOR M(var_k)
    LOAD M(var_c)
    SUB M(var_um)
    STOR M(var_c)
    JUMP+ M(laco)
  LOAD M(var_k)
  JUMP M(0x400)

.org 0x105
  # velocidade
  var_v:
    .word 0000000010

.org 0x301
  # gravidade
  var_g:
    .word 0000000010
  # k
  var_k:
    .word 0000000000
  # contador
  var_c:
    .word 0000000009
  # constante 1
  var_um:
    .word 0000000001
  # variavel
  var_x:
    .word 0000000000
