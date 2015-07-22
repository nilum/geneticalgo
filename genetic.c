/*
  Given the digits 0 through 9 and the operators +, -, * and /,
  find a sequence that will represent a given target number.
  The operators will be applied sequentially from left to right as you read.

  Problem from: http://www.ai-junkie.com/ga/intro/gat3.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
/*
  Gene encoding:
  0:         0000  
  1:         0001  
  2:         0010  
  3:         0011  
  4:         0100
  5:         0101  
  6:         0110
  7:         0111
  8:         1000
  9:         1001
  +:         1010    A
  -:         1011    B
  *:         1100    C
  /:         1101    D

  E, F left unused

  Every integer must be followed by an operator and then another integer
*/

//2A2B3 should decode to 2+2-3
#define VALIDGENES 14
#define TARGET 869
#define NUMGENE 10
#define NUMCHROM 100
#define NUMPARENT 100
#define CROSSOVER 0.7
#define MUTATION 0.01*NUMGENE
#define GENMAX 100000
//we evaluate whether to mutate once per chromosome, rather than once per gene, but want the same effect as checking every gene
//this saves precious CPU time

typedef struct Chrom
{
    unsigned short gene[NUMGENE];
    unsigned int decoded;
    float fitness;
} Chrom;

void decodeChrom (Chrom *Chrom, char print);
float getFitness (int);
void genRandomChrom(Chrom *cptr);
void chooseParents(Chrom carry[], Chrom parent[], float totalfitness);
void breedParents(Chrom carry[], Chrom parent[]);

int main (void)
{
    srand(42); //set the seed

    Chrom Chromarray[NUMCHROM] = {0};  //the array that holds this generation's chromosomes
    Chrom testchrom = {0};
    Chrom *cptr = &testchrom;
    Chrom parent[NUMPARENT] = {0}; //these are the weighted chromosomes chosen to breed the next generation
    float avgfitness = 0;
    float totalfitness = 0;
    Chrom *bestfitness = &testchrom;
    int generation = 0;
    
    for(int i = 0; i < NUMGENE; i++)
    {
        if (i%2 == 0)
            cptr->gene[i] = (unsigned short)i;
        else
            cptr->gene[i] = 0xA;
        printf("%x\t", cptr->gene[i]);
    }

    decodeChrom(cptr, 0);
    printf("\n%d\t%f\n", cptr->decoded, cptr->fitness);

    cptr = &Chromarray[0];
    for(int i = 0; i < NUMCHROM; i++)
    {
        genRandomChrom(cptr);
        totalfitness += cptr->fitness;
        if(bestfitness->fitness < cptr->fitness)
            bestfitness = cptr;
        cptr++;
    }

    totalfitness = 0;
    avgfitness = 0;
    while(generation < GENMAX)
    {
        cptr = &Chromarray[0];
        for(int i = 0; i < NUMCHROM; i++)
        {
            decodeChrom(cptr, 0);
            totalfitness += cptr->fitness;
            if(bestfitness->fitness < cptr->fitness)
                bestfitness = cptr;
            cptr++;
        }
        if(bestfitness->fitness == 1.0)
            break;
        avgfitness = totalfitness/NUMCHROM;
        printf("\t\tGEN %d\nAverage Fitness is: \t%f\nTotal Fitness is: \t%f\nBest Fitness is: \t%f\n\n", generation, avgfitness, totalfitness, bestfitness->fitness);
        chooseParents(Chromarray, parent, totalfitness);
        breedParents(Chromarray, parent);
        generation++;
        avgfitness = 0;
        totalfitness = 0;
    }
    decodeChrom(bestfitness, 1);
    printf("\nDone!\nBest fitness was %f, value of %d, with the genes:\n", bestfitness->fitness, bestfitness->decoded);
    for(int i = 0; i < NUMGENE; i++)
    {
        printf("%x\t",bestfitness->gene[i]);
    }
    return 0;
}

