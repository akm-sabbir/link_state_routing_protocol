#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include "client.h"

tempHolder stringTokennizer(char str[])
{
  //char * str = " x1 x7 x10 x29 ";
  //char str[] = " x1 x7 x10 x29 ";
  printf ("Splitting string \"%s\" into tokens:\n",str);
  char * pch = new char[100];
  char tempsHolder[10][100];
	  pch = strtok (str," ");
   //= new char*[10];
 /* int i = 0;
  for(i = 0; i  < 10; i++)
	  tempsHolder[i] = new char[100];*/
  int i = 0;
  while (pch != NULL)
  {
   // printf ("%s\n",pch);
	strcpy(tempsHolder[i++],pch);
    pch = strtok (NULL," ");
	printf ("%s\n",tempsHolder[i-1]);
  }

  return tempsHolder;
}