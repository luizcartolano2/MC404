/**************************************
* Aluno: Luiz Eduardo T. C. Cartolano *
* RA: 183012                          *
* Turma: B                            *
***************************************/
#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declaracao do escopo de funcoes q vao ser utilizadas para verificar a veracidade das palavras encontradas */
int verificaDiretiva (char *palavra);
int verificaInstrucao (char *palavra);
int verificaRotuloDefinicao (char *palavra);
int verificaHexadecimal (char* palavra);
int verificaDecimal (char* palavra);
int verificaNome (char* palavra);

/* Funcoes que lidam com os erros */
int achaErroLexico(Token token);
int achaErroGramatical();

/* Funcao que manipula as palavras lidas */
void manipulaPalavra (char* palavra);
char* removeAspas(char* palavra);

/* Funcao que cria os tokens */
Token criaToken(TipoDoToken tipo, char* palavra, unsigned linha);

/* copia de funcao da string.h que talvez nao funcionasse no libc dos labs do IC */
char* strsep_404 (char **stringp, const char *delim);

/* variaveis globais */
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
  numLinha = 0;

  linha = strsep_404(&entrada,"\n");
  numLinha++;
  while (linha != NULL) {
    palavra = strtok(linha," \t");
    while (palavra != NULL) {
      if (strcmp(palavra,"#") == 0) {
        palavra = NULL;
      } else {
          manipulaPalavra(palavra);
          palavra = strtok(NULL," \t");
        }
      }
      linha = strsep_404(&entrada,"\n");
      numLinha++;
    }

  if(achaErroGramatical() == 0)
    return 1;

  imprimeListaTokens();

  return 0;

}

/*
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem;
 *  0 caso não haja erro.
 */
char* strsep_404 (char **stringp, const char *delim) {
  char *begin, *end;
  begin = *stringp;

  if (begin == NULL)
    return NULL;

  if (delim[0] == '\0' || delim[1] == '\0') {
    char ch = delim[0];
    if (ch == '\0')
      end = NULL;
    else {
      if (*begin == ch)
        end = begin;
      else if (*begin == '\0')
        end = NULL;
      else
        end = strchr(begin + 1, ch);
    }
  } else
     end = strpbrk(begin, delim);

  if (end) {
    *end++ = '\0';
    *stringp = end;
  } else
    *stringp = NULL;

  return begin;

}

/*
 * Funcao que recebe um Token e verifica se ele possui ou nao erros lexicos
 * Argumentos:
 *  1 - se nao encontrar um erro lexico
 *  0 - se encontrar um erro lexico
 */
int achaErroLexico(Token token) {
  int numDeTokens = getNumberOfTokens();
  int verifica;

  if (token.tipo == Diretiva) {
    verifica = verificaDiretiva(token.palavra);
    if (verifica == 0) {
      fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n",token.linha);
      return 0;
    }
    /* Se for um rotulo */
  } else if (token.tipo == DefRotulo) {
    verifica = verificaRotuloDefinicao(token.palavra);
    if (verifica == 0) {
      fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n",token.linha);
      return 0;
    }
    /* Se for um Hexadecimal */
  } else if(token.tipo == Hexadecimal) {
    verifica = verificaHexadecimal(token.palavra);
    if (verifica == 0) {
      fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n",token.linha);
      return 0;
    }
    /* Se for um Decimal */
  } else if (token.tipo == Decimal) {
    verifica = verificaDecimal(token.palavra);
    if (verifica == 0) {
      fprintf(stderr, "oi\n");
      fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n",token.linha);
      return 0;
    }
  } else if (token.tipo == Nome) {
    verifica = verificaNome(token.palavra);
    if (verifica == 0) {
      fprintf(stderr, "oi2\n");
      fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n",token.linha);
      return 0;
    }
  }

  return 1;

}

/*
 * Funcao que percorre o vetor de Tokens encontrando erros gramaticais
 * Argumentos:
 *  A funcao nao possui argumentos pois ela acessa o vetor de tokens que ja eh visivel para ela
 * Retorna:
 *  1 - se nao existirem erros gramaticais
 *  0 - se existirem erros gramaticais
 */
