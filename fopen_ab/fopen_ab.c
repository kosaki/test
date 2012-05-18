#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
  FILE *file;
  char stra[] = "AAAAAAAA";
  char strb[] = "BBBB";

  file = fopen("hoge", "ab");
  fwrite(stra, strlen(stra), 1, file);
  fseek(file, 2, SEEK_SET);
  fwrite(strb, strlen(strb), 1, file);

  return 0;
}
