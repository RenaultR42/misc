#include <stdio.h>

int main(void)
{
  int c, nbcara = 0;

  while((c = getchar()) != EOF)
    {
      if(c != ' ' || c != '\t' || c != '\n')
	nbcara = 0;

      else
	nbcara++;
    }

  printf("\n%d\n", nbcara);

  return 0;
}
