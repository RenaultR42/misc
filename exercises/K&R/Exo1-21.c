#include <stdio.h>
#define MAXLIGNE 1000
#define TAILLE 4

int lireligne(char ligne[], int maxligne);

int main(void)
{
  int l, i, j, espace, espacement;
  char ligne[MAXLIGNE];

  while((l = lireligne(ligne, MAXLIGNE)) > 0)
    {
      espace = 0;

      for(i = 0 ; i < l ; i++)
	{
	  if((((i+1) % TAILLE) == 0) || (TAILLE % (i+1) == 0))
	    espacement = TAILLE;
	  
	  else if((((i+1) % TAILLE) != 0) && (i+1) > TAILLE)
	    espacement = (i+1) % TAILLE;

	  else
	    espacement  = TAILLE % (i+1);

	  if(ligne[i] == ' ')
	    espace++;

	  else
	    espace = 0;
		  
	  if((espace == espacement) && espace > 0)
	    {
	      i -= espace;
	      l -= espace;
	      ligne[i] = '\t';

	      for(j = i+1 ; j < l ; j++)
		ligne[j] = ligne[j + espace];

	      espace = 0;
	      ligne[l] = '\0';
	    }	  
	}
      printf("%s", ligne);
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
