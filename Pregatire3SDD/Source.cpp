#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Candidat
{
	int cod;
	char* nume;
	char* facultate;
	float medie;
	int salariu;
}Candidat;
typedef struct Nod
{
	Candidat* info;
	struct Nod* next;
};
typedef struct HashT
{
	Nod** vector;//vector de candidati ->dinamici
	int dim;//nr elem din vector
}HashT;

Candidat* initializareCandidat(int cod, const char* nume, const char* facultate, float medie,int salariu)
{
	Candidat* candidat = (Candidat*)malloc(sizeof(Candidat));
	candidat->cod = cod;
	candidat->nume = (char*)malloc(strlen(nume)+1);
	strcpy(candidat->nume, nume);
	candidat->facultate = (char*)malloc(strlen(facultate) + 1);
	strcpy(candidat->facultate, facultate);
	candidat->medie = medie;
	candidat->salariu = salariu;
	return candidat;
}
Nod* initializareNod(Candidat* candidat)
{
	Nod* nodNou = (Nod*)malloc(sizeof(Nod));
	nodNou->info = candidat;
	nodNou->next = NULL;
	return nodNou;
}
HashT* initializareHashT()
{
	HashT* hashT = (HashT*)malloc(sizeof(HashT));
	hashT->dim = 20;
	hashT->vector = (Nod**)malloc(sizeof(Nod*) * hashT->dim);
	for (int i = 0; i < hashT->dim; i++)
	{
		hashT->vector[i] = NULL;
	}
	return hashT;
}

int functieHash(HashT* hashT, const char* facultate)//determina indexul la care se insereaza
//fiecare nod din lista simpla in vectorul suport
{
	return (facultate[0] - 'A') % hashT->dim; //asa se face pe char*
	//return CNP % hashT->dim; //asa se face pt int unsigned etc..
}

//inserare chain
HashT* inserareHashT(HashT* hashT, Candidat* candidat)
{
	if (hashT->vector != NULL)
	{
		int poz = functieHash(hashT,candidat->facultate);
		Nod* nodNou = initializareNod(candidat);
		if (hashT->vector[poz] == NULL)//evitare coliziuni - cautare pozitie libera
		{
			hashT->vector[poz] = nodNou;
			
		}
		else//daca nu gasim poz libera continuam cautarea in vector
		{
			Nod* temp = hashT->vector[poz];
			while (temp->next != NULL)
			{
				temp = temp->next;
			}

			temp->next = nodNou;
		}
	}
	return hashT;
	
}
void afisareHashT(HashT* hashT)
{
	if (hashT->vector != NULL)
	{
		for (int i = 0; i < hashT->dim; i++)
		{
			if (hashT->vector[i] != NULL)
			{
				Nod* temp = hashT->vector[i];
				while (temp != NULL)
				{
					printf("Pozitie: %d\n", i);
					printf("%d %s %s  %5.2f %d\n", temp->info->cod, temp->info->nume,
						temp->info->facultate, temp->info->medie, temp->info->salariu);
					temp = temp->next;
				}
			}
		}
	}
}
void dezalocareHashT(HashT* hashT)
{
	if (hashT->vector != NULL)
	{
		for (int i = 0; i < hashT->dim; i++)
		{
			if (hashT->vector[i] != NULL)
			{
				Nod* temp = hashT->vector[i];
				while (temp != NULL)
				{
					Nod* nodDeSters = temp;
					temp = temp->next;
					free(nodDeSters->info->nume);
					free(nodDeSters->info->facultate);
					free(nodDeSters->info);
					free(nodDeSters);
					nodDeSters = temp;
				}
			}
			
		}
		free(hashT->vector);
		free(hashT);
	}
}
//functie care det cea mai mare medie de absolvire pt o anumita facultate data ca param
float medieMax(HashT* hashT,  const char* facultate)
{
	if (hashT->vector != NULL)
	{
		//mereu cand trb sa caut dupa cheie se fooseste functia hash
		//altfel se foloseste vectorul cu for
		int poz = functieHash(hashT, facultate);
		float medieMax = 0;

		Nod* temp = hashT->vector[poz];
		while (temp != NULL)
		{
			if (strcmp(temp->info->facultate, facultate) == 0)
			{
				if (temp->info->medie > medieMax)
				{
					medieMax = temp->info->medie;
				}
			}
			temp = temp->next;
		}
		return medieMax;
	}
	
}
//actualizare salariu ->candidat dat ca param si sal nou tot ca param
void actualizare(HashT* hashT, int cod, int salariuNou)
{
	for (int i = 0; i < hashT->dim; i++)
	{
		Nod* temp = hashT->vector[i];
		while (temp != NULL)
		{
			if (temp->info->cod == cod)
			{
				temp->info->salariu = salariuNou;
			}
			temp = temp->next;
		}
	}
}
//stergere candidat al carui nume este dat ca param
void stergereCandidat(HashT* hashT,const  char* nume)
{
	for (int i = 0; i < hashT->dim; i++)
	{
		Nod* temp = hashT->vector[i];
		Nod* prev = NULL;
		while (temp != NULL)
		{
			if (strcmp(temp->info->nume, nume) == 0)
			{
				if (prev == NULL)//stergere primul nod
				{
					hashT->vector[i] = temp->next;
				}
				else
				{
					prev->next = temp->next;
				}
				Nod* nodDeSters = temp;
				temp = temp->next;
				free(nodDeSters->info->nume);
				free(nodDeSters->info->facultate);
				free(nodDeSters->info);
				free(nodDeSters);
			
			}
			else
			{
				prev = temp;
				temp = temp->next;
			}

		}
	}
}


void main()
{
	
	FILE* f = fopen("Candidat.txt", "r");
	if (f != NULL)
	{
		char bufferLine[300], sep_list[] = ",\n";
		HashT* hashT = initializareHashT();

		while (fgets(bufferLine, sizeof(bufferLine), f))
		{
			char* token = strtok(bufferLine, sep_list);
			int cod = (int)atoi(token);

			token = strtok(NULL, sep_list);
			char* nume = (char*)malloc(strlen(token) + 1);
			strcpy(nume, token);

			token = strtok(NULL, sep_list);
			char* facultate = (char*)malloc(strlen(token) + 1);
			strcpy(facultate, token);

			token = strtok(NULL, sep_list);
			float medie = (float)atof(token);

			token = strtok(NULL, sep_list);
			int salariu = (int)atoi(token);


			Candidat* candidat = initializareCandidat(cod, nume, facultate, medie, salariu);
			hashT = inserareHashT(hashT, candidat);


		}
		fclose(f);
		afisareHashT(hashT);
		

		printf("Cerinta 2: \n");
		float medie = 0;
		 medie = medieMax(hashT, "CSIE");
		printf("%5.2f\n", medie);

		printf("Cerinta 3: \n");
		actualizare(hashT,112, 6000);
		afisareHashT(hashT);

		printf("Cerinta 4: \n");
		stergereCandidat(hashT, "David");
		afisareHashT(hashT);

		dezalocareHashT(hashT);
	}
}