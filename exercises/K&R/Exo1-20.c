#include <stdio.h>
#define MAXLIGNE 1000
#define TAILLE 4

int lireligne(char ligne[], int maxligne);

int main(void)
{
  int l, i, espace, k, espacement;
  char ligne[MAXLIGNE];

  while((l = lireligne(ligne, MAXLIGNE)) > 0)
    {
      for(i = 0, k = 0; i < l ; i++)
	{
	  if(ligne[i] == '\t')
	    {
	      espacement = TAILLE - (k % TAILLE);

	      for(espace = 1 ; espace <= espacement ; espace++)
		{
		  putchar(' ');
		  k++;
		}
	    }

	  else
	    {
	      putchar(ligne[i]);
	      k++;
	    }
	}
    }
  
  return 0;
}

int lireligne(char s[], int lim)
{
  int c, i;
  
  for(i = 0 ; i < lim-1 && (c = getchar()) != EOF && c != '\n' ; i++)
    s[i] = c;

  if(c == '\n')
    {
      s[i] = c;
      i++;
    }

  s[i] = '\0';

  return i;
}
