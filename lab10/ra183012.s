.org 0x0
.section .iv,"a"

_start:

interrupt_vector:
  b RESET_HANDLER
.org 0x18
  b IRQ_HANDLER

.org 0x100
.text

RESET_HANDLER:

  @ Zera o contador
  ldr r2, =CONTADOR  @lembre-se de declarar esse contador em uma secao de dados!
  mov r0, #0
  str r0, [r2]

  @Faz o registrador que aponta para a tabela de interrupções apontar para a tabela interrupt_vector
  ldr r0, =interrupt_vector
  mcr p15, 0, r0, c12, c0, 0

  @ Ajustar a pilha do modo IRQ.
  msr CPSR_c, #0x12
  ldr sp, =pilha_irq
  msr CPSR_c, #0b00010011

  @ Você deve iniciar a pilha do modo IRQ aqui. Veja abaixo como usar a instrução MSR para chavear de modo.
  bl SET_GPT
  bl SET_TZIC

  @ habilita as interrupcoes do ARM
  msr  CPSR_c,  #0x13   @ SUPERVISOR mode, IRQ/FIQ enabled

@ funcao IRQ_HANDLER
IRQ_HANDLER:
  .set GPT_SR, 0x53FA0008

  @ informa que o processador ja esta ciente da interrupcao
  mov r0, #1
  ldr r1,=GPT_SR
  str r0, [r1]

  @ incrementa 1 ao contador
  ldr r1,=CONTADOR
  ldr r0, [r1]
  add r0, r0, #1
  str r0, [r1]

  @ corrige o valor de lr
  sub lr, lr, #4
  movs pc, lr

@ funcao que seta as configuracoes do GPT
SET_GPT:
  push {r4}
  @ Constantes para os enderecos do GPT
  .set GPT_CR, 0x0
  .set GPT_PR, 0x4
  .set GPT_OCR1, 0x10
  .set GPT_IR, 0xC
  .set GPT_BASE, 0x53FA0000

  ldr r4,=GPT_BASE

  @ habilita o o clock no GPT_CR
  mov r0, #0x41
  str r0, [r4,#GPT_CR]

  @ zera o prescaler(GPT_PR) e salva em GPT_OCR1 o que eu quero salvar
  mov r0, #0
  str r0, [r4,#GPT_PR]
  mov r0, #100
  str r0, [r4,#GPT_OCR1]

  @ grava 1 no GPT_IR para habilitar a interrupcao compare_channel
  mov r0, #1
  str r0, [r4,#GPT_IR]

  pop {r4}

  mov pc, lr

@ funcao que seta as configuracoes do TZIC
SET_TZIC:
  @ Constantes para os enderecos do TZIC
  .set TZIC_BASE,             0x0FFFC000
  .set TZIC_INTCTRL,          0x0
  .set TZIC_INTSEC1,          0x84
  .set TZIC_ENSET1,           0x104
  .set TZIC_PRIOMASK,         0xC
  .set TZIC_PRIORITY9,        0x424

  @ Liga o controlador de interrupcoes
  @ R1 <= TZIC_BASE
  ldr	r1, =TZIC_BASE

  @ Configura interrupcao 39 do GPT como nao segura
  mov	r0, #(1 << 7)
  str	r0, [r1, #TZIC_INTSEC1]

  @ Habilita interrupcao 39 (GPT)
  @ reg1 bit 7 (gpt)

  mov	r0, #(1 << 7)
  str	r0, [r1, #TZIC_ENSET1]

  @ Configure interrupt39 priority as 1
  @ reg9, byte 3

  ldr r0, [r1, #TZIC_PRIORITY9]
  bic r0, r0, #0xFF000000
  mov r2, #1
  orr r0, r0, r2, lsl #24
  str r0, [r1, #TZIC_PRIORITY9]

  @ Configure PRIOMASK as 0
  eor r0, r0, r0
  str r0, [r1, #TZIC_PRIOMASK]

  @ Habilita o controlador de interrupcoes
  mov	r0, #1
  str	r0, [r1, #TZIC_INTCTRL]

  @instrucao msr - habilita interrupcoes
  msr  CPSR_c, #0x13       @ SUPERVISOR mode, IRQ/FIQ enabled

  mov pc, lr

@ loop infinito para aguardar a proxima interrupcao
aguarda_interrupcao:
  b aguarda_interrupcao

@ Declaracao do contador no trecho .data do codigo
.data
CONTADOR:
  .word 0
.skip 100
pilha_irq:
.skip 100
