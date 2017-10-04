#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int main(int argc, char const *argv[]) {

  int i = 10;
  char* buffer = malloc(sizeof(10));

  sprintf(buffer,"%010X",i);
  printf("%010X\n",i);

  printf("%c%c %c%c%c %c%c %c%c%c\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9]);

  return 0;

}
