/**************************************
* Aluno: Luiz Eduardo T. C. Cartolano *
* RA: 183012                          *
* Turma: B                            *
***************************************/
#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// struct que servira como uma "estrutura" para armazenar os rotulos ja definidos
typedef struct RotuloDef {
  char* palavra;
  int posicao;
  int lado; //0 - Esquerda, 1 - Direita
} RotuloDef;
// tabela que contera todos os rotulos que foram definidos
RotuloDef rotulosDefinidos[4096];
int numRotulosDefinidos;

// struct que servira como uma "estrutura" para armazenar os simbolos ja definidos
typedef struct SimboloDef {
  char* palavra;
  int valor;
} SimboloDef;
// tabela que contera todos os simbolos que foram definidos
SimboloDef simbolosDefinidos[4096];
int numSimbolosDefinidos;

// estrutura de memoria do IAS (seria como o PC do IAS)
typedef struct Memoria {
  int posicao;
  int lado; //0 - Esquerda, 1 - Direita
}Memoria;
Memoria memoria;

// declaracao de dois vetores que irao servir de controle para as posicoes de memoria ja escritas
int posicoesMemoriaUsadas[1100];
int esquerdaMemoriaUsadas[1100];

// funcoes que manipulam as tabelas de rotulo e simbolos declarados (e verificam os erros de memoria)
RotuloDef criaNovoRotulo(char* nome, int posicao, int lado);
SimboloDef criaNovoSimbolo(char* palavra, int valor);
int criaTabelaDefinicoes();
char* copiaNomeToken(char* palavra);
int achaPosicaoMemoria(int posicao);

// funcoes que lidam com os erros de definicao
int achaErroDefinicao();
int achaSimbolo(char* palavra);
int achaRotulo(char* palavra);

// funcoes que manipulam as impressoes do mapa de memoria
void printaMapaDeMemoria();
void printaMnemonico (char* palavra);
void printaHexadecimal(int numDeDigitos, long int valorImprimir);
long int converteHexToDec (char* palavra);

/* Retorna:
*  1 caso haja erro na montagem;
*  0 caso não haja erro.
*/
int emitirMapaDeMemoria() {

  // iniciar com -1 os vetores de controle das posicoes de memoria
  for (int i = 0; i < 1100; i++){
    posicoesMemoriaUsadas[i] = -1;
    esquerdaMemoriaUsadas[i] = -1;
  }

  // iniciamos a montagem no lado esquerdo da posicao 0
  memoria.posicao = 0;
  memoria.lado = 0;

  // seta como 0 o valor inicial para o numero de rotulos e simbolos definidos
  numRotulosDefinidos = 0;
  numSimbolosDefinidos = 0;

  // criamos as tabelas de rotulos e simbolos ja definidos, se der erro encerra a montagem
  if (criaTabelaDefinicoes() == 0) {
    return 1;
  }

  // antes de dar proseguimento a montagem verficamos se tem alguma palavra sendo usada sem estar definida
  if (achaErroDefinicao() == 0) {
    return 1;
  }

  // apos criada as tabelas, reiniciamos "PC" para o lado esquerdo da posicao 0
  memoria.posicao = 0;
  memoria.lado = 0;

  // chama a funcao que ira imprimir o mapa
  printaMapaDeMemoria();

  return 0;

}

/*
 * A funcao nao possui entrada e nem retorno, ao ser chamada ela printa o mapa de memoria emitido
 */
