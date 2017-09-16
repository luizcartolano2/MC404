#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declaracao das diretivas */
#define SET ".set"
#define ORG ".org"
#define ALIGN ".align"
#define WFILL ".wfill"
#define WORD ".word"

/* Declaracao das instrucoes*/
#define LOAD_M "LOAD"
#define LOAD_MENOS_M "LOAD-"
#define LOAD_MODULO "LOAD|"
#define LOAD_MQ "LOADmq"
#define LOAD_MQ_MX "LOADmq_mx"
#define STOR "STOR"
#define JUMP "JUMP"
#define JUMP_COND "JMP+"
#define ADD "ADD"
#define ADD_MODULO "ADD|"
#define SUB "SUB"
#define SUB_MODULO "SUB|"
#define MUL "MUL"
#define DIV "DIV"
#define LSH "LSH"
#define RSH "RSH"
#define STOR_M "STORA"

/* Declaracao do escopo de funcoes q vao ser utilizadas para verificar a veracidade das palavras encontradas */
int verificaDiretiva (char *palavra);
int verificaInstrucao (char *palavra);
int verificaRotuloDefinicao (char *palavra);
int verificaRotuloUso (char* palavra);
int verificaSimbolo (char *palavra);
int verificaHexadecimal (char* palavra);
int verificaDecimal (char* palavra);

/* Funcoes que lidam com os erros */
void erroLexico (int linha);
void erroGramatical (int linha);
void achaErroLexico();
void achaErroGramatical();

/* Funcao que manipula as palavras lidas */
void manipulaPalavra (char* palavra);
char* removeAspas(char* palavra);

/* Funcao que cria os tokens */
void criaTokenDiretiva(char* palavra);
void criaTokenRotulo(char* palavra);
void criaTokenInstrucao(char* palavra);
void criaTokenHexadecimal(char* palavra);
void criaTokenDecimal(char* palavra);
void criaTokenNome(char* palavra);
Token criaToken(TipoDoToken tipo, char* palavra, unsigned linha);

/* variaveis globais */
int num_barraN;
unsigned numLinha;


/*
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem;
 *  0 caso não haja erro.
 */
int processarEntrada(char* entrada, unsigned tamanho) {

  char* linha;
  char* palavra;
  char* saveptr2;
  numLinha = 0;

  linha = strsep(&entrada,"\n");
  numLinha++;
  while (linha != NULL) {
    palavra = strtok_r(linha," \t",&saveptr2);
    while (palavra != NULL) {
      if (strcmp(palavra,"#") == 0) {
        palavra = NULL;
      } else {
          manipulaPalavra(palavra);
          palavra = strtok_r(NULL," \t",&saveptr2);
        }
      }
      linha = strsep(&entrada,"\n");
      numLinha++;
    }

  achaErroGramatical();
  achaErroLexico();
  imprimeListaTokens();

  return 0;

}

/* Funcao que percorre o vetor de Tokens encontrando erros lexicos */
void achaErroLexico() {
  int numDeTokens = getNumberOfTokens();
  int verifica;

  if (numDeTokens == 0) {
    // se nao tem nenhum token nao tem erro lexico
    return;
  } else {
    for (int i = 0; i < numDeTokens; i++) {
      Token token = recuperaToken(i);
      /* Se for uma Diretiva */
      if (token.tipo == Diretiva) {
        verifica = verificaDiretiva(token.palavra);
        if (verifica == 0) {
          erroLexico(token.linha);
        }
        /* Se for um rotulo */
      } else if (token.tipo == DefRotulo) {
        verifica = verificaRotuloDefinicao(token.palavra);
        if (verifica == 0) {
          erroLexico(token.linha);
        }
        /* Se for um Hexadecimal */
      } else if(token.tipo == Hexadecimal) {
        verifica = verificaHexadecimal(token.palavra);
        if (verifica == 0) {
          erroLexico(token.linha);
        }
        /* Se for um Decimal */
      } else if (token.tipo == Decimal) {
        verifica = verificaDecimal(token.palavra);
        if (verifica == 0) {
          erroLexico(token.linha);
        }
      }
    }
  }
}

/* Funcao que percorre o vetor de Tokens encontrando erros gramaticais */
void achaErroGramatical() {
  Token primeiroToken = recuperaToken(0);
  int linhaAtual = primeiroToken.linha;
  int linhaAnterior = linhaAtual - 1;
  int numDeTokens = getNumberOfTokens();
  if (numDeTokens == 4096) {
    numDeTokens--;
  }

  for (int i = 0; i < numDeTokens; i++) {
    Token tokenRecuperado = recuperaToken(i);
    linhaAtual = tokenRecuperado.linha;
    /* Aqui verficamos se a primeira palavra de cada linha é de fato, uma instrucao, rotulo ou diretiva */
    if (linhaAtual != linhaAnterior) {
      if ((tokenRecuperado.tipo == Nome) || (tokenRecuperado.tipo == Decimal) || (tokenRecuperado.tipo == Hexadecimal)) {
        erroGramatical(tokenRecuperado.linha);
      } else{
        linhaAnterior = linhaAtual;
      }
    }

    if (tokenRecuperado.tipo == Diretiva) {
      if (i == numDeTokens + 1) {
        /* Se uma diretiva estiver na ultima posicao do vetor de tokens ela nao sera seguida por nada, o que eh um erro gramatical*/
        erroGramatical(tokenRecuperado.linha);
      } else if (strcmp((tokenRecuperado.palavra),SET) == 0) {
        Token proximoToken = recuperaToken(i+1);
        Token proxProxToken = recuperaToken(i+2);
        /* a primeira palavra que segue um .set DEVE ser um nome */
        if (!(proximoToken.tipo == Nome)) {
          // printf("ERRO NO .SET\n");
          erroGramatical(tokenRecuperado.linha);
        }
        /* a segunda palavra que segue um .set DEVE ser um Hexadecimal ou um Decimal */
        if (!((proxProxToken.tipo == Hexadecimal) || (proxProxToken.tipo == Decimal))) {
          // printf("ERRO NO .SET\n");
          erroGramatical(tokenRecuperado.linha);
        }
      } else if (strcmp((tokenRecuperado.palavra),ORG) == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (((proximoToken.tipo != Hexadecimal) && (proximoToken.tipo != Decimal))) {
          // printf("ERRO NO .ORG\n");
          erroGramatical(tokenRecuperado.linha);
        }
      } else if (strcmp((tokenRecuperado.palavra),ALIGN) == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (!(proximoToken.tipo == Decimal)) {
          // printf("ERRO NO .ALIGN\n");
          // printf("%s\n",tokenRecuperado.palavra);
          erroGramatical(tokenRecuperado.linha);
        }
      } else if (strcmp((tokenRecuperado.palavra),WFILL) == 0) {
        Token proximoToken = recuperaToken(i+1);
        Token proxProxToken = recuperaToken(i+2);
        /* a primeira palavra que segue um .wfill DEVE ser um Decimal */
        if (!(proximoToken.tipo == Decimal)) {
          // printf("ERRO NO .WFILL\n");
          erroGramatical(tokenRecuperado.linha);
        }
        /* a segunda palavra que segue um .wfill DEVE ser um Hexadecimal ou um Decimal ou um Nome ou um Rotulo */
        if (!((proxProxToken.tipo == Hexadecimal) || (proxProxToken.tipo == Decimal) || (proxProxToken.tipo == DefRotulo) || (proxProxToken.tipo == Nome))) {
          // printf("ERRO NO .WFILL\n");
          erroGramatical(tokenRecuperado.linha);
        }

      } else if (strcmp((tokenRecuperado.palavra),WORD) == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (!((proximoToken.tipo == Hexadecimal) || (proximoToken.tipo == Decimal) || (proximoToken.tipo == DefRotulo) || (proximoToken.tipo == Nome))) {
          // printf("ERRO NO .WORD\n");
          erroGramatical(tokenRecuperado.linha);
        }
      }
    }
    else if (tokenRecuperado.tipo == Instrucao) {
      if (!((strcmp(tokenRecuperado.palavra,LOAD_MQ) == 0) || (strcmp(tokenRecuperado.palavra,LSH) == 0) || (strcmp(tokenRecuperado.palavra,RSH) == 0) || (strcmp(tokenRecuperado.palavra,STOR_M) == 0))) {
        if (i == numDeTokens + 1) {
          /* com excessao das instrucoes LOAD_MQ, LSH, RSH e STOR_M, todas as demais devem ser seguidas de alguma coisa (nome, decimal ou hexadecimal),
          logo ocupar a ultima posicao do vetor de tokes eh um erro gramatical */
          erroGramatical(tokenRecuperado.linha);
        }
        Token proximoToken = recuperaToken(i+1);
        if (!((proximoToken.tipo == Hexadecimal) || (proximoToken.tipo == Decimal) || (proximoToken.tipo == Nome))) {
          erroGramatical(tokenRecuperado.linha);
        }
      }
    }
    else if (tokenRecuperado.tipo == DefRotulo) {
      /* um rotulo sempre deve iniciar uma linha em linguagem de montagem */
      if (i > 0) {
        Token tokenAnterior = recuperaToken(i-1);
        if (tokenAnterior.linha == tokenRecuperado.linha) {
          erroGramatical(tokenRecuperado.linha);
        }
      }
    }


  }

}

