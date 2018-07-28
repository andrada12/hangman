#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ascuns.h"

void retinere_caractere(char mesaj[], char caractere[])
{
	int i=0,k=0,lung=0;

	while(i<=strlen(mesaj))
	{
		if(mesaj[i]!= ' ' && i!= strlen(mesaj))
		{
			i++;
			lung++;
		}
		else
		{
			if(lung==2)
			{
				caractere[k]=mesaj[i-2];
				k++;
			}
			else
				if(lung>2)
				{
					caractere[k]=mesaj[i-lung];
					k++;
					caractere[k]=mesaj[i-1];
					k++;
				}
				i++;
				lung=0;
		}
	}

}

void scriere(char aux[], char caractere[])
{
	int i;
	char c1=aux[0], c2=aux[strlen(aux)-1];
	if(strlen(aux)==1 && !(strchr(caractere,aux[0])) )
		strcpy(aux,"_");
	else
		if(strlen(aux)==2 && !(strchr(caractere,aux[1])) )
			aux[1]='_';
		else
			for(i=0;i<strlen(aux);i++)
			{
				if(aux[i]!=c1 && aux[i]!=c2 && !(strchr(caractere,aux[i])) )
					aux[i]='_';
			}
}


void creare_mesaj_ascuns(char s[], char ascuns[], char caractere[])
{
	int i=0,j,l=strlen(s),p;
	int k=0;
	char aux[50]="\0";
	char spatiu[3]=" ";
	
	
	while(i<=l)
	{
		if(s[i]!=' ' && i!=strlen(s))
			{
				aux[k]=s[i];
				i++;
				k++;
			}
			else
				if( s[i] == ' ' )
					{
						i++;
						scriere(aux,caractere);
						
						strcat(ascuns,aux);
						strcat(ascuns,spatiu);

						p=strlen(aux);
						for(j=0;j<=p;j++)
							aux[j]='\0';
						k=0;
					}
					else
						if(i==strlen(s))
						{	
							
							scriere(aux,caractere);
							strcat(ascuns,aux);
							i++;
						}
	}

}