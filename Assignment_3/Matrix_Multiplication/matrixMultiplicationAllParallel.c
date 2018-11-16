#include "mpi.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    Matrix Multiplication of Two Sparse Matrix Input Files.
 
    Student Name/ID: Varun Jain 21963986
    Student Name/ID: Kieron Ho 20500057
 */

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

#define TEMP_ARRAY_SIZE 5000
#define FINAL_ARRAY_SIZE 10000

int row1[TEMP_ARRAY_SIZE];              //row1 - stores all the column 1 index values for the row column from file input 1
int column1[TEMP_ARRAY_SIZE];           //column1 - stores all the column 2 index values from file input 1
float value1[TEMP_ARRAY_SIZE];          //row1 - stores all the column 3 values for the specified (row1, column1) from file input 1

int row2[TEMP_ARRAY_SIZE];              //row2 - stores all the column 1 index values for the row column from file input 2
int column2[TEMP_ARRAY_SIZE];           //column2 - stores all the column 2 index values from file input 2
float value2[TEMP_ARRAY_SIZE];          //row2 - stores all the column 3 values for the specified (row2, column2) from file input 2

int rowOutput[TEMP_ARRAY_SIZE];    //rowOutput - stores the temporary rows received from workers
int columnOutput[TEMP_ARRAY_SIZE]; //columnOutput - stores the temporary columns received from workers
float valueOutput[TEMP_ARRAY_SIZE];//valueOutput - stores the temporary values received from workers

int file_one_number_of_lines;
int file_two_number_of_lines;
int resultSize;

void printMatrix(int row[], int column[], float values[], int elementsInMatrix){//Working
int i;
for(i = 0 ; i < elementsInMatrix ; i++){
        if(values[i] != 0){
    printf("%d %d %.2f\n", row[i], column[i], values[i]);
        }
}
}

int maximumValue(int length, int* row)
{
    int max = row[0];
    for(int i = 1; i < length; i++)
    {
        if(row[i] > max)
        {
            max = row[i];
        }
    }
    return max;
}

/*
    input_file_number_of_lines() function returns the number of lines in a file
*/
int input_file_number_of_lines(char filename[])
{
    int linecount = 0;
    char character;

    FILE *fp = fopen(filename, "r");

    while((character = fgetc(fp)) != EOF)
    {
        if(character == '\n')   linecount++;
    }

    fclose(fp);

    return linecount;
}


void readfile(char filename[], int *row, int *column, float *value)
{
    char line[BUFSIZ];

    FILE *fp = fopen(filename, "r");

    int i = 0;

    while(fgets(line, sizeof line, fp) != NULL)
    {
        row[i] = atoi(strtok(line, " "));
        column[i] = atoi(strtok(NULL, " "));
        value[i] = atof(strtok(NULL, " "));
        i++;
    }
    fclose(fp);
}



void allocate_space_file1(char filename[])
{
    file_one_number_of_lines = input_file_number_of_lines(filename);
    readfile(filename, row1, column1, value1);
}

void allocate_space_file2(char filename[])
{
    file_two_number_of_lines = input_file_number_of_lines(filename);
    readfile(filename, row2, column2, value2);
}