void breedParents(struct Chrom carry[], struct Chrom parent[])
{
    unsigned int i = 0;
    Chrom *a = &parent[0];
    Chrom *b = &parent[1];
    int random = 0;
    while (i < NUMCHROM)
    {
        //Choose two random parents
        a = &parent[rand()%NUMPARENT];
        b = &parent[rand()%NUMPARENT];

        carry[i] = *a; //bring a into the next generation

        //Decide whether to crossover a
        if(rand() < (RAND_MAX*CROSSOVER))
        {
            //If we crossover, choose a random gene after which to swap with b
            random = rand()%NUMGENE;
            while(random < NUMGENE)
            {
                carry[i].gene[random] = b->gene[random];
                random++;
                //printf("\n\tSwapped genes with fitnesses %f and %f after gene %d\n", a->fitness, b->fitness, random);
            }
        }
        //Decide whether a should get a mutation
        if(rand() < (RAND_MAX*MUTATION))
        {
            //If there's a mutation, just choose a random gene to replace with another random, valid gene
            carry[i].gene[rand()%NUMGENE] = rand()%VALIDGENES;
            //printf("\n\n\tMUTATION!!!\n");
        }
        i++;
    }
}

void chooseParents(struct Chrom carry[], struct Chrom parent[], float totalfitness)
{
//TODO: Sort the arrays first before calling this function.
    unsigned int i = 0, j = 0;
    unsigned int numparents = 0;
    float bestfitness = 0;
    while(i < NUMPARENT)
    {
        if(carry[j].fitness > bestfitness)
            bestfitness=carry[j].fitness;
        numparents = (int)((carry[j].fitness/totalfitness)*NUMPARENT); //we weight the new parents based on what percent the chromosome contributed to the total fitness
        //printf("\nNumparents for %d is %d\n", i, numparents);
        while(numparents > 0)
        {
            parent[i] = carry[j];
            //printf("%d ", j); //uncomment this to see which chromosomes we choose
            numparents--;
            i++;
        }
        if(++j == NUMCHROM)
            j = 0;
        if(i == numparents)
            break; //oops!
    }
    
}

void genRandomChrom (Chrom *cptr)
{
    for(int i = 0; i < NUMGENE; i++)
    {
        cptr->gene[i] = (rand()%VALIDGENES);
        //printf("%x\t", cptr->gene[i]);
    }
    decodeChrom(cptr, 0);
}

void decodeChrom (Chrom *Chrom, char print)
{
    int a = UINT_MAX, b = UINT_MAX;
    int decoded = 0;
    int op = 0;
    for(int i = 0; i < NUMGENE; i++)
    {
        if((Chrom->gene[i] < 0xA) && (a == UINT_MAX))
            a = Chrom->gene[i];
        if ((Chrom->gene[i] > 9) && (Chrom->gene[i] < 0xE) && (a != UINT_MAX) && (op == 0))
            op = i;
        else if ((Chrom->gene[i] < 0xA) && op)
        {
            b = Chrom->gene[i];
            switch(Chrom->gene[op])
            {
                case 0xA : //+
                    decoded = a + b;
                    if(print)
                        printf("\n%d + %d", a, b);
                    break;
                case 0xB : //-
                    decoded = a - b;
                    if(print)
                        printf("\n%d - %d", a, b);
                    break;
                case 0xC : // *
                    decoded = a * b;
                    if(print)
                        printf("\n%d * %d", a, b);
                    break;
                case 0xD : ///
                    if (b == 0)
                    {
                        b = UINT_MAX;
                        continue;
                    }
                    decoded = a / b;
                    if(print)
                        printf("\n%d / %d", a, b);
                    break;
                default :
                    exit(-1);
            }
            a = decoded;
            b = UINT_MAX;
            op = 0;
        }
    }
    Chrom->fitness = getFitness(decoded);
    Chrom->decoded = decoded;
 }

float getFitness (int decoded)
{
    if (decoded - TARGET == 0)
        return 1;
    float fitness = (decoded > TARGET) ? (1/(float)(decoded - TARGET)) : (1/(float)(TARGET - decoded));
    //printf("\nFitness is %f\n\n", fitness);
    return fitness;
}
