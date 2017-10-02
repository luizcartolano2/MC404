#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// tabela que contera todos os rotulos que foram definidos
typedef struct RotuloDef {
  char* palavra;
  int posicao;
  int lado; //0 - Esquerda, 1 - Direita
} RotuloDef;
RotuloDef rotulosDefinidos[4096];
int numRotulosDefinidos;

// tabela que contera todos os simbolos que foram definidos
typedef struct SimboloDef {
  char* palavra;
  int valor;
} SimboloDef;
SimboloDef simbolosDefinidos[4096];
int numSimbolosDefinidos;

// estrutura de memoria do IAS
typedef struct Memoria {
  int posicao;
  int lado; //0 - Esquerda, 1 - Direita
}Memoria;
Memoria memoria;
int posicoesMemoriaUsadas[1025];
int numPosicoesMemoriaUsadas;
int achaPosicaoMemoria(int posicao);

long int converteHexToDec (char* palavra);
char* forneceMnemonico (char* palavra);

// funcoes que manipulam as tabelas de rotulo e simbolos declarados
RotuloDef criaNovoRotulo(char* nome, int posicao, int lado);
SimboloDef criaNovoSimbolo(char* palavra, int valor);
int criaTabelaDefinicoes();
char* copiaNomeToken(char* palavra);
void imprimeListaRotulos();
void imprimeListaSimbolos();

// funcoes que lidam com os erros de definicao
int achaErroDefinicao();
int achaSimbolo(char* palavra);
int achaRotulo(char* palavra);
/* Retorna:
*  1 caso haja erro na montagem;
*  0 caso não haja erro.
*/
int emitirMapaDeMemoria() {
  // iniciamos a montagem no lado esquerdo da posicao 0
  memoria.posicao = 0;
  memoria.lado = 0;
  numPosicoesMemoriaUsadas = 0;

  // seta como 0 o valor inicial para o numero de rotulos e simbolos definidos
  numRotulosDefinidos = 0;
  numSimbolosDefinidos = 0;

  // criamos as tabelas de rotulos e simbolos ja definidos (e imprimimos apenas para verificacao)
  if (criaTabelaDefinicoes() == 0) {
    return 1;
  }
  imprimeListaSimbolos();
  imprimeListaRotulos();
  // antes de dar proseguimento a montagem verficamos se tem alguma palavra sendo usada sem estar definida
  if (achaErroDefinicao() == 0) {
    return 1;
  }

  // apos criada as tabelas, reiniciamos a montagem no lado esquerdo da posicao 0
  memoria.posicao = 0;
  memoria.lado = 0;

  return 0;

}

// funcao que cria as tabelas de simbolos e rotulos definidos
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
          if ((verificaRotulo == 0) && (verificaSimbolo == 0)) {
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
          if ((verificaRotulo == 0) && (verificaSimbolo == 0)) {
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
          if ((verificaRotulo == 0) && (verificaSimbolo == 0)) {
            fprintf(stderr, "USADO MAS NÃO DEFINIDO: %s!\n",tokenVerificar.palavra);
            return 0;
          }
        }
      }
    }
  }

  return 1;

}

int achaRotulo(char* palavra) {
  for (int i = 0; i < numRotulosDefinidos; i++) {
    if (strcmp(palavra,rotulosDefinidos[i].palavra) == 0) {
      return 1;
    }
  }
  return 0;
}

int achaSimbolo(char* palavra) {
  for (int i = 0; i < numSimbolosDefinidos; i++) {
    if (strcmp(palavra,simbolosDefinidos[i].palavra) == 0) {
      return 1;
    }
  }
  return 0;
}

int criaTabelaDefinicoes() {
  int numDeTokens = getNumberOfTokens();

  for (int i = 0; i < numDeTokens; i++) {

    Token tokenRecuperado = recuperaToken(i);

    // se o token for a definicao de um rotulo, entao o adicionamos na tabela de rotulos que estamos criando
    if (tokenRecuperado.tipo == DefRotulo) {
      char* palavra = copiaNomeToken(tokenRecuperado.palavra);
      RotuloDef novoRotulo = criaNovoRotulo(palavra, memoria.posicao, memoria.lado);
      rotulosDefinidos[numRotulosDefinidos] = novoRotulo;
      numRotulosDefinidos++;
    }
    else if (tokenRecuperado.tipo == Diretiva) {
      // se o token for um .set entao ele esta definindo um simbolo, logo devemos adicionar esse simbolo a nossa tabela
      if (strcmp(tokenRecuperado.palavra,".set") == 0) {
        Token simboloDefinido = recuperaToken(i+1);
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
        if (achaPosicaoMemoria(memoria.posicao) == 0) {
          fprintf(stderr, "Impossível montar o código!\n");
          return 0;
        } else {
          posicoesMemoriaUsadas[numPosicoesMemoriaUsadas] = memoria.posicao;
          numPosicoesMemoriaUsadas++;
        }
      }
      else if (strcmp(tokenRecuperado.palavra,".align") == 0) {
        Token parametro = recuperaToken(i+1);
        int multiplo = atoi(parametro.palavra);
        if (memoria.lado == 1) {
          memoria.lado = 0;
          (memoria.posicao)++;
          posicoesMemoriaUsadas[numPosicoesMemoriaUsadas] = memoria.posicao;
          numPosicoesMemoriaUsadas++;
        } while (memoria.posicao % multiplo != 0) {
          (memoria.posicao)++;
          posicoesMemoriaUsadas[numPosicoesMemoriaUsadas] = memoria.posicao;
          numPosicoesMemoriaUsadas++;
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
        for (int i = memoria.posicao; i < memoria.posicao + posicoesAOcupar; i++) {
          posicoesMemoriaUsadas[numPosicoesMemoriaUsadas] = i;
          numPosicoesMemoriaUsadas++;
        }
        memoria.posicao = memoria.posicao + posicoesAOcupar;

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
        memoria.posicao = memoria.posicao + 1;
        posicoesMemoriaUsadas[numPosicoesMemoriaUsadas] = memoria.posicao;
        numPosicoesMemoriaUsadas++;
      }
    }
    else if (tokenRecuperado.tipo == Instrucao) {
      if (memoria.posicao <= 1024) {
        if (memoria.lado == 1) {
          (memoria.posicao)++;
          memoria.lado = 0;
          posicoesMemoriaUsadas[numPosicoesMemoriaUsadas] = memoria.posicao;
          numPosicoesMemoriaUsadas++;
        } else {
          memoria.lado = 1;
        }
      } else {
        fprintf(stderr, "Impossível montar o código!\n");
        return 0;
      }
    }
  }

  return 1;

}

int achaPosicaoMemoria(int posicao) {
  for (int i = 0; i < numPosicoesMemoriaUsadas; i++) {
    if (posicoesMemoriaUsadas[i] == posicao) {
      return 0;
    }
  }
  return 1;
}

// funcao generica que cria um novo RotuloDefinido
RotuloDef criaNovoRotulo(char* nome, int posicao, int lado) {
  RotuloDef novoRotulo;
  novoRotulo.palavra = nome;
  novoRotulo.posicao = posicao;
  novoRotulo.lado = lado;

  return novoRotulo;
}

// funcao generica que cria um novo SimboloDefinido
SimboloDef criaNovoSimbolo(char* palavra, int valor) {
  SimboloDef novoSimbolo;
  novoSimbolo.palavra = palavra;
  novoSimbolo.valor = valor;

  return novoSimbolo;
}

// funcao que copia o nome do token definido sem os ":", ja que essa sera a maneira que ele sera usado
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

// funcao que converte Hexadecimal para decimal
long int converteHexToDec (char* palavra) {

  long int decimal = strtol(palavra, NULL, 0);
  return decimal;

}

// funcao que fornece o Hexadecimal correspondente a cada instrucao
char* forneceMnemonico (char* palavra) {
  /* verifica se eh um LOAD_M */
  if (strcmp(palavra,"LOAD") == 0) {
    return "01";
    /* verifica se eh um LOAD_MENOS_M */
  } else if (strcmp(palavra,"LOAD-") == 0) {
    return "02";
    /* verifica se eh um LOAD_MODULO */
  } else if (strcmp(palavra,"LOAD|") == 0) {
    return "03";
    /* verifica se eh um LOAD_MQ */
  } else if (strcmp(palavra,"LOADmq") == 0) {
    return "0A";
    /* verifica se eh um LOAD_MQ_MX */
  } else if (strcmp(palavra,"LOADmq_mx") == 0) {
    return "09";
    /* verifica se eh um STOR */
  } else if (strcmp(palavra,"STOR") == 0) {
    return "21";
    /* verifica se eh um JUMP */
  } else if (strcmp(palavra,"JUMP_E") == 0) {
    return "0D";
    /* verifica se eh um JUMP_COND */
  } else if (strcmp(palavra,"JUMP_D") == 0) {
    return "0E";
    /* verifica se eh um JUMP_COND */
  } else if (strcmp(palavra,"JMP+_E") == 0) {
    return "0F";

  }else if (strcmp(palavra,"JMP+_D") == 0) {
    return "10";
    /* verifica se eh um ADD */
  } else if (strcmp(palavra,"ADD") == 0) {
    return "05";
    /* verifica se eh um ADD_MODULO */
  } else if (strcmp(palavra,"ADD|") == 0) {
    return "07";
    /* verifica se eh um SUB */
  } else if (strcmp(palavra,"SUB") == 0) {
    return "06";
    /* verifica se eh um SUB_MODULO */
  } else if (strcmp(palavra,"SUB|") == 0) {
    return "08";
    /* verifica se eh um MUL */
  } else if (strcmp(palavra,"MUL") == 0) {
    return "0B";
    /* verifica se eh um DIV */
  } else if (strcmp(palavra,"DIV") == 0) {
    return "0C";
    /* verifica se eh um LSH */
  } else if (strcmp(palavra,"LSH") == 0) {
    return "14";
    /* verifica se eh um RSh */
  } else if (strcmp(palavra,"RSH") == 0) {
    return "15";
    /* verifica se eh um STOR_M */
  } else if (strcmp(palavra,"STORA_E") == 0) {
    return "12";

  } else if (strcmp(palavra,"STORA_D") == 0) {
    return "13";
    /* instrucao eh invalida */
  } else {
    return "ERRROOO";
  }
}

/*Funcoes auxiliares para verificar as tabelas de rotulo e simbolos criadas*/
void imprimeListaRotulos() {
  printf("\n");
  printf("Lista de Rotulos\n");
  for (int i = 0; i < numRotulosDefinidos; i++) {
    printf("RotuloDef %d\n", i);
    printf("\tPalavra: %s\n", rotulosDefinidos[i].palavra);
    printf("\tPosicao: %d\n", rotulosDefinidos[i].posicao);
    printf("\tLado: %d\n", rotulosDefinidos[i].lado);
  }
  printf("\n");
}

void imprimeListaSimbolos() {
  printf("\n");
  printf("Lista de Simbolos\n");
  for (int i = 0; i < numSimbolosDefinidos; i++) {
    printf("Simbolo %d\n", i);
    printf("\tPalavra: %s\n", simbolosDefinidos[i].palavra);
    printf("\tValor: %d\n", simbolosDefinidos[i].valor);
  }
  printf("\n");
}
