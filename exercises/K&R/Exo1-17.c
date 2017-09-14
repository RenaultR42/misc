#include <stdio.h>
#define MAXLIGNE 1000

int lireligne(char ligne[], int maxligne);
void copier(char vers[], char de[]);

int main(void)
{
  int l;
  char ligne[MAXLIGNE];

  while((l = lireligne(ligne, MAXLIGNE)) > 0)
    {
      if(l >= 80)
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
