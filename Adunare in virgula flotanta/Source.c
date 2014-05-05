#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

FILE *f;

typedef struct 
{
	int semn;
	int exponent[8], mantissa[23];
}numar;
numar numar1, numar2, rezultat;
char n1[33], n2[33];

void read()
{//TESTAT - OK
	f = fopen("in.txt", "r");
	fscanf(f, "%s %s", n1, n2);
	fclose(f);
	//printf("%s\n%s\n\n", n1, n2); -> TEST PENTRU CITIRE IN CHAR !ok!
}

void set()
{//TESTAT - OK
	numar1.semn = n1[0] - '0';
	numar2.semn = n2[0] - '0';
	for (int i = 1; i < 9; i++)
	{
		numar1.exponent[i - 1] = n1[i] - '0';
		numar2.exponent[i - 1] = n2[i] - '0';
	}
	for (int i = 9; i < 32; i++)
	{
		numar1.mantissa[i - 9] = n1[i] - '0';
		numar2.mantissa[i - 9] = n2[i] - '0';
	}
	/*for (int i = 0; i < 32; i++)
	{
		if (i == 0) printf("%d  ", numar1.semn);
		else if (i < 9) {
			printf("%d", numar1.exponent[i - 1]); if (i == 8) printf(" ");
		}
		else printf("%d", numar1.mantissa[i - 9]);
	} //->TESTAT PENTRU TRANSFORMARE IN INT !ok!
	printf("\n\n\n\n\n");*/
}

unsigned int transmant(int v[23])//calculez mantissa sub forma de int (din baza 2 in baza 10)
{//TESTAT - OK
	int n=1;
	for (int i = 0; i < 23; i++)
	{
		n = ((n << 1) | v[i]);
	}
	//printf("\n\n Mantissa in baza 10 cu hidden bit inclus: %d\n\n\n\n", n);
	return n;
}

int transexp(int v[8])//calculez exponentul sub forma de int (din baza 2 in baza 10)
{//TESTAT - OK
	int n = 0;
	for (int i = 0; i < 8; i++)
	{
		n = ((n << 1) | v[i]);
	}
	return (n - 127);
}

int swap(unsigned int *m1,unsigned int *m2, int *e1, int *e2,int *rez)//daca trebuie, fac swap si stabilesc exponentul rezultatului
{//TESTAT - OK
	if (((*e1) - (*e2)) < 0)
	{
		unsigned int aux1;
		int aux2;
		aux1 = (*m1);
		(*m1) = (*m2);
		(*m2) = aux1;
		aux2 = (*e1);
		(*e1) = (*e2);
		(*e2) = aux2;
		aux1 = numar1.semn;
		numar1.semn = numar2.semn;
		numar2.semn = aux1;
	}
	(*rez) = (*e1);
	return (*e1) - (*e2);
}

void sign(unsigned int *m)//complementez ultimii 23 de biti (pt ca ne folosim doar de cei 23)
{//TESTAT - OK
	for (int i = 0; i < 24; i++)
	{
		(*m) = ((*m) ^ (1 << i));
	}
	if (verifbit((*m), 0) == 0)
		(*m) = (*m) | 1;
	else
	{
		int i = 0;
		while (verifbit((*m), i))
		{
			if (i <= 23)
				(*m) = (*m) & (~(1 << i));
			else break;
			i++;
		}
	}
}

unsigned int verifbit(unsigned int nr,int poz)//(verific ce bit e pe pozitia poz din numarul nr
{//TESTAT - OK
	return (1 & (nr >> poz));
}

unsigned int shiftmantc(unsigned int *mantn, unsigned int x)//shiftarea mantissei daca aceasta a fost complementata
{
	unsigned int bit = 0;
	for (int i = 1; i <= x; i++)
	{
		bit = (bit << 1) | verifbit(mantn, 0);
		(*mantn) = ((*mantn) >> 1) | (1 << 23);
	}
	return bit;//returnez bit (g,r,s)
}

unsigned int shiftmant(unsigned int *mantn, unsigned int x)//shiftarea mantissei daca aceasta nu a fost complementata
{
	unsigned int bit = 0;
	for (int i = 1; i <= x; i++)
	{
		bit = (bit << 1) | verifbit(mantn, 0);
		(*mantn) = (*mantn) >> 1;
	}
	return bit;//returnez bit (g,r,s)
}

unsigned int sum(unsigned int mantn1, unsigned int mantn2, unsigned int *rez)//calculez suma mantisselor
{
	(*rez) = mantn1 + mantn2;
	return verifbit((*rez), 24);//returnez daca am carry out
}

int main()
{
	unsigned int mantn1, mantn2, mantrez, exprez, shift, cout, bit;//bit -> (g,r,s)
	int expn1, expn2, test = 0;
	read();
	set();
	mantn1 = transmant(numar1.mantissa);
	mantn2 = transmant(numar2.mantissa);
	expn1 = transexp(numar1.exponent);
	expn2 = transexp(numar2.exponent);
	//printf("\n\n%u\n%u\n%d\n%d\n\n", mantn1, mantn2, expn1, expn2);
	shift = swap(&mantn1, &mantn2, &expn1, &expn2, &exprez);//pasul 2
	//printf("\n\n%u\n%u\n%d\n%d\n%d\n\n", mantn1, mantn2, expn1, expn2,shift); ->TESTARE SWAP !ok!
	if (numar1.semn == numar2.semn)//preshifting
	{
		rezultat.semn = numar1.semn;
		bit = shiftmant(&mantn2, shift);//pasul 4
	}
	else
	{
		sign(&mantn2);//pasul 3
		bit = shiftmantc(&mantn2, shift);//pasul 4
	}
	cout = sum(mantn1, mantn2, &mantrez);//pasul 5
	if(cout) mantrez = mantrez & (~(1 << 24));//sterg cout din mantissa rezultatului
	if (numar1.semn != numar2.semn && cout == 0 && verifbit(mantrez, 23))//pasul 5
	{
		sign(&mantrez);
	}
	/*if (cout && numar1.semn != numar2.semn)
	{
		if (verifbit(mantrez, 22) == 1)
		{
			mantrez = ((mantrez << 1) | verifbit(bit, 0));
			exprez--;
		}
	}*/
	if (cout && numar1.semn == numar2.semn)//pasul 6
	{
		bit = bit << 1 | verifbit(mantrez, 0);
		mantrez = ((mantrez >> 1) | (1 << 23));//nu mai avem nevoie de g -> r devinte ce am scos din mantrez si restul este s
		exprez++;
	}
	else if (!cout && numar1.semn == numar2.semn)
	{
		if (!verifbit(mantrez, 23)) //daca este denormalizat
		{
			mantrez = ((mantrez << 1) | verifbit(bit, 0));//am introdus in mantrez g-ul si este eliminat din bit...g nu mai conteaza
			exprez--;
			bit = bit >> 1;
			while (verifbit(mantrez, 23) == 0)
			{
				mantrez = (mantrez << 1);
				exprez--;
			}
		}
	}//!!!!!!!bit=***sr!!!!!!!!
	//round to nearest!

	//*********************TEST*********************//
	printf("\n\n\n%d  %u  %u", rezultat.semn,exprez,mantrez);
	printf("\n\n\t\t\t\t\t\tProgram incheiat cu succes!");
	_getch();
	return 0;
}