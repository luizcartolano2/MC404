#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
int main(int argc, char const *argv[]) {

  char* palavra = "________vetor______:";

  char* copia = copiaNomeToken(palavra);

  printf("%s\n",copia);

  return 0;

}