void printaMapaDeMemoria() {
  int numDeTokens = getNumberOfTokens();

  for (int i = 0; i < numDeTokens; i++) {

    Token tokenRecuperado = recuperaToken(i);

    if (tokenRecuperado.tipo == Diretiva) {
      if (strcmp(tokenRecuperado.palavra,".org") == 0) {
        if (memoria.lado == 1) {
          printf("00 000\n");
        }
        Token tokenPosicaoDeMemoria = recuperaToken(i+1);
        if (tokenPosicaoDeMemoria.tipo == Hexadecimal) {
          int novaPosicao = (int)converteHexToDec(tokenPosicaoDeMemoria.palavra);
          memoria.posicao = novaPosicao;
          memoria.lado = 0;
        } else {
          int novaPosicao = atoi(tokenPosicaoDeMemoria.palavra);
          memoria.posicao = novaPosicao;
          memoria.lado = 0;
        }
      }
      else if (strcmp(tokenRecuperado.palavra,".align") == 0) {
        Token parametro = recuperaToken(i+1);
        int multiplo = atoi(parametro.palavra);
        if (memoria.lado == 1) {
          printf("00 000\n");
          memoria.lado = 0;
          (memoria.posicao)++;
        } while (memoria.posicao % multiplo != 0) {
          (memoria.posicao)++;
        }
      }
      else if (strcmp(tokenRecuperado.palavra,".wfill") == 0) {
        Token parametro = recuperaToken(i+1);
        int posicoesAOcupar = atoi(parametro.palavra);
        Token valorPreencher = recuperaToken(i+2);

        long int valor;
        if (valorPreencher.tipo == Hexadecimal) {
          valor = converteHexToDec(valorPreencher.palavra);
        } else if (valorPreencher.tipo == Decimal) {
          valor = atoi(valorPreencher.palavra);
        } else {
          int indiceRotulo = achaRotulo(valorPreencher.palavra);
          if(indiceRotulo != -1) {
            valor = rotulosDefinidos[indiceRotulo].posicao;
          } else {
            int indiceSimbolo = achaSimbolo(valorPreencher.palavra);
            valor = simbolosDefinidos[indiceSimbolo].valor;
          }
        }
        for (int i = 0; i < posicoesAOcupar; i++) {
          // printa a posicao de memoria
          printaHexadecimal(3,memoria.posicao);
          // printa o valor q ira ocupar essa posicao de memoria
          printaHexadecimal(10,valor);
          // atualiza a posicao de memoria
          (memoria.posicao)++;
        }
      }
      else if (strcmp(tokenRecuperado.palavra,".word") == 0) {
        Token valorPreencher = recuperaToken(i+1);
        long int valor;
        if (valorPreencher.tipo == Hexadecimal) {
          valor = converteHexToDec(valorPreencher.palavra);
        } else if (valorPreencher.tipo == Decimal) {
          valor = atoi(valorPreencher.palavra);
        } else {
          int indiceRotulo = achaRotulo(valorPreencher.palavra);
          if(indiceRotulo != -1) {
            valor = rotulosDefinidos[indiceRotulo].posicao;
          } else {
            int indiceSimbolo = achaSimbolo(valorPreencher.palavra);
            valor = simbolosDefinidos[indiceSimbolo].valor;
          }
        }
        // printa a posicao de memoria
        printaHexadecimal(3,memoria.posicao);
        printaHexadecimal(10,valor);
        (memoria.posicao)++;
      }
    }
    else if (tokenRecuperado.tipo == Instrucao) {
      Token valorPreencher;
      if (i+1 < numDeTokens) {
        valorPreencher = recuperaToken(i+1);
      }
      long int valorMemoria;
      int ladoMemoria = -1;
      if (valorPreencher.tipo == Hexadecimal) {
        valorMemoria = converteHexToDec(valorPreencher.palavra);
      } else if (valorPreencher.tipo == Decimal) {
        valorMemoria = atoi(valorPreencher.palavra);
      } else {
        int indiceRotulo = achaRotulo(valorPreencher.palavra);
        if(indiceRotulo != -1) {
          valorMemoria = rotulosDefinidos[indiceRotulo].posicao;
          ladoMemoria = rotulosDefinidos[indiceRotulo].lado;
        }
      }

      if (memoria.lado == 1) {
        if ((strcmp(tokenRecuperado.palavra,"JUMP") == 0) || (strcmp(tokenRecuperado.palavra,"JMP+") == 0) || (strcmp(tokenRecuperado.palavra,"STORA") == 0)) {
          if (ladoMemoria == 0) {
            if (strcmp(tokenRecuperado.palavra,"JUMP") == 0) {
              printaMnemonico("JUMP_E");
            } else if (strcmp(tokenRecuperado.palavra,"JMP+") == 0) {
              printaMnemonico("JMP+_E");
            } else {
              printaMnemonico("STORA_E");
            }
          } else if (ladoMemoria == 1) {
            if (strcmp(tokenRecuperado.palavra,"JUMP") == 0) {
              printaMnemonico("JUMP_D");
            } else if (strcmp(tokenRecuperado.palavra,"JMP+") == 0) {
              printaMnemonico("JMP+_D");
            } else {
              printaMnemonico("STORA_D");
            }
          } else {
            // se ladoMemoria for igual a -1, entao a posicao eh um decimal ou um Hexadecimal, que consideramos estar no lado esquerdo
            if (strcmp(tokenRecuperado.palavra,"JUMP") == 0) {
              printaMnemonico("JUMP_E");
            } else if (strcmp(tokenRecuperado.palavra,"JMP+") == 0) {
              printaMnemonico("JMP+_E");
            } else {
              printaMnemonico("STORA_E");
            }
          }
          // por fim printamos o endereco de memoria
          printaHexadecimal(3,valorMemoria);
        } else if ((strcmp(tokenRecuperado.palavra,"RSH") == 0) || (strcmp(tokenRecuperado.palavra,"LSH") == 0) || (strcmp(tokenRecuperado.palavra,"LOADmq") == 0)) {
          printaMnemonico(tokenRecuperado.palavra);
        } else {
          //printaMnemonico
          printaMnemonico(tokenRecuperado.palavra);
          printaHexadecimal(3,valorMemoria);
        }
        (memoria.posicao)++;
        memoria.lado = 0;
      } else {
        // printamos o endereco de memoria
        printaHexadecimal(3,memoria.posicao);

        if ((strcmp(tokenRecuperado.palavra,"JUMP") == 0) || (strcmp(tokenRecuperado.palavra,"JMP+") == 0) || (strcmp(tokenRecuperado.palavra,"STORA") == 0)) {
          if (ladoMemoria == 0) {
            if (strcmp(tokenRecuperado.palavra,"JUMP") == 0) {
              printaMnemonico("JUMP_E");
            } else if (strcmp(tokenRecuperado.palavra,"JMP+") == 0) {
              printaMnemonico("JMP+_E");
            } else {
              printaMnemonico("STORA_E");
            }
          } else if (ladoMemoria == 1) {
            if (strcmp(tokenRecuperado.palavra,"JUMP") == 0) {
              printaMnemonico("JUMP_D");
            } else if (strcmp(tokenRecuperado.palavra,"JMP+") == 0) {
              printaMnemonico("JMP+_D");
            } else {
              printaMnemonico("STORA_D");
            }
          } else {
            // se ladoMemoria for igual a -1, entao a posicao eh um decimal ou um Hexadecimal, que consideramos estar no lado esquerdo
            if (strcmp(tokenRecuperado.palavra,"JUMP") == 0) {
              printaMnemonico("JUMP_E");
            } else if (strcmp(tokenRecuperado.palavra,"JMP+") == 0) {
              printaMnemonico("JMP+_E");
            } else {
              printaMnemonico("STORA_E");
            }
          }
          // por fim printamos o endereco de memoria
          printaHexadecimal(3,valorMemoria);
        } else if ((strcmp(tokenRecuperado.palavra,"RSH") == 0) || (strcmp(tokenRecuperado.palavra,"LSH") == 0) || (strcmp(tokenRecuperado.palavra,"LOADmq") == 0)) {
          printaMnemonico(tokenRecuperado.palavra);
        } else {
          //printaMnemonico
          printaMnemonico(tokenRecuperado.palavra);
          printaHexadecimal(3,valorMemoria);
        }

        memoria.lado = 1;
      }
    }
  }

  if (memoria.lado == 1) {
    printf("00 000\n");
  }
}

