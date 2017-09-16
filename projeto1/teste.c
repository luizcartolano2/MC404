#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){

char sentence[500], *word;
int ct=0;

puts("Sentence: ");
gets(sentence);

word = strtok( sentence , " " );

while( word != NULL ) {
	printf( "%s ", word );
	word = strtok( NULL, " " );
}

  return 0;

}
