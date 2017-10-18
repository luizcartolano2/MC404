#include <stdio.h>
#include <math.h>

int main(int argc, char const *argv[]) {

  int num_rows, n, k;
  int mat [23][23]={1};
  scanf("%d",&num_rows);

  for (int i = 1; i < (num_rows); i++) {
    for (int j = 1; j < (num_rows); j++) {
      mat[i][j] = mat[i-1][j-1] + mat[i-1][j];
      if(mat[i][j] != 0){
        printf("%08x ",mat[i][j]);
      }
    }
    printf("\n");
  }
  return 0;
}