/*
 * Argumentos:
 *  numDeDigitos: numero de digitos do decimal a ser impresso
 *  valorImprimir: numero decimal a ser impresso (em Hexadecimal)
 * Retorna:
 *  A funcao nao possui retorno, ela apenas printa o numero solicitado
 */
void printaHexadecimal(int numDeDigitos, long int valorImprimir) {

  if (numDeDigitos == 3) {
    char* buffer = malloc(sizeof(10));
    sprintf(buffer,"%03lX",valorImprimir);
    if (memoria.lado == 1) {
      printf("%c%c%c\n",buffer[0],buffer[1],buffer[2]);
    } else {
      printf("%c%c%c ",buffer[0],buffer[1],buffer[2]);
    }
    free(buffer);
  } else if (numDeDigitos == 10) {
    char* buffer = malloc(sizeof(4));
    sprintf(buffer,"%010lX",valorImprimir);
    if (valorImprimir < 0) {
      printf("%c%c %c%c%c %c%c %c%c%c\n",buffer[6],buffer[7],buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
    } else {
      printf("%c%c %c%c%c %c%c %c%c%c\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9]);
    }
    free(buffer);
  }

}

/*
 * Argumentos:
 *  A funcao nao recebe argumentos, pois acessa vetores diretamente na memoria
 * Retorna:
 *  0 caso haja erro na montagem;
 *  1 caso não haja erro.
 */
int achaErroDefinicao() {
  int numDeTokens = getNumberOfTokens();
  for (int i = 0; i < numDeTokens; i++) {
    Token tokenRecuperado = recuperaToken(i);
    if (tokenRecuperado.tipo == Diretiva) {
      // se o token recuperado for um ".wfill", devemos verificar se o segundo argumento dele foi definido (no caso de ser um Nome)
      if (strcmp(tokenRecuperado.palavra,".wfill") == 0) {
        Token tokenVerificar = recuperaToken(i+2);
        if (tokenVerificar.tipo == Nome) {
          int verificaRotulo = achaRotulo(tokenVerificar.palavra);
          int verificaSimbolo = achaSimbolo(tokenVerificar.palavra);
          if ((verificaRotulo == -1) && (verificaSimbolo == -1)) {
            fprintf(stderr, "USADO MAS NÃO DEFINIDO: %s!\n",tokenVerificar.palavra);
            return 0;
          }
        }
        // se o token recuperado for um ".word", devemos verificar se o primeiro argumento dele foi definido (no caso de ser um Nome)
      } else if (strcmp(tokenRecuperado.palavra,".word") == 0) {
        Token tokenVerificar = recuperaToken(i+1);
        if (tokenVerificar.tipo == Nome) {
          int verificaRotulo = achaRotulo(tokenVerificar.palavra);
          int verificaSimbolo = achaSimbolo(tokenVerificar.palavra);
          if ((verificaRotulo == -1) && (verificaSimbolo == -1)) {
            fprintf(stderr, "USADO MAS NÃO DEFINIDO: %s!\n",tokenVerificar.palavra);
            return 0;
          }
        }
      }
    } else if (tokenRecuperado.tipo == Instrucao) {
      if (!((strcmp(tokenRecuperado.palavra,"LOADmq") == 0) || (strcmp(tokenRecuperado.palavra,"LSH") == 0) || (strcmp(tokenRecuperado.palavra,"RSH") == 0))) {
        Token tokenVerificar = recuperaToken(i+1);
        if (tokenVerificar.tipo == Nome) {
          int verificaRotulo = achaRotulo(tokenVerificar.palavra);
          int verificaSimbolo = achaSimbolo(tokenVerificar.palavra);
          if ((verificaRotulo == -1) && (verificaSimbolo == -1)) {
            fprintf(stderr, "USADO MAS NÃO DEFINIDO: %s!\n",tokenVerificar.palavra);
            return 0;
          }
        }
      }
    }
  }

  return 1;

}

/*
 * Argumentos:
 *  palavra: rotulo que devemos verificar se ja existe
 * Retorna:
 *  i - a posicao do rotulo no vetor de rotulos definidos
 *  -1 -  caso o rotulo nao exista
 */
int achaRotulo(char* palavra) {
  // printf("numRotulosDefinidos: %d\n",numRotulosDefinidos);
  for (int i = 0; i < numRotulosDefinidos; i++) {
    if (strcmp(palavra,rotulosDefinidos[i].palavra) == 0) {
      // printf("%s\n",palavra);
      return i;
    }
  }
  return -1;
}

/*
 * Argumentos:
 *  palavra: simbolo que devemos verificar se ja existe
 * Retorna:
 *  i - a posicao do simbolo no vetor de simbolos definidos
 *  -1 -  caso o simbolo nao exista
 */
int achaSimbolo(char* palavra) {
  for (int i = 0; i < numSimbolosDefinidos; i++) {
    if (strcmp(palavra,simbolosDefinidos[i].palavra) == 0) {
      return i;
    }
  }
  return -1;
}

/*
 * Retorna:
 *  1 - se der tudo certo na criacao dos rotulos e simbolos (e tambem se nao ha tentativa de escrita em lugar errado)
 *  0 - caso haja uma palavra que deva ser escrita em lugar errado
 */
int criaTabelaDefinicoes() {
  int numDeTokens = getNumberOfTokens();

  for (int i = 0; i < numDeTokens; i++) {

    Token tokenRecuperado = recuperaToken(i);

    // se o token for a definicao de um rotulo, entao o adicionamos na tabela de rotulos que estamos criando
    if (tokenRecuperado.tipo == DefRotulo) {
      char* palavra = copiaNomeToken(tokenRecuperado.palavra);
      int rotuloExiste = achaRotulo(palavra);
      if (rotuloExiste != -1){
        fprintf(stderr, "Rotulo ja definido!\n");
        return 0;
      }
      RotuloDef novoRotulo = criaNovoRotulo(palavra, memoria.posicao, memoria.lado);
      rotulosDefinidos[numRotulosDefinidos] = novoRotulo;
      numRotulosDefinidos++;
    }
    else if (tokenRecuperado.tipo == Diretiva) {
      // se o token for um .set entao ele esta definindo um simbolo, logo devemos adicionar esse simbolo a nossa tabela
      if (strcmp(tokenRecuperado.palavra,".set") == 0) {
        Token simboloDefinido = recuperaToken(i+1);
        if (achaSimbolo(simboloDefinido.palavra) != -1){
          fprintf(stderr, "Simbolo ja definido!\n");
          return 0;
        }
        Token valorDoSimbolo = recuperaToken(i+2);
        if (valorDoSimbolo.tipo == Hexadecimal) {
          int valor = (int)converteHexToDec(valorDoSimbolo.palavra);
          SimboloDef novoSimbolo = criaNovoSimbolo(simboloDefinido.palavra,valor);
          simbolosDefinidos[numSimbolosDefinidos] = novoSimbolo;
          numSimbolosDefinidos++;
        } else {
          int valor = atoi(valorDoSimbolo.palavra);
          SimboloDef novoSimbolo = criaNovoSimbolo(simboloDefinido.palavra,valor);
          simbolosDefinidos[numSimbolosDefinidos] = novoSimbolo;
          numSimbolosDefinidos++;
        }
      }
      else if (strcmp(tokenRecuperado.palavra,".org") == 0) {
        /* Pensar em como verificar se essa posicao ja foi usada */
        Token tokenPosicaoDeMemoria = recuperaToken(i+1);
        if (tokenPosicaoDeMemoria.tipo == Hexadecimal) {
          int novaPosicao = (int)converteHexToDec(tokenPosicaoDeMemoria.palavra);
          memoria.posicao = novaPosicao;
          memoria.lado = 0;
        } else {
          int novaPosicao = atoi(tokenPosicaoDeMemoria.palavra);
          memoria.posicao = novaPosicao;
          memoria.lado = 0;
        }
      }
      else if (strcmp(tokenRecuperado.palavra,".align") == 0) {
        Token parametro = recuperaToken(i+1);
        int multiplo = atoi(parametro.palavra);
        if (memoria.lado == 1) {
          memoria.lado = 0;
          (memoria.posicao)++;
          posicoesMemoriaUsadas[memoria.posicao] = 1;
        } while (memoria.posicao % multiplo != 0) {
          posicoesMemoriaUsadas[memoria.posicao] = 1;
          (memoria.posicao)++;
        }
      }
      else if (strcmp(tokenRecuperado.palavra,".wfill") == 0) {
        Token parametro = recuperaToken(i+1);
        int posicoesAOcupar = atoi(parametro.palavra);
        // se a posicao de memoria ocupada ao fim do .wfill for maior que 1024, temos um erro
        if (memoria.posicao + posicoesAOcupar > 1024) {
          fprintf(stderr, "Impossível montar o código!\n");
          return 0;
        }
        // se o .wfill foi chamado para o lado direito de uma palavra temos um erro
        if (memoria.lado == 1) {
          fprintf(stderr, "Impossível montar o código!\n");
          return 0;
        }

        int inicio = memoria.posicao;
        int fim = memoria.posicao + posicoesAOcupar;
        for (int i = inicio; i < fim; i++) {
          if (achaPosicaoMemoria(memoria.posicao) == 0) {
             fprintf(stderr, "Impossível montar o código!\n");
             return 0;
          }
          posicoesMemoriaUsadas[memoria.posicao] = 1;
          esquerdaMemoriaUsadas[memoria.posicao] = 1;
          (memoria.posicao)++;
        }

      }
      else if (strcmp(tokenRecuperado.palavra,".word") == 0) {
        // um .word nao pode ser chamado para o lado direito de uma palavra de memoria
        if (memoria.lado == 1) {
          fprintf(stderr, "Impossível montar o código!\n");
          return 0;
        }
        if (memoria.posicao > 1024) {
          fprintf(stderr, "Impossível montar o código!\n");
          return 0;
        }
        if (achaPosicaoMemoria(memoria.posicao) == 0) {
          fprintf(stderr, "Impossível montar o código!\n");
          return 0;
        }
        posicoesMemoriaUsadas[memoria.posicao] = 1;
        esquerdaMemoriaUsadas[memoria.posicao] = 1;
        (memoria.posicao)++;
      }
    }
    else if (tokenRecuperado.tipo == Instrucao) {
      if (memoria.posicao <= 1024) {
        if (memoria.lado == 1) {
          posicoesMemoriaUsadas[memoria.posicao] = 1;
          (memoria.posicao)++;
          memoria.lado = 0;
        } else {
          if (achaPosicaoMemoria(memoria.posicao) == 0) {
            fprintf(stderr, "Impossível montar o código!\n");
            return 0;
          }
          if (esquerdaMemoriaUsadas[memoria.posicao] == -1){
            memoria.lado = 1;
            esquerdaMemoriaUsadas[memoria.posicao] = 1;
          } else {
            fprintf(stderr, "Impossível montar o código!\n");
            return 0;
          }
        }
      } else {
        fprintf(stderr, "Impossível montar o código!\n");
        return 0;
      }
    }
  }

  return 1;

}

/*
 * Argumentos:
 *  posicao: posicao de memoria que quero verificar
 * Retorna:
 *  1 - se a posicao ainda nao foi usada
 *  0 - se a posicao ja foi usada
 */
int achaPosicaoMemoria(int posicao) {
  if (posicoesMemoriaUsadas[posicao] == -1)
    return 1;
  else
    return 0;
}

/*
 * Argumentos:
 *  nome: nome do rotulo que sera inserido na lista de rotulos
 *  posicao: posicao que o rotulo ocupa no mapa de memoria
 *  lado: lado da palavra de memoria que o rotulo ocupa
 * Retorna:
 *  novoRotulo - rotulo que sera adicionado a lista de rotulos
 */
RotuloDef criaNovoRotulo(char* nome, int posicao, int lado) {
  RotuloDef novoRotulo;
  novoRotulo.palavra = nome;
  novoRotulo.posicao = posicao;
  novoRotulo.lado = lado;

  return novoRotulo;
}

/*
 * Argumentos:
 *  palavra: nome do simbolo que sera inserido na lista de simbolos
 *  valor: valor associado ao simbolo que esta sendo definido
 * Retorna:
 *  novoSimbolo - simbolo que sera adicionado a lista de simbolos
 */
SimboloDef criaNovoSimbolo(char* palavra, int valor) {
  SimboloDef novoSimbolo;
  novoSimbolo.palavra = palavra;
  novoSimbolo.valor = valor;

  return novoSimbolo;
}

/*
 * Argumentos:
 *  palavra: rotulo com ":" ao final (que sera tratado)
 * Retorna:
 *  copia - rotulo sem os ":" ao final, que sera salvo na lista de rotulos definidos
 */
char* copiaNomeToken(char* palavra) {
  int tamanho = strlen(palavra);
  char* copia = malloc(sizeof(tamanho));
  int j = 0;

  for (int i = 0; i < tamanho; i++) {
    if (palavra[i] == ':') {
      copia[i] = '\0';
    } else {
      copia[j] = palavra[i];
      j++;
    }
  }
  return copia;

}

/*
 * Argumentos:
 *  palavra: string com o Hexadecimal que sera convertido em um decimal
 * Retorna:
 *  decimal - valor do Hexadecimal na representacao decimal
 */
long int converteHexToDec (char* palavra) {

  long int decimal = strtol(palavra, NULL, 0);
  return decimal;

}

/*
 * Argumentos:
 *  palavra: instrucao que deve ser impressa
 * Retorna:
 *  A funcao nao possui retorno, ja que ela apenas printa o codigo da instrucao solicitado no argumento
 */
void printaMnemonico (char* palavra) {
  /* verifica se eh um LOAD_M */
  if (strcmp(palavra,"LOAD") == 0) {
    printf("01 ");
    /* verifica se eh um LOAD_MENOS_M */
  } else if (strcmp(palavra,"LOAD-") == 0) {
    printf("02 ");
    /* verifica se eh um LOAD_MODULO */
  } else if (strcmp(palavra,"LOAD|") == 0) {
    printf("03 ");
    /* verifica se eh um LOAD_MQ */
  } else if (strcmp(palavra,"LOADmq") == 0) {
    if (memoria.lado == 1) {
      printf("0A 000\n");
    } else {
      printf("0A 000 ");
    }
  } else if (strcmp(palavra,"LOADmq_mx") == 0) {
    printf("09 ");
    /* verifica se eh um STOR */
  } else if (strcmp(palavra,"STOR") == 0) {
    printf("21 ");
    /* verifica se eh um JUMP */
  } else if (strcmp(palavra,"JUMP_E") == 0) {
    printf("0D ");
    /* verifica se eh um JUMP_COND */
  } else if (strcmp(palavra,"JUMP_D") == 0) {
    printf("0E ");
    /* verifica se eh um JUMP_COND */
  } else if (strcmp(palavra,"JMP+_E") == 0) {
    printf("0F ");
  }else if (strcmp(palavra,"JMP+_D") == 0) {
    printf("10 ");
    /* verifica se eh um ADD */
  } else if (strcmp(palavra,"ADD") == 0) {
    printf("05 ");
    /* verifica se eh um ADD_MODULO */
  } else if (strcmp(palavra,"ADD|") == 0) {
    printf("07 ");
    /* verifica se eh um SUB */
  } else if (strcmp(palavra,"SUB") == 0) {
    printf("06 ");
    /* verifica se eh um SUB_MODULO */
  } else if (strcmp(palavra,"SUB|") == 0) {
    printf("08 ");
    /* verifica se eh um MUL */
  } else if (strcmp(palavra,"MUL") == 0) {
    printf("0B ");
    /* verifica se eh um DIV */
  } else if (strcmp(palavra,"DIV") == 0) {
    printf("0C ");
    /* verifica se eh um LSH */
  } else if (strcmp(palavra,"LSH") == 0) {
    if (memoria.lado == 1) {
      printf("14 000\n");
    } else {
      printf("14 000 ");
    }
    /* verifica se eh um RSh */
  } else if (strcmp(palavra,"RSH") == 0) {
    if (memoria.lado == 1) {
      printf("15 000\n");
    } else {
      printf("15 000 ");
    }
    /* verifica se eh um STOR_M */
  } else if (strcmp(palavra,"STORA_E") == 0) {
    printf("12 ");

  } else if (strcmp(palavra,"STORA_D") == 0) {
    printf("13 ");
    /* instrucao eh invalida */
  } else {
    printf("Deu ruim ! ¯_(ツ)_/¯");
  }
}
