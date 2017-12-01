#include "api_robot2.h"

// funcoes para a Ronda
void ronda();
void vira_direita();
void anda_frente();
void forca_virar();

// funcao auxiliar
void seta_motor(int motor0_v, int motor1_v);

void _start() {
  ronda();
  while(1);
}

/*
 * Inicia o modo ronda do Uoli
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void ronda () {
  register_proximity_callback(3,200,forca_virar);
  register_proximity_callback(4,200,forca_virar);
  anda_frente();
}

/*
 * Faz o Uoli andar para frente, e ao mesmo tempo seta um alarme para ele virar depois de um certo tempo
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void anda_frente () {
  int tempo_sistema;
  int constante = 50;

  seta_motor(30,30);

  get_time(&tempo_sistema);
  tempo_sistema += constante;

  add_alarm(vira_direita,tempo_sistema);

   constante = (constante % 1000) + 50;

}

/*
 * Faz o robo virar a direita por um certo tempo
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void vira_direita () {
  int t;
  get_time(&t);

  seta_motor(0,5);

  int i;
  for (i = 0; i < t + 150; i++);

  anda_frente();

}

/*
 * Forca o robo a virar a direita em caso de emergencia e seta o callback
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void forca_virar () {
  int t;
  get_time(&t);

  seta_motor(0,5);

  int i;
  for (i = 0; i < t + 150; i++);

  register_proximity_callback(3,1200,forca_virar);
  register_proximity_callback(4,1200,forca_virar);

  anda_frente();

}

/*
 * Funcao auxiliar para setar as velocidades dos motores
 * Parameter:
 *   motor0_v: velocidade para o motor0
 *   motor1_v: velocidade para o motor1
 * Returns:
 *   void
 */
void seta_motor (int motor0_v, int motor1_v) {
  // declaracao dos motores do Uoli
  motor_cfg_t motor0, motor1;

  motor0.id = 0;
  motor1.id = 1;
  motor0.speed = motor0_v;
  motor1.speed = motor1_v;

  set_motors_speed(&motor0,&motor1);

}
