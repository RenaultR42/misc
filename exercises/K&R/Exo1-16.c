#include <stdio.h>
#define MAXLIGNE 1000

int lireligne(char ligne[], int maxligne);
void copier(char vers[], char de[]);

int main(void)
{
  int l, max;
  char ligne[MAXLIGNE], pluslongue[MAXLIGNE];
  
  max = 0;

  while((l = lireligne(ligne, MAXLIGNE)) > 0)
    {
      printf("Cette chaîne a %d caractères\n", l);
      if(l > max)
	{
	  max = l;
	  copier(pluslongue, ligne);
	}
    }

  if(max > 0)
    printf("La plus longue des lignes avec %d caractères est : \n\"%s\"\n", max, pluslongue);
  
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

void copier(char vers[], char de[])
{
  int i;

  i = 0;

  while((vers[i] = de[i]) != '\0')
    i++;
}
