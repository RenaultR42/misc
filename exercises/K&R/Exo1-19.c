#include <stdio.h>
#define MAXLIGNE 1000

int lireligne(char ligne[], int maxligne);
void inverser(char s[]);

int main(void)
{
  int l;
  char ligne[MAXLIGNE];

  while((l = lireligne(ligne, MAXLIGNE)) > 0)
    {
      inverser(ligne);
      printf("%s", ligne);
    }
  
  return 0;
}

void inverser(char s[])
{
  int i, j;
  char carac;

  for(j = 0 ; s[j] != '\0' ; j++)
    ;

  j -= 2;

  for(i = 0 ; i < j ; i++)
    {
      carac = s[j];
      s[j] = s[i];
      s[i] = carac;
      j--;
    }
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
