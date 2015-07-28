#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define VALIDGENES 14
#define NUMGENE 13
#define NUMCHROM 100
#define NUMPARENT 100
#define CROSSOVER 0.7
#define MUTATION 0.001
#define GENMAX 250000

typedef struct Chrom
{
    unsigned short gene[NUMGENE];
    int decoded;
    double fitness;
} Chrom;

void decodeChrom (Chrom *Chrom, char print);
double getFitness (int);
void genRandomChrom(Chrom *cptr);
void chooseParents(Chrom *carry[], Chrom parent[], double totalfitness);
void breedParents(Chrom carry[], Chrom parent[], float MutMod);

int sortChromArrPartition (Chrom *arr[], int lo, int hi)
{
//The array is arr, lo should be 0, and hi should be the rightmost element.
    //We also need a temp variable for swapping
    Chrom *temp = 0;
    //Let's pick a pivot!
    int pivotIndex = hi/2; //HALF THE LENGTH, sounds fine
    double pivotValue = (arr[pivotIndex])->fitness; 
    //Now we move the pivot to the end
    temp = arr[hi];
    arr[hi] = arr[pivotIndex];
    arr[pivotIndex] = temp;
    //OK, swapped!
    int storeIndex = lo;
    //Now we "compare remaining array elements against pivotValue = A[hi]"
    for (int i = lo; i < hi; i++)
    {
        if((arr[i])->fitness > pivotValue)
        {
            temp = arr[storeIndex];
            arr[storeIndex] = arr[i];
            arr[i] = temp;
            storeIndex++;
        }
    }
  
    //Swap arr[storeIndex] and arr[hi];
    temp = arr[storeIndex];
    arr[storeIndex] = arr[hi];
    arr[hi] = temp;
    return storeIndex;
}

void sortChromArr (Chrom *arr[], int lo, int hi)
{
    //Let's implement a quicksort
    if (lo < hi)
    {
        int p = sortChromArrPartition(arr, lo, hi);
        sortChromArr (arr, lo, p - 1);
        sortChromArr (arr, p + 1, hi);
    }
}
