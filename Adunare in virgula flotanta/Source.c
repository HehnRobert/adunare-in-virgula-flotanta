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
	f = fopen("in.txt", "w");
	fprintf(f, "%s\n%s", n1, n2);
	fclose(f);
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

int swap(unsigned int *m1,unsigned int *m2, int *e1, int *e2,int *rez,int *ifs)//daca trebuie, fac swap si stabilesc exponentul rezultatului
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
		(*ifs) = 1;
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

void adjust_r_s(unsigned int *b, int d, unsigned int *r, unsigned int *s,unsigned int z0,unsigned int shift)
{
	if (d == 1)
	{
		(*r) = z0;
		if ((*b)) (*s) = 1;
		else (*s) = 0;
	}
	else if (d == 0)
	{
		(*r) = verifbit((*b), 0);
		(*b) >> 1;
		if ((*b)) (*s) = 1;
		else (*s) = 0;
	}
	else if (d == -1)
	{
		(*r) = verifbit((*b), 0);
		(*b) >> 1;
		(*s) = 0;
		for (int i = 0; i < shift; i++)
		{
			if (verifbit((*b), 0)) {
				(*s) = 1; i = shift;
			}
			(*b) >> 1;
		}
	}
	else
	{
		(*r) = 0;
		(*s) = 0;
	}
}

void rounding(unsigned int r,unsigned int s,unsigned int *zm,int *exp)
{
	unsigned int zon = verifbit((*zm), 0);
	if (r&(zon | s))
	{
		(*zm)++;
	}
	if (verifbit((*zm), 24))
	{
		(*zm) >> 1;
		(*exp)++;
	}
}

void sign_result(int ifs,int ifc)
{
	if (ifs)
	{
		if ((numar1.semn = 0) && (numar2.semn = 1)) rezultat.semn = 1;
		if ((numar1.semn = 1) && (numar2.semn = 0)) rezultat.semn = 0;
	}
	else
	{
		if (!ifc)
		{
			if ((numar1.semn = 0) && (numar2.semn = 1)) rezultat.semn = 0;
			if ((numar1.semn = 1) && (numar2.semn = 0)) rezultat.semn = 1;
		}
		else
		{
			if ((numar1.semn = 0) && (numar2.semn = 1)) rezultat.semn = 1;
			if ((numar1.semn = 1) && (numar2.semn = 0)) rezultat.semn = 0;
		}
	}
}

void pack(int expr,unsigned int mantr)
{
	int i,vecexp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }, vecmant[23] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	f = fopen("in.txt", "a");
	fprintf(f, "\n\nRezultat:\n");
	expr += 127;
	fprintf(f, "%d", rezultat.semn);
	//Transformarea exponentului
	i = 7;
	while (expr)
	{
		vecexp[i--] = expr % 2;
		expr /= 2;
	}
	//Scriere exponent in fisier
	for (i = 0; i < 8; i++)
		fprintf(f, "%d", vecexp[i]);
	//Transformarea mantissei
	i = 22;
	mantr = mantr&(~(1 << 23));
	while (mantr)
	{
		vecmant[i--] = mantr % 2;
		mantr /= 2;
	}
	//Scriere mantissa in fisier
	for (i = 0; i < 23; i++)
	{
		fprintf(f, "%d", vecmant[i]);
	}
	fclose(f);
}

int main()
{
	unsigned int mantn1, mantn2, mantrez, shift, cout, bit, R, S,z0=0;//bit -> (g,r,s)
	int expn1, expn2, exprez, test = 0, deplasare = 0, ifswap = 0, ifcomplement = 0;
	read();
	set();
	mantn1 = transmant(numar1.mantissa);
	mantn2 = transmant(numar2.mantissa);
	expn1 = transexp(numar1.exponent);
	expn2 = transexp(numar2.exponent);
	//printf("\n\n%u\n%u\n%d\n%d\n\n", mantn1, mantn2, expn1, expn2);
	shift = swap(&mantn1, &mantn2, &expn1, &expn2, &exprez,&ifswap);//pasul 2
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
		ifcomplement = 1;
	}
	if (cout && numar1.semn == numar2.semn)//pasul 6
	{
		//bit = bit << 1 | verifbit(mantrez, 0);
		z0 = verifbit(mantrez, 0);
		mantrez = ((mantrez >> 1) | (1 << 23));//nu mai avem nevoie de g -> r devine ce am scos din mantrez si restul este s
		exprez++;
		deplasare = 1;
	}
	else if (!cout && numar1.semn == numar2.semn)
	{
		if (!verifbit(mantrez, 23)) //daca este denormalizat
		{
			deplasare = -1;
			mantrez = ((mantrez << 1) | verifbit(bit, 0));//am introdus in mantrez g-ul si este eliminat din bit...g nu mai conteaza
			exprez--;
			bit = bit >> 1;
			shift--;
			while (verifbit(mantrez, 23) == 0)
			{
				mantrez = (mantrez << 1);
				exprez--;
				deplasare--;
			}
		}
	}




	//-----------------Pasul 7-----------------//
	adjust_r_s(&bit, deplasare, &R, &S,z0,shift);
	//=========================================//

	//-----------------Pasul 8-----------------//
	rounding(R,S,&mantrez,&exprez);
	//=========================================//

	//-----------------Pasul 9-----------------//
	if (numar1.semn != numar2.semn) sign_result(ifswap,ifcomplement);
	//=========================================//

	//-----------------Pasul 10----------------//
	pack(exprez, mantrez);
	//=========================================//




	//*********************TEST*********************//
	printf("\n\n\n%d  %u  %u", rezultat.semn,exprez,mantrez);
	printf("\n\n\t\t\t\t\t\tProgram incheiat cu succes!");
	_getch();
	return 0;
}//ATENTIE, am schimbat exprez din unsigned int int INT!