/* Funcao que manipula as palavras que sao lidas */
void manipulaPalavra (char* palavra) {
  int tamanho = strlen(palavra);

  /* Verifica se eh uma diretiva*/
  if (palavra[0] == '.') {
    // printf("CRIANDO DIRTIVA\n");
    criaTokenDiretiva(palavra);
  } else if (palavra[tamanho-1] == ':') {
    // printf("CRIANDO ROTULO\n");
    criaTokenRotulo(palavra);
  } else if (verificaInstrucao(palavra)) {
    // printf("CRIANDO INSTRUCAO\n");
    criaTokenInstrucao(palavra);
  } else if (palavra[0] == '\"') {
    char* semAspas = removeAspas(palavra);
    if (semAspas[0] == '0' && ((semAspas[1] == 'x') || (semAspas[1] == 'X'))) {
      // printf("CRIANDO HEXADECIMAL\n");
      criaTokenHexadecimal(semAspas);
    } else if (verificaDecimal(semAspas)) {
      // printf("CRIANDO DECIMAL\n");
      criaTokenDecimal(semAspas);
    } else {
      // printf("CRIANDO NOME\n");
      criaTokenNome(semAspas);
    }
  } else if (palavra[0] == '0' && ((palavra[1] == 'x') || (palavra[1] == 'X'))) {
    // printf("CRIANDO HEXADECIMAL\n");
    criaTokenHexadecimal(palavra);
  } else if (verificaDecimal(palavra)) {
    // printf("CRIANDO DECIMAL\n");
    criaTokenDecimal(palavra);
  } else {
    // printf("CRIANDO UM NOME\n");
    criaTokenNome(palavra);
  }
}

/* Funcao que verifica a validade da diretiva */
int verificaDiretiva (char* palavra) {
  if (strcmp(palavra,SET) == 0) {
    return 1;
  } else if (strcmp(palavra,ORG) == 0) {
    return 2;
  } else if (strcmp(palavra,ALIGN) == 0) {
    return 3;
  } else if (strcmp(palavra,WFILL) == 0) {
    return 4;
  } else if (strcmp(palavra,WORD) == 0) {
    return 5;
  } else {
    // a diretiva esta incorreta
    return 0;
  }
}

/* Funcao que verifica a validade da instrucao */
int verificaInstrucao (char* palavra) {
  if (strcmp(palavra,LOAD_M) == 0) {
    return 1;
  } else if (strcmp(palavra,LOAD_MENOS_M) == 0) {
    return 1;
  } else if (strcmp(palavra,LOAD_MODULO) == 0) {
    return 2;
  } else if (strcmp(palavra,LOAD_MQ) == 0) {
    return 3;
  } else if (strcmp(palavra,LOAD_MQ_MX) == 0) {
    return 4;
  } else if (strcmp(palavra,STOR) == 0) {
    return 5;
  } else if (strcmp(palavra,JUMP) == 0) {
    return 6;
  } else if (strcmp(palavra,JUMP_COND) == 0) {
    return 7;
  } else if (strcmp(palavra,ADD) == 0) {
    return 8;
  } else if (strcmp(palavra,ADD_MODULO) == 0) {
    return 9;
  } else if (strcmp(palavra,SUB) == 0) {
    return 10;
  } else if (strcmp(palavra,SUB_MODULO) == 0) {
    return 11;
  } else if (strcmp(palavra,MUL) == 0) {
    return 12;
  } else if (strcmp(palavra,DIV) == 0) {
    return 13;
  } else if (strcmp(palavra,LSH) == 0) {
    return 14;
  } else if (strcmp(palavra,RSH) == 0) {
    return 15;
  } else if (strcmp(palavra,STOR_M) == 0) {
    return 16;
  } else {
    // instrucao eh invalida
    return 0;
  }

}

/* Funcao que verifica a validade do rotulo na sua definicao */
int verificaRotuloDefinicao (char* palavra) {
  int tamanho = strlen(palavra); // nao tem o '\0'

  if (palavra[0] >= '0' && palavra[0] <= '9') {
    // rotulo eh invalido
    return 0;
  }

  for (int i = 0; i < tamanho-1; i++) {
    if (!((palavra[i] >= 'A' && palavra[i] <= 'Z') || (palavra[i] >= 'a' && palavra[i] <= 'z') || (palavra[i] >= '0' && palavra[i] <= '9') || (palavra[i] == '_') )) {
      // caso um desses if's nao ocorra, o rotulo tambem eh invalido
      return 0;
    }
  }

  // caso tudo esteja correto retornamos 1
  return 1;
}

