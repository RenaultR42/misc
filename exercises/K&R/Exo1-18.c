#include <stdio.h>
#define MAXLIGNE 1000

int lireligne(char ligne[], int maxligne);
int traiterligne(char s[], int taille);

int main(void)
{
  int l;
  char ligne[MAXLIGNE];

  while((l = lireligne(ligne, MAXLIGNE)) >= -1)
    {
      if(l > 0)
	printf("%s",ligne);
    }
  
  return 0;
}

int traiterligne(char s[], int taille)
{
  int i;
  
  for(i = taille - 2 ; i >= 0 && (s[i] == '\t' || s[i] == ' ') ; i--)
    {
      if(s[i] == '\t' || s[i] == ' ')
	{
	  s[i+1] = '\0';
	  s[i] = '\n';
	}
    }

  if(!i)
    i = -1;

  return i;
}

int lireligne(char s[], int lim)
{
  int c, i, autre;
  
  for(i = 0 ; i < lim-1 && (c = getchar()) != EOF && c != '\n' ; i++)
    s[i] = c;

  if(c == '\n')
    {
      s[i] = c;
      i++;
    }

  s[i] = '\0';
  autre = traiterligne(s, i);

  return autre;
}
