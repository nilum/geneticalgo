/*
  Given the digits 0 through 9 and the operators +, -, * and /,
  find a sequence that will represent a given target number.
  The operators will be applied sequentially from left to right as you read.

  Problem from: http://www.ai-junkie.com/ga/intro/gat3.html
*/

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

//Chance of a chromosome having *any* mutation.

#include "genetic.h"
int TARGET = 0;
int main (int argc, char *argv[])
{
    srand(time(NULL)); //set the seed
    time_t epoch1;
    epoch1 = time(NULL);
    //printf("\nProgram started at %ld", (long)epoch1);

    if(argc != 2)
        TARGET = 853; //placeholder
    else
    {
        char* endptr = 0;    
        TARGET = strtol(argv[1], &endptr, 10);
    }
    Chrom Chromarray[NUMCHROM] = {0};  //the array that holds this generation's chromosomes
    Chrom *Chromptr[NUMCHROM]; //this will hold the pointers to the previous array, sorted by fitness
    Chrom *cptr;
    Chrom parent[NUMPARENT] = {0}; //these are the weighted chromosomes chosen to breed the next generation
    double avgfitness = 0;
    double totalfitness = 0;
    Chrom *bestfitness;
    int generation = 0;
    double lasttotalfitness = 0;
    float MutMod = 1.0;
    
    cptr = &Chromarray[0];
    bestfitness = &Chromarray[0];
    for(int i = 0; i < NUMCHROM; i++)
    {
        Chromptr[i] = &Chromarray[i];
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
            Chromptr[i] = &(Chromarray[i]);
            totalfitness += cptr->fitness;
            if(bestfitness->fitness < cptr->fitness)
                bestfitness = cptr;
            cptr++;
        }
        sortChromArr(Chromptr, 0, NUMCHROM-1); //qsort takes the rightmost element, not length        
        if(bestfitness->fitness == 100.0)
            break;
        avgfitness = totalfitness/NUMCHROM;
        //printf("\t\tGEN %d\nAverage Fitness is: \t%f\nTotal Fitness is: \t%f\nBest Fitness is: \t%f\n\n", generation, avgfitness, totalfitness, bestfitness->fitness);
        chooseParents(Chromptr, parent, totalfitness);
        breedParents(Chromarray, parent, MutMod);
        generation++;
        avgfitness = 0;
        if(lasttotalfitness >= totalfitness)
        {
            MutMod *= 2; //if last total fitness was greater or equal, we may have hit a rut, so temporarily double the mutation rate
        }
        else
        {
            MutMod = 1;
        }
        lasttotalfitness = totalfitness;
        totalfitness = 0;
    }
    decodeChrom(bestfitness, 1);
    printf("\nDone!\nBest fitness was %f, with a value of %d after %d generations\n", bestfitness->fitness, bestfitness->decoded, generation);

    time_t epoch2;
    epoch2 = time(NULL);
    printf("\nExecution time of %ld seconds", (long)epoch2 - (long)epoch1);
    return 0;
}

void breedParents(struct Chrom carry[], struct Chrom parent[], float MutMod)
{
    unsigned int i = 1;
    Chrom *a = &parent[0];
    Chrom *b = &parent[1];
    int crossover = 0;
    while (i < NUMCHROM)
    {
        //Choose two random parents
        a = &parent[rand()%NUMPARENT];
        b = &parent[rand()%NUMPARENT];

        carry[i-1] = *a; //bring a into the next generation
        carry[i] = *b; //bring b over too
        carry[i-1].fitness = 0;
        carry[i-1].decoded = 0;
        carry[i].fitness = 0;
        carry[i].decoded = 0;

        //Decide whether to crossover
        if(rand() < (RAND_MAX*CROSSOVER))
        {
            //If we crossover, choose a random gene after which to swap with b
            crossover = rand()%NUMGENE;
            while(crossover < NUMGENE)
            {
                carry[i-1].gene[crossover] = b->gene[crossover];
                carry[i].gene[crossover] = a->gene[crossover];
                crossover++;
                //printf("\n\tSwapped genes with fitnesses %f and %f after gene %d\n", a->fitness, b->fitness, random);
            }
        }
        //Decide whether a should get a mutation
        for(int j = 0; j < NUMGENE; j++)
        {
            //If there's a mutation, just choose a random gene to replace with another random, valid gene
            if(rand()<(RAND_MAX*MUTATION*MutMod))
            {
                carry[i].gene[rand()%NUMGENE] = rand()%VALIDGENES;
            }
            if(rand()<(RAND_MAX*MUTATION*MutMod))
            {
                carry[i-1].gene[rand()%NUMGENE] = rand()%VALIDGENES;
            }
            //printf("\n\n\tMUTATION!!!\n");
        }
        i++;
    }
}

void chooseParents(Chrom *carry[], Chrom parent[], double totalfitness)
{
    unsigned int i = 0, j = 0;
    unsigned int numparents = 0;
    double bestfitness = 0;
    while(i < NUMPARENT)
    {
        if((carry[j]->fitness) > bestfitness)
            bestfitness=carry[j]->fitness;
        numparents = (int)(((carry[j]->fitness)/totalfitness)*NUMPARENT);
        //we weight the new parents based on what percent the chromosome contributed to the total fitness
        //printf("\nNumparents for %d is %d\n", i, numparents);
        while((numparents > 0) && i < NUMPARENT)
        {
            parent[i] = *(carry[j]);
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
    Chrom->decoded = 0;
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

double getFitness (int decoded)
{
    
    if (decoded - TARGET == 0)
        return 100;
    double fitness = 0;
/*
  int difference = (decoded - TARGET);
  double temp = 2;
    if(difference < 0)
        difference = -difference;

    for (int i = 0; i < difference; i++)
    {
        temp = temp * temp;
    }
*/
    fitness = (double)1/(decoded - TARGET);
    if (fitness < 0)
        fitness = -fitness;
    return fitness*10;
}