/* Funcao que verifica a validade do rotulo no seu uso */
int verificaRotuloUso (char* palavra) {

  int verifica = verificaRotuloDefinicao(palavra);
  if (verifica == 0) {
    // o rotulo possui alguma invalidez
    return 0;
  } else {
    int tamanho = strlen(palavra);
    if (palavra[tamanho - 1] == ':') {
      // um rotulo q esta sendo usado nao deve apresentar ":" no seu final
      return 0;
    } else {
      return 1;
    }
  }

}

/* Funcao que verifica a validade dos simbolos */
int verificaSimbolo (char* palavra) {
  int tamanho = strlen(palavra); // tem o '\0'

  if (palavra[0] >= '0' && palavra[0] <= '9') {
    // simbolo eh invalido
    return 0;
  }

  for (int i = 0; i < tamanho-1; i++) {
    if (!((palavra[i] >= 'A' && palavra[i] <= 'Z') || (palavra[i] >= 'a' && palavra[i] <= 'z') || (palavra[i] >= '0' && palavra[i] <= '9') || (palavra[i] == '_') )) {
      // caso um desses if's nao ocorra, o simbolo tambem eh invalido
      return 0;
    }
  }

  // caso tudo esteja correto retornamos 1
  return 1;

}

/* Funcao que verifica a validade dos hexadecimais usados */
int verificaHexadecimal (char* palavra) {
  int tamanho = strlen(palavra);
  if (tamanho == 12) {
    if (!((palavra[0] == '0') && ((palavra[1] == 'x') || (palavra[1] == 'X')))) {
      // se o primeiro digito for diferente de 0 e o segundo de x ou X, o hexadecimal eh invalido
      return 0;
    }
    for (int i = 2; i < tamanho; i++) {
      if (!((palavra[i] >= 'A' && palavra[i] <= 'F') || (palavra[i] >= 'a' && palavra[i] <= 'f') || (palavra[i] >= '0' || palavra[i] <= '9'))) {
        // caso um desses if's nao ocorra, o hexadecimal tambem eh invalido
        return 0;
      }
    }
    return 1;
  } else {
    return 0;
  }

}

/* Funcao que verifica a validade dos decimais usados */
int verificaDecimal (char* palavra) {
  int tamanho = strlen(palavra) - 1;
    for (int i = 0; i < tamanho-1; i++) {
      if (!(palavra[i] >= 0 && palavra[i] <= 9)) {
        return 0;
      }
    }

  return 1;

}

/* Funcao que avisa sobre um erro do tipo LEXICO*/
void erroLexico (int linha) {
  fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n",linha);
  exit(1);
}

/* Funcao que avisa sobre um erro do tipo GRAMATICAL*/
void erroGramatical (int linha) {
  fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",linha);
  exit(1);
}

/* Funcao que cria os tokens das diretivas */
void criaTokenDiretiva(char* palavra) {

  Token novoToken = criaToken(Diretiva, palavra, numLinha);
  adicionarToken(novoToken);

}

/* Funcao que cria os tokens dos rotulos */
void criaTokenRotulo(char* palavra) {
  Token novoToken = criaToken(DefRotulo, palavra, numLinha);
  adicionarToken(novoToken);
}

/* Funcao que cria os tokens das instrucoes */
void criaTokenInstrucao(char* palavra) {
  Token novoToken = criaToken(Instrucao, palavra, numLinha);
  adicionarToken(novoToken);
}

/* Funcao que cria os tokens dos hexadecimais */
void criaTokenHexadecimal(char* palavra) {
  Token novoToken = criaToken(Hexadecimal, palavra, numLinha);
  adicionarToken(novoToken);

}

/* Funcao que cria os tokens dos decimais */
void criaTokenDecimal(char* palavra) {
  Token novoToken = criaToken(Decimal, palavra, numLinha);
  adicionarToken(novoToken);
}

/* Funcao que cria os tokens dos nomes */
void criaTokenNome(char* palavra) {
  Token novoToken = criaToken(Nome, palavra, numLinha);
  adicionarToken(novoToken);
}

/* Funcao generica para criacao dos tokens */
Token criaToken(TipoDoToken tipo, char* palavra, unsigned linha) {
  Token newToken;
  newToken.tipo = tipo;
  newToken.palavra = palavra;
  newToken.linha = linha;

  return newToken;

}

/* Funcao que remove as aspas de uma palavra que vem entre aspas */
char* removeAspas(char* palavra) {
  int tamanho = strlen(palavra) - 1;
  char* copia = malloc(sizeof(tamanho-2));
  int j = 0;

  for (int i = 0; i < tamanho; i++) {
    if (palavra[i] == '\"') {
      continue;
    } else {
      copia[j] = palavra[i];
      j++;
    }
  }

  return copia;

}
