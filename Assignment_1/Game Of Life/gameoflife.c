#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*
    CITS3402 Assignment 1: Conway's Game of Life
    Description: Simulate Conway's Game of Life using normal sequential processing
    @author 1: Varun Jain 21963986
    @author 2: Kieron Ho 20500057
 */

//generates a 2D array filled with 1's and 0's
int** make2Darray(int arraysizeRow, int arraysizeColumn);
//detects the number of neighbouring cells that are alive
int detect(int** array, int row, int column, int arraySize);
//generates a 2D array filled with 0's
int** makeNEW2Darray(int arraysizeRow, int arraysizeColumn);
//plays the next iteration of the game of life
int** play(int** array, int** newArray, int arraySize);
//stores the intial board into the text file
void intial_board(int arraySize, char* filename);
//stores the final board in the text file
void final_board(int arraySize, char* filename);

//Global Variables
int** array;        //generates 2D board with 1's and 0's
int** newarray;     //stores the evolution of the 2D board


/*------------------------------------THE MAIN FUNCTIONs----------------------------------------------------*/

int main(int argc, char* argv[])
{
    //user-input the size of the array
    int array2DSize = atoi(argv[1]);
    array = make2Darray(array2DSize,array2DSize);
    newarray = makeNEW2Darray(array2DSize,array2DSize);

    char initialboard[] = "initialboard.txt";
    char finalboard[] = "finalboard.txt";

    double delta;

    struct timeval start, end;
    gettimeofday(&start, NULL);
  
    //plays the first iteration of the game
    play(array, newarray, array2DSize);

    intial_board(array2DSize, initialboard);

    for(int e = 0; e < 99; e++)
    {
        for(int i = 0; i < array2DSize; i++)
        {
            for(int j = 0; j < array2DSize; j++)
            {
                array[i][j] = newarray[i][j];
            }
        }
        play(array, newarray, array2DSize);
    }
    gettimeofday(&end, NULL);
    delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    printf("time=%12.10f\n",delta);
    final_board(array2DSize, finalboard);
    return 0;

}



/*-----------------------------2D ARRAY FILLED WITH 1's and 0's-------------------------------------------------------*/

/*
 Generates a 2D Array filled with 1's and 0's with a given size input by the user.
 referenced from: http://pleasemakeanote.blogspot.com/2008/06/2d-arrays-in-c-using-malloc.html
 referenced from: https://www.eskimo.com/~scs/cclass/int/sx9b.html
 */

int** make2Darray(int arraysizeRow, int arraysizeColumn)
{
    int** array;
    array = (int**) malloc(arraysizeRow*sizeof(int*));
    
    for(int row = 0; row < arraysizeRow; row++)
    {
        array[row] = (int*) malloc(arraysizeColumn*sizeof(int));
        for(int column = 0; column < arraysizeColumn; column++)
        {
            array[row][column] = rand()%2;
        }
    }
    return array;
}

/*----------------------------------------2D ARRAY FILLED WITH 0's-------------------------------------------------------*/

/*
 Generates a 2D Array filled with 0's with a given size input by the user.
 Stores the Board after Each Simulation of the Game of Life
 */

int** makeNEW2Darray(int arraysizeRow, int arraysizeColumn)
{
    int** newarray;

    newarray = (int**) malloc(arraysizeRow*sizeof(int*));
    
    for(int row = 0; row < arraysizeRow; row++)
    {
        newarray[row] = (int*) malloc(arraysizeColumn*sizeof(int));
        for(int column = 0; column < arraysizeColumn; column++)
        {
            newarray[row][column] = 0;
        }
    }
    return newarray;
}

/*-----------------------------------------------INITIAL BOARD IN TEXT FILE---------------------------------------------*/

/*
 Writes the 2D 1's and 0's generated board into a text file.
 @param - arraySize: the size of the input
 @param - filename: the text file which stores the 2D array
 */

void intial_board(int arraySize, char* filename)
{
    FILE *fp = fopen(filename, "w+");
    if(fp == NULL)
    {
        perror("Error");
        exit(1);
    }
    
    for(int row = 0; row < arraySize; row++)
    {
        for(int column = 0; column < arraySize; column++)
        {
                fprintf(fp, "%d ", array[row][column]);
        }
        fprintf(fp, "\n");
    }
    
    
    fclose(fp);
}



/*---------------------------------------------- FINAL BOARD IN TEXT FILE----------------------------------------------*/