int main(int argc, char* argv[])
{
    int numtasks, taskid, numworkers, source, dest, mtype, indices, aveindex,//indices replaces rows, aveindex replaces averow
    extra, offset, i, j, k, rc, threadsProvided;
        int startTime = clock()*1000/CLOCKS_PER_SEC;

    if(argc > 3){
            int threads = atoi(argv[3]);
    omp_set_num_threads(threads);
    }else omp_set_num_threads(1);

    MPI_Status status;
    MPI_Init_thread(&argc,&argv, MPI_THREAD_FUNNELED, &threadsProvided);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    if(numtasks < 2){
        printf("Need at least two MPI tasks. Quitting...\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }
    numworkers = numtasks-1;



    /***************************** master task ********************************/
    if(taskid == MASTER)
    {
    allocate_space_file1(argv[1]);
    allocate_space_file2(argv[2]);
    aveindex = file_one_number_of_lines/numworkers;
    extra = file_one_number_of_lines%numworkers;
    offset = 0;
    mtype = FROM_MASTER;

    /*Should initialise master-only data structures here*/

    int rowFinal[FINAL_ARRAY_SIZE];
    int columnFinal[FINAL_ARRAY_SIZE];
    float valueFinal[FINAL_ARRAY_SIZE];

    for(dest=1; dest<=numworkers; dest++)
    {
    indices = (dest <= extra) ? aveindex+1 : aveindex;//number of tuples to send from matrix
    MPI_Send(&indices, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);//the number of tuples
    MPI_Send(&file_two_number_of_lines, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);//send the amount of tuples in the second matrix

    //send the trimmed matrix A data
    MPI_Send(&row1[offset], indices, MPI_INT, dest, mtype, MPI_COMM_WORLD);//trimmed matrix one rows
    MPI_Send(&column1[offset], indices, MPI_INT, dest, mtype, MPI_COMM_WORLD);//trimmed matrix one columns
    MPI_Send(&value1[offset], indices, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);//trimmed matrix one value

    //send the matrix B data
    MPI_Send(&row2, file_two_number_of_lines, MPI_INT, dest, mtype, MPI_COMM_WORLD);//all matrix two rows
    MPI_Send(&column2, file_two_number_of_lines, MPI_INT, dest, mtype, MPI_COMM_WORLD);//all matrix two columns
    MPI_Send(&value2, file_two_number_of_lines, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);//all matrix two value

    //prepare the next message
    offset = offset + indices;
    }

    /*Receive results from worker tasks*/
            int finalArrayDataSize = 0;
    mtype = FROM_WORKER;
    for(i = 1 ; i <= numworkers; i++)
    {
        source = i;
        MPI_Recv(&resultSize, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rowOutput, TEMP_ARRAY_SIZE, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);//resultant row
        MPI_Recv(&columnOutput, TEMP_ARRAY_SIZE, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);//resultant column
        MPI_Recv(&valueOutput, TEMP_ARRAY_SIZE, MPI_FLOAT, source, mtype, MPI_COMM_WORLD, &status);//resultant values

        /*Process new data here. designed to add to the existing results arrays marked "final"*/
        //needs to be run sequentially
        if(resultSize>0){
            for(j = 0 ; j < resultSize ; j++){//for each received output tuple, non zeroes
                    for(k = 0 ; k < FINAL_ARRAY_SIZE ; k++){//go through the final tuples
            if(rowOutput[j] == rowFinal[k] && columnOutput[j] == columnFinal[k]){//if element result exists, update
                valueFinal[k] += valueOutput[j];//the added multiplication value

                break;//if you have found a place to put it, move along
                }
                else if(columnFinal[k] <= 0 && rowFinal[k] <= 0 && valueFinal[k] == 0){//if element didn't exist, add it(provided it isn't 0, 0)
                    rowFinal[k] = rowOutput[j];
                    columnFinal[k] = columnOutput[j];
                    valueFinal[k] = valueOutput[j];
                    finalArrayDataSize++;
                    break;
                    }
            }
        }
        }
    }

            /*Print results here*/
                                printMatrix(rowFinal, columnFinal, valueFinal,finalArrayDataSize);
                                    int endTime = clock()*1000/CLOCKS_PER_SEC;

    printf("Time taken: %dms\n", endTime - startTime);
    }

    /************************** Worker Tasks *******************************************/
    if(taskid > MASTER)
    {


        mtype = FROM_MASTER;
        //Receive the parameters
        MPI_Recv(&indices, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&file_two_number_of_lines, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        //Receive the data for matrix one trimmed
        MPI_Recv(&row1, indices, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&column1, indices, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&value1, indices, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);

        //Receive all of data for matrix two
        MPI_Recv(&row2, file_two_number_of_lines, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&column2, file_two_number_of_lines, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&value2, file_two_number_of_lines, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);

            int resultantIndex = 0;
            int matTwoIndex;
            file_one_number_of_lines = indices;
    #pragma omp parallel for private(matTwoIndex)
    for(int matOneIndex = 0; matOneIndex < file_one_number_of_lines; matOneIndex++)//for each line in file one,
    {
        for(matTwoIndex = 0; matTwoIndex < file_two_number_of_lines; matTwoIndex++)//for each line in file two,
        {
            float sum_of_the_values = 0;
            if(column1[matOneIndex] == row2[matTwoIndex])
            {
                sum_of_the_values += value1[matOneIndex]*value2[matTwoIndex];
            }
            if(sum_of_the_values != 0)
            {
                rowOutput[resultantIndex] =  row1[matOneIndex];
                columnOutput[resultantIndex] = column2[matTwoIndex];
                valueOutput[resultantIndex] = sum_of_the_values;
                resultantIndex++;
            }
        }

    }
    int total_number_resultant_columns = sizeof(rowOutput)/sizeof(rowOutput[0]);
    resultSize = total_number_resultant_columns;

    for(int output_index = 0; output_index < total_number_resultant_columns; output_index++)
    {
        for(int resultant_index = output_index+1; resultant_index < total_number_resultant_columns; resultant_index++)
        {
            if(rowOutput[output_index] == rowOutput[resultant_index] && columnOutput[output_index] == columnOutput[resultant_index])
            {
                valueOutput[output_index] = valueOutput[output_index]+valueOutput[resultant_index];
                rowOutput[resultant_index] = 0;
                columnOutput[resultant_index] = 0;
                valueOutput[resultant_index] = 0;
            }
        }
    }
    if(indices>0){
    }
        mtype = FROM_WORKER;
        MPI_Send(&resultSize, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&rowOutput, TEMP_ARRAY_SIZE, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&columnOutput, TEMP_ARRAY_SIZE, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&valueOutput, TEMP_ARRAY_SIZE, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD);
    }

    MPI_Finalize();



    return 0;
}


