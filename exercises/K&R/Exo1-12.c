#include <stdio.h>

int main(void)
{
  int c;

  while((c = getchar()) != EOF)
    {
      if((c == '\t') || (c == ' '))
	{
	  do
	    c = getchar();
	  while((c == '\t') || (c == ' '));
	  putchar('\n');
	  putchar(c);
	}

      else
	putchar(c);
    }

  return 0;
}