/*
 Writes the final generated board into a text file.
 @param - arraySize: the size of the input
 @param - filename: the text file which stores the 2D array
 */


void final_board(int arraySize, char* filename)
{
    FILE *fp = fopen(filename, "w+");
    if(fp == NULL)
    {
        perror("Error");
        exit(1);
    }
    
    for(int row = 0; row < arraySize; row++)
    {
        for(int column = 0; column < arraySize; column++)
        {
            fprintf(fp, "%d ", newarray[row][column]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

/*------------------------------------------LIVE NEIGHBOURS-------------------------------------------------------*/

/*
 @param - array: the intial board or the updated board when the game is played more than once
 @param - row: search through a soecifed row of the 2D array
 @param - column: search through specified column of the 2D array
 param arraySize: the size of the 2D array given by the user.
 */

int detect(int** array, int row, int column, int arraySize)
{
    //variable declared and initalised to 0
    int countNeighbours = 0;
     //checks the top upper left cell
    if(column == 0 && row == 0)
    {
        countNeighbours += array[arraySize-1][column];
        countNeighbours += array[row][arraySize-1];
        countNeighbours += array[row+1][column];
        countNeighbours += array[row][column+1];
    }
    //checks the bottom right cell
    else if(column == arraySize-1 && row == arraySize-1)
    {
        countNeighbours += array[row][0];
        countNeighbours += array[0][column];
        countNeighbours += array[row-1][column];
        countNeighbours += array[row][column-1];
    }
      //checks the top right cell
    else if(column == arraySize-1 && row == 0)
    {
        countNeighbours += array[row][0];
        countNeighbours += array[arraySize-1][column];
        countNeighbours += array[row+1][column];
        countNeighbours += array[row][column-1];
    }
    //checks bottom left cell
    else if(column == 0 && row == arraySize-1)
    {
        countNeighbours += array[0][column];
        countNeighbours += array[row][arraySize-1];
        countNeighbours += array[row-1][column];
        countNeighbours += array[row][column+1];
    }
    //the the first column
    else if(column == 0)
    {
        countNeighbours += array[row][arraySize-1];
        countNeighbours += array[row][column+1];
        countNeighbours += array[row-1][column];
        countNeighbours += array[row+1][column];
    }
    //checks the last column
    else if(column == arraySize-1)
    {
        countNeighbours += array[row][0];
        countNeighbours += array[row][column-1];
        countNeighbours += array[row-1][column];
        countNeighbours += array[row+1][column];
    }
     //checks first row
    else if(row == 0)
    {
        countNeighbours += array[arraySize-1][column];
        countNeighbours += array[row][column-1];
        countNeighbours += array[row][column+1];
        countNeighbours += array[row+1][column];
    }
    //checks last row
    else if(row == arraySize-1)
    {
        countNeighbours += array[0][column];
        countNeighbours += array[row-1][column];
        countNeighbours += array[row][column-1];
        countNeighbours += array[row][column+1];
    }
    else
    {
        countNeighbours += array[row][column+1];
        countNeighbours += array[row+1][column];
        countNeighbours += array[row-1][column];
        countNeighbours += array[row][column-1];
    }
    //returns the count for the live cells surrouding a specified cell
    return countNeighbours;
}

/*-----------------------------------------------PLAY THE   GAME-------------------------------------------------------*/

/*
 @param - array: the intial board or the updated board when the game is played more than once
 @param - newArray: store 1's and 0's to the new board, after evaulating the array board
 @param - arraySize: the size of the 2D array given by the user.
 @param - filename: the text file which stores the 2D array
 */

int** play(int** array, int** newArray, int arraySize)
{
    int countNeighbours;

    
    for(int row = 0; row < arraySize; row++)
    {
        for(int column = 0; column < arraySize; column++)
        {
            //returns the number of live cells surrounding the specified row and column by calling the detect function

            countNeighbours = detect(array, row,column, arraySize);
            if((countNeighbours == 2 || countNeighbours == 3) && array[row][column] == 1)
            {
                newArray[row][column] = array[row][column];
            }
            if(countNeighbours == 3 && array[row][column] == 0)
            {
                newArray[row][column] = 1;
            }
            if(countNeighbours < 2 && array[row][column] == 1)
            {
                newArray[row][column] = 0;
            }
            if(countNeighbours > 3 && array[row][column] == 1)
            {
                newArray[row][column] = 0;
            }
        }
    }
    

    //returns the update 2D Array after game rules followed by examining the previous board iteration

    return newArray;
}




