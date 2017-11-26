/*
* Aluno: Luiz Eduardo Cartolano
* RA: 183012
* Turma MC 404 - B
*
* Interface de controle do Uoli
* Posicao dos sonares:       s03  s04
*                         s02      s05
*                        s01        s06
*                       s00          s07
*                   Motor 1          Motor 0
*                       s15          s08
*                        s14        s09
*                         s13      s10
*                           s12  s11
*/
#include "api_robot2.h"

// Segue parede
void segue_parede();
void busca_parede();
void modo_segue_parede();

// funcoes auxiliares
void para_robo();
void seta_motor(int motor0_v, int motor1_v);
void vira_robo_dir();

/*
 * Faz o Uoli encontrar uma parede para seguir e depois segui-la
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void _start(void) {
  // encontra uma parede para seguir
  busca_parede();

  // segue a parede encontrada
  modo_segue_parede();

  while (1);

}

/*
 * Busca uma parede que sera seguida pelo Uoli
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void busca_parede() {

  // faremos a leitura dos sonares frontais para iniciar o modo busca_parede
  unsigned short int sonar_3 = read_sonar(3);
  unsigned short int sonar_4 = read_sonar(4);

  // colocamos velocidade igual nos dois motores para que o Uoli ande para frente
  seta_motor(15,15);

  // enquanto os dois sonares frontais estiverem livres o robo anda pra frente
  while (sonar_3 >= 800 && sonar_4 >= 800) {
    // refazemos a leitura dos sonares
    sonar_3 = read_sonar(3);
    sonar_4 = read_sonar(4);
  }

  // quandos sair do loop eh porque encontramos uma parede, entao, paramos o robo
  para_robo();

}

/*
 * Segue a parede que foi encontrada anteriormente
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void modo_segue_parede() {

  vira_robo_dir();

  unsigned int sonar_00, sonar_01, sonar_02, sonar_03, sonar_04, sonar_05, sonar_06;

  do {
    // tem que estar ocupados
    sonar_00 = read_sonar(0);
    sonar_01 = read_sonar(1);
    sonar_02 = read_sonar(2);

    // tem que estar livres
    sonar_03 = read_sonar(3);
    sonar_04 = read_sonar(4);
    sonar_05 = read_sonar(5);
    sonar_06 = read_sonar(6);

    if (sonar_03 < 550 || sonar_04 < 550 || sonar_05 < 550 || sonar_06 < 550) {
      seta_motor(0, 2);
    } else if (sonar_00 < 370 || sonar_01 < 370 || sonar_02 < 370) {
      seta_motor(0, 2);
    } else if (sonar_00 > 550 || sonar_01 > 550) {
      seta_motor(2, 0);
    } else {
      seta_motor(3, 3);
    }

  } while (1);

}

/*
 * Para o robo
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void para_robo() {
  seta_motor(0,0);
}

/*
 * Codigo para virar o robo e deixar a parede a esquerda dele
 * Parameter:
 *   void
 * Returns:
 *   void
 */
void vira_robo_dir() {

  unsigned short int sonar_00 = read_sonar(0);
  unsigned short int sonar_15 = read_sonar(15);

  seta_motor(1,4);
  while ((sonar_00 > 650 && sonar_15 > 650) || ((sonar_00 - sonar_15 >= 15) || (sonar_15 - sonar_00 >= 15))) {
    sonar_00 = read_sonar(0);
    sonar_15 = read_sonar(15);
  }

  para_robo();

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
