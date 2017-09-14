#include <stdio.h>
#define DEBUT 0
#define FIN 300
#define INTERVALLE 20

int conv_temp_fahr(int celsius)
{
   return 9 * celsius / 5 + 32;
}

int conv_temp_celsius(int fahr)
{
  return 5 * (fahr - 32) / 9;
}

int main(void)
{
  int celsius, fahr;

  printf("Celsius\tFahr\n");

  for(celsius = DEBUT ; celsius <= FIN ; celsius += INTERVALLE)
    {  
      fahr = conv_temp_fahr(celsius);
      printf("%d\t%d\n", celsius, fahr);
    }

  printf("\n\nFahr\tCelsius\n");

  for(fahr = DEBUT ; fahr <= FIN ; fahr += INTERVALLE)
    {
      celsius = conv_temp_celsius(fahr);
      printf("%d\t%d\n", fahr, celsius);
    }

  return 0;
}