int achaErroGramatical() {
  Token primeiroToken = recuperaToken(0);
  int linhaAtual = primeiroToken.linha;
  int linhaAnterior = linhaAtual - 1;
  int numDeTokens = getNumberOfTokens();

  if (numDeTokens == 0) {
    return 1;
  }

  for (int i = 0; i < numDeTokens; i++) {
    Token tokenRecuperado = recuperaToken(i);
    if (achaErroLexico(tokenRecuperado) == 0) {
      return 0;
    }
    linhaAtual = tokenRecuperado.linha;
    /* Aqui verficamos se a primeira palavra de cada linha é de fato, uma instrucao, rotulo ou diretiva */
    if (linhaAtual != linhaAnterior) {
      if ((tokenRecuperado.tipo == Nome) || (tokenRecuperado.tipo == Decimal) || (tokenRecuperado.tipo == Hexadecimal)) {
        fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
        return 0;
      } else{
        linhaAnterior = linhaAtual;
      }
    }
    if (tokenRecuperado.tipo == Diretiva) {
      if (i == numDeTokens + 1) {
        /* Se uma diretiva estiver na ultima posicao do vetor de tokens ela nao sera seguida por nada, o que eh um erro gramatical*/
        fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
        return 0;
      }
      else if (strcmp((tokenRecuperado.palavra),".set") == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (achaErroLexico(proximoToken) == 0) {
          return 0;
        }
        /* a primeira palavra que segue um .set DEVE ser um nome */
        if (!(proximoToken.tipo == Nome)) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i+2 <= numDeTokens) {
          Token proxProxToken = recuperaToken(i+2);
          if (achaErroLexico(proxProxToken) == 0) {
            return 0;
          }
          /* a segunda palavra que segue um .set DEVE ser um Hexadecimal ou um Decimal */
          if (!((proxProxToken.tipo == Hexadecimal) || (proxProxToken.tipo == Decimal))) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        } else {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i + 3 <= numDeTokens){
          Token token_tres = recuperaToken(i+3);
          if (token_tres.linha == tokenRecuperado.linha) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
      }
      else if (strcmp((tokenRecuperado.palavra),".org") == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (achaErroLexico(proximoToken) == 0) {
          return 0;
        }
        if (proximoToken.tipo == Decimal) {
          int numero = verificaDecimal(proximoToken.palavra);
          if (numero < 0 || numero > 1023) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
        if (((proximoToken.tipo != Hexadecimal) && (proximoToken.tipo != Decimal))) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i + 2 <= numDeTokens) {
          Token proxProxToken = recuperaToken(i+2);
          if (proxProxToken.linha == tokenRecuperado.linha) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
      }
      else if (strcmp((tokenRecuperado.palavra),".align") == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (achaErroLexico(proximoToken) == 0) {
          return 0;
        }
        if (proximoToken.tipo == Decimal) {
          int numero = verificaDecimal(proximoToken.palavra);
          if (numero < 0 || numero > 1023) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
        if (!(proximoToken.tipo == Decimal)) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i + 2 <= numDeTokens) {
          Token proxProxToken = recuperaToken(i+2);
          if (proxProxToken.linha == tokenRecuperado.linha) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
      }
      else if (strcmp((tokenRecuperado.palavra),".wfill") == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (achaErroLexico(proximoToken) == 0) {
          return 0;
        }
        if (proximoToken.tipo == Decimal) {
          int numero = verificaDecimal(proximoToken.palavra);
          if (numero < 0 || numero > 1023) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
        /* a primeira palavra que segue um .wfill DEVE ser um Decimal */
        if (!(proximoToken.tipo == Decimal)) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i+2 <= numDeTokens) {
          Token proxProxToken = recuperaToken(i+2);
          if (achaErroLexico(proxProxToken) == 0) {
            return 0;
          }
          /* a segunda palavra que segue um .wfill DEVE ser um Hexadecimal ou um Decimal ou um Nome ou um Rotulo */
          if (!((proxProxToken.tipo == Hexadecimal) || (proxProxToken.tipo == Decimal) || (proxProxToken.tipo == DefRotulo) || (proxProxToken.tipo == Nome))) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        } else {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i + 3 <= numDeTokens){
          Token token_tres = recuperaToken(i+3);
          if (token_tres.linha == tokenRecuperado.linha) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }

      }
      else if (strcmp((tokenRecuperado.palavra),".word") == 0) {
        Token proximoToken = recuperaToken(i+1);
        if (achaErroLexico(proximoToken) == 0) {
          return 0;
        }
        if (!((proximoToken.tipo == Hexadecimal) || (proximoToken.tipo == Decimal) || (proximoToken.tipo == DefRotulo) || (proximoToken.tipo == Nome))) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i + 2 <= numDeTokens) {
          Token proxProxToken = recuperaToken(i+2);
          if (proxProxToken.linha == tokenRecuperado.linha) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
      }}
    else if (tokenRecuperado.tipo == Instrucao) {
      if (!((strcmp(tokenRecuperado.palavra,"LOADmq") == 0) || (strcmp(tokenRecuperado.palavra,"LSH") == 0) || (strcmp(tokenRecuperado.palavra,"RSH") == 0))) {
        if (i == numDeTokens + 1) {
          /* com excessao das instrucoes LOAD_MQ, LSH, RSH, todas as demais devem ser seguidas de alguma coisa (nome, decimal ou hexadecimal),
          logo ocupar a ultima posicao do vetor de tokes eh um erro gramatical */
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        Token proximoToken = recuperaToken(i+1);
        if (achaErroLexico(proximoToken) == 0) {
          return 0;
        }
        if (!((proximoToken.tipo == Hexadecimal) || (proximoToken.tipo == Decimal) || (proximoToken.tipo == Nome))) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
        if (i + 2 <= numDeTokens) {
          Token proxProxToken = recuperaToken(i+2);
          if (proxProxToken.linha == tokenRecuperado.linha) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
      } else {
        if (i + 1 <= numDeTokens) {
          Token proximoToken = recuperaToken(i+1);
          if (proximoToken.linha == tokenRecuperado.linha) {
            fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
            return 0;
          }
        }
      }
    }
    else if (tokenRecuperado.tipo == DefRotulo) {
      /* um rotulo sempre deve iniciar uma linha em linguagem de montagem */
      if (i > 0) {
        Token tokenAnterior = recuperaToken(i-1);
        if (tokenAnterior.linha == tokenRecuperado.linha) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
      }
      if (i + 1 <= numDeTokens) {
        Token proximoToken = recuperaToken(i+1);
        if (achaErroLexico(proximoToken) == 0) {
          return 0;
        }
        if (proximoToken.tipo == Nome || proximoToken.tipo == Hexadecimal || proximoToken.tipo == Decimal) {
          fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n",tokenRecuperado.linha);
          return 0;
        }
      }
    }
  }

  return 1;

}

/*
 * Funcao que manipula as palavras que sao lidas
 * Argumentos:
 *  palavra - palavra a ser processada pela funcao
 * Retorna:
 *  A funcao nao possui retorno, pois ao longo do seu funcionamento ela chama outras funcoes que serao
  responsaveis por criar os tokens
 */
void manipulaPalavra (char* palavra) {
  int tamanho = strlen(palavra);

  /* Verifica se eh uma diretiva*/
  if (palavra[0] == '.') {
    Token novoToken = criaToken(Diretiva, palavra, numLinha);
    adicionarToken(novoToken);
    /* Verifica se eh um rotulo*/
  } else if (palavra[tamanho-1] == ':') {
    Token novoToken = criaToken(DefRotulo, palavra, numLinha);
    adicionarToken(novoToken);
    /* Verifica se eh uma instrucao */
  } else if (verificaInstrucao(palavra)) {
    Token novoToken = criaToken(Instrucao, palavra, numLinha);
    adicionarToken(novoToken);
    /* Verifica se a palavra tem aspas */
  } else if (palavra[0] == '\"') {
    char* semAspas = removeAspas(palavra);
    /* Verifica se eh um hexadecimal */
    if (semAspas[0] == '0' && ((semAspas[1] == 'x') || (semAspas[1] == 'X'))) {
      Token novoToken = criaToken(Hexadecimal, semAspas, numLinha);
      adicionarToken(novoToken);
      /* Verifica se eh um decimal */
    } else if (verificaDecimal(semAspas)) {
      Token novoToken = criaToken(Decimal, semAspas, numLinha);
      adicionarToken(novoToken);
    } else {
      /* eh um nome */
      Token novoToken = criaToken(Nome, semAspas, numLinha);
      adicionarToken(novoToken);
    }
    /* Verifica se eh um hexadecimal */
  } else if (palavra[0] == '0' && ((palavra[1] == 'x') || (palavra[1] == 'X'))) {
    Token novoToken = criaToken(Hexadecimal, palavra, numLinha);
    adicionarToken(novoToken);
    /* Verifica se eh um decimal */
  } else if (verificaDecimal(palavra)) {
    Token novoToken = criaToken(Decimal, palavra, numLinha);
    adicionarToken(novoToken);
    /* eh um nome */
  } else {
    Token novoToken = criaToken(Nome, palavra, numLinha);
    adicionarToken(novoToken);
  }
}

/*
 * Funcao que verifica a validade da diretiva
 * Argumentos:
 *  palavra - palavra a ser processada pela funcao
 * Retorna:
 *  1, 2, 3, 4 ou 5 - se a palavra for uma diretiva valida
 *  0 - se a palavra for uma diretiva invalida
 */
int verificaDiretiva (char* palavra) {
  /* verifica se eh um .set */
  if (strcmp(palavra,".set") == 0) {
    return 1;
    /* verifica se eh um .org */
  } else if (strcmp(palavra,".org") == 0) {
    return 2;
    /* verifica se eh um .align */
  } else if (strcmp(palavra,".align") == 0) {
    return 3;
    /* verifica se eh um .wfill */
  } else if (strcmp(palavra,".wfill") == 0) {
    return 4;
    /* verifica se eh um .word */
  } else if (strcmp(palavra,".word") == 0) {
    return 5;
    /* a diretiva eh invalida */
  } else {
    return 0;
  }
}

/*
 * Funcao que verifica a validade da diretiva
 * Argumentos:
 *  palavra - palavra a ser processada pela funcao
 * Retorna:
 *  1 a 16 - se a palavra for uma instrucao valida
 *  0 - se a palavra for uma instrucao invalida
 */
int verificaInstrucao (char* palavra) {
  /* verifica se eh um LOAD_M */
  if (strcmp(palavra,"LOAD") == 0) {
    return 1;
    /* verifica se eh um LOAD_MENOS_M */
  } else if (strcmp(palavra,"LOAD-") == 0) {
    return 1;
    /* verifica se eh um LOAD_MODULO */
  } else if (strcmp(palavra,"LOAD|") == 0) {
    return 2;
    /* verifica se eh um LOAD_MQ */
  } else if (strcmp(palavra,"LOADmq") == 0) {
    return 3;
    /* verifica se eh um LOAD_MQ_MX */
  } else if (strcmp(palavra,"LOADmq_mx") == 0) {
    return 4;
    /* verifica se eh um STOR */
  } else if (strcmp(palavra,"STOR") == 0) {
    return 5;
    /* verifica se eh um JUMP */
  } else if (strcmp(palavra,"JUMP") == 0) {
    return 6;
    /* verifica se eh um JUMP_COND */
  } else if (strcmp(palavra,"JMP+") == 0) {
    return 7;
    /* verifica se eh um ADD */
  } else if (strcmp(palavra,"ADD") == 0) {
    return 8;
    /* verifica se eh um ADD_MODULO */
  } else if (strcmp(palavra,"ADD|") == 0) {
    return 9;
    /* verifica se eh um SUB */
  } else if (strcmp(palavra,"SUB") == 0) {
    return 10;
    /* verifica se eh um SUB_MODULO */
  } else if (strcmp(palavra,"SUB|") == 0) {
    return 11;
    /* verifica se eh um MUL */
  } else if (strcmp(palavra,"MUL") == 0) {
    return 12;
    /* verifica se eh um DIV */
  } else if (strcmp(palavra,"DIV") == 0) {
    return 13;
    /* verifica se eh um LSH */
  } else if (strcmp(palavra,"LSH") == 0) {
    return 14;
    /* verifica se eh um RSh */
  } else if (strcmp(palavra,"RSH") == 0) {
    return 15;
    /* verifica se eh um STOR_M */
  } else if (strcmp(palavra,"STORA") == 0) {
    return 16;
    /* instrucao eh invalida */
  } else {
    return 0;
  }

}

/*
 * Funcao que verifica a validade do rotulo na sua definicao
 * Argumentos:
 *  palavra - palavra a ser processada pela funcao
 * Retorna:
 *  1 - se a palavra for um rotulo valida
 *  0 - se a palavra for um rotulo invalida
 */
int verificaRotuloDefinicao (char* palavra) {
  int tamanho = strlen(palavra);

  /* Rotulos nao podem inciar com numero */
  if (palavra[0] >= '0' && palavra[0] <= '9') {
    return 0;
  }

  /* Rotulos devem conter apenas letras, numeros ou underscore no seu corpo (com excessao dos : finais) */
  for (int i = 0; i < tamanho-1; i++) {
    if (!((palavra[i] >= 'A' && palavra[i] <= 'Z') || (palavra[i] >= 'a' && palavra[i] <= 'z') || (palavra[i] >= '0' && palavra[i] <= '9') || (palavra[i] == '_') )) {
      // caso um desses if's nao ocorra, o rotulo tambem eh invalido
      return 0;
    }
  }

  /* Rotulo eh valido */
  return 1;
}

/*
 * Funcao que verifica a validade dos hexadecimais usados
 * Argumentos:
 *  palavra - palavra a ser processada pela funcao
 * Retorna:
 *  1 - se a palavra for um hexadecimal valido
 *  0 - se a palavra for um hexadecimal invalido
 */
int verificaHexadecimal (char* palavra) {
  int tamanho = strlen(palavra);
  /* Hexadecimais devem conter 12 digitos*/
  if (tamanho == 12) {
    /* o primeiro digito deve ser um '0' e o segundo um 'x' ou 'X'*/
    if (!((palavra[0] == '0') && ((palavra[1] == 'x') || (palavra[1] == 'X')))) {
      // o hexadecimal eh invalido
      return 0;
    }
    /* Os demais digitos devem estar entre 0 e 9 ou entre a/A e f/F */
    for (int i = 2; i < tamanho; i++) {
      if (!((palavra[i] >= 'A' && palavra[i] <= 'F') || (palavra[i] >= 'a' && palavra[i] <= 'f') || (palavra[i] >= '0' || palavra[i] <= '9'))) {
        // caso uma dessas consdicoes nao ocorra, o hexadecimal tambem eh invalido
        return 0;
      }
    }
    // hexadecimal valido
    return 1;
  } else {
    // hexadecimal invalido
    return 0;
  }

}

/*
 * Funcao que verifica a validade dos decimais usados
 * Argumentos:
 *  palavra - palavra a ser processada pela funcao
 * Retorna:
 *  decimal - se a palavra for um decimal valido
 *  0 - se a palavra for um decimal invalido
 */
int verificaDecimal (char* palavra) {

  int inteiro = atoi(palavra);
  return inteiro;

}

/*
 * Funcao que verifica a validade dos nomes usados
 * Argumentos:
 *  palavra - palavra a ser processada pela funcao
 * Retorna:
 *  1 - se a palavra for um nome valido
 *  0 - se a palavra for um nome invalido
 */
int verificaNome (char* palavra) {
  int tamanho = strlen(palavra) - 1;

  if (palavra[0] >= '0' && palavra[0] <= '9') {
    return 0;
  }

  for (int i = 0; i < tamanho; i++) {
    if (!((palavra[i] >= 'A' && palavra[i] <= 'Z') || (palavra[i] >= 'a' && palavra[i] <= 'z') || (palavra[i] >= '0' && palavra[i] <= '9') || (palavra[i] == '_') )) {
      return 0;
    }
  }

  return 1;

}

/*
 * Funcao generica para criacao dos tokens
 * Argumentos:
 *   tipo - tipo do token a ser criado
 *   palavra - palavra do token a ser criado
 *   linha - linha do token a ser criado
 * Retorna:
 *   newToken - novo token preenchido com as informacoes fornecidas
 */
Token criaToken(TipoDoToken tipo, char* palavra, unsigned linha) {
  Token newToken;
  newToken.tipo = tipo;
  newToken.palavra = palavra;
  newToken.linha = linha;

  return newToken;

}

/*
 * Funcao que remove as aspas de uma palavra que vem entre aspas
 * Argumentos:
 *   palavra - palavra cujas aspas devem ser removidas
 * Retorna:
 *   copia - palavra da entrada sem as aspas
 */
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
