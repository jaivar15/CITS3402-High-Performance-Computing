#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Matrix Multiplication of Two Sparse Matrix Input Files.
 
    Student Name/ID: Varun Jain 21963986
    Student Name/ID: Kieron Ho 20500057
 */

//-----------------------------Global Variables---------------------------------

int* row1;                      //row1 - stores all the column 1 index values for the row column from file input 1
int* column1;                   //column1 - stores all the column 2 index values from file input 1
float* value1;                  //row1 - stores all the column 3 values for the specified (row1, column1) from file input 1

int* row2;                      //row2 - stores all the column 1 index values for the row column from file input 2
int* column2;                   //column2 - stores all the column 2 index values from file input 2
float* value2;                  //row2 - stores all the column 3 values for the specified (row2, column2) from file input 2

int rowOutput[10000];           //rowOutput - store final unique row index value
int columnOutput[10000];        //columnOutput - store final unique column index value
float valueOutput[10000];       //valueOutput - store final value afer computation

int file_one_number_of_lines;   //stores the total number of lines in text file 1
int file_two_number_of_lines;   //stores the total number of lines in text file 2



//-----------------------------Final Matrix Computation-------------------------------


void compute_matrix_multiplication(char filename[])
{
    FILE *fp = fopen(filename, "w+");
    
    int total_number_resultant_columns = sizeof(rowOutput)/sizeof(rowOutput[0]);
    //used nested loops in order to find the duplicate values in the 1D arrays
    //iterate through one dimensional array starting at index 0
    for(int output_index = 0; output_index < total_number_resultant_columns; output_index++)
    {
        //iterate through one dimensional array starting at index 1
        for(int resultant_index = output_index+1; resultant_index < total_number_resultant_columns; resultant_index++)
        {
            //checks whether the ith position of the rowOutput and columnOutput is the same
            //as the jth position of the rowOutput and columnOutput.
            if(rowOutput[output_index] == rowOutput[resultant_index] && columnOutput[output_index] == columnOutput[resultant_index])
            {
                //if the row and column indexs match then add the values of the duplicate row and column indexes
                valueOutput[output_index] = valueOutput[output_index]+valueOutput[resultant_index];
                //replace all duplicate values for rowOutput, columnOutput and valueOutput with 0's
                //ensures there are only unique sets of row and column index pairs
                rowOutput[resultant_index] = 0;
                columnOutput[resultant_index] = 0;
                valueOutput[resultant_index] = 0;
            }
        }
        //results print out on terminal and stores the data in matrixmultiplication.txt in the form of a triplet
        //output should look like - rowOutput columnOutput valueOutput
        //returns only the non zero and unique values.
        if(rowOutput[output_index] != 0 && columnOutput[output_index] != 0 && valueOutput[output_index] != 0)
        {
            printf("%d %d %f\n", rowOutput[output_index], columnOutput[output_index], valueOutput[output_index]);
            fprintf(fp, "%d %d %f\n", rowOutput[output_index], columnOutput[output_index], valueOutput[output_index]);
        }
    }
}

//--------------------------------Matrix Multiplication------------------------------

void multiply()
{
    int resultantIndex = 0;
    //iterate through file input 1 matrix
    for(int matOneIndex = 0; matOneIndex < file_one_number_of_lines; matOneIndex++)
    {
        //iterate through file input 2 matrix
        for(int matTwoIndex = 0; matTwoIndex < file_two_number_of_lines; matTwoIndex++)
        {
            float sum_of_the_values = 0;
            //check whether the column index for file input 1 is the
            //same as the row index for file input 2
            if(column1[matOneIndex] == row2[matTwoIndex])
            {
                //if true, multiply the values for those column and row index values.
                sum_of_the_values += value1[matOneIndex]*value2[matTwoIndex];
            }
            //checks whether sum_of_the_values is a non-zero
            if(sum_of_the_values != 0)
            {
                //store the row index from file input 1
                rowOutput[resultantIndex] =  row1[matOneIndex];
                //store the column index from file input 2
                columnOutput[resultantIndex] = column2[matTwoIndex];
                //store non-zero value in valueOuput
                valueOutput[resultantIndex] = sum_of_the_values;
                resultantIndex++;
            }
        }

    }
}

//-----------------------------Maximum Value---------------------------------

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

//-----------------------------Total Number of Line------------------------

/*
    input_file_number_of_lines() function returns the number of lines in a file
*/
int input_file_number_of_lines(char filename[])
{
    //intalise line counter
    int linecount = 0;
    char character;
    
    FILE *fp = fopen(filename, "r");

    while((character = fgetc(fp)) != EOF)
    {
        if(character == '\n')   linecount++;
    }

    fclose(fp);
    //linecount will store the number o files in a a file
    return linecount;
}

//-----------------------------Readfile---------------------------------

void readfile(char filename[], int *row, int *column, float *value)
{
    char line[BUFSIZ];

    FILE *fp = fopen(filename, "r");

    int i = 0;

    while(fgets(line, sizeof line, fp) != NULL)
    {
        //stores column 1 - the row indexes
        row[i] = atoi(strtok(line, " "));
        //stores column 2 - the column indexes
        column[i] = atoi(strtok(NULL, " "));
        //stores column 3 - the value of the row and column index
        value[i] = atof(strtok(NULL, " "));
        i++;
    }
    fclose(fp);
}


//------------------sparse file 1 input--------------------------

void allocate_space_file1(char filename[])
{
    //returns the number of lines in file input 1
    file_one_number_of_lines = input_file_number_of_lines(filename);
    //creates dynamic memory for row1, column1 and value1 array
    row1 = (int*)malloc(file_one_number_of_lines*sizeof(int));
    column1 = (int*)malloc(file_one_number_of_lines*sizeof(int));
    value1 = (float*)malloc(file_one_number_of_lines*sizeof(float));
    //call readfile() to store values in row1, column1, value1 array
    readfile(filename, row1, column1, value1);
}

//------------------sparse file 2 input--------------------------

void allocate_space_file2(char filename[])
{
    //return the total number of lines in file input 2
    file_two_number_of_lines = input_file_number_of_lines(filename);
    //creates dynamic memory for row1, column1 and value1 array
    row2 = (int*)malloc(file_two_number_of_lines*sizeof(int));
    column2 = (int*)malloc(file_two_number_of_lines*sizeof(int));
    value2 = (float*)malloc(file_two_number_of_lines*sizeof(float));
    //call readfile() to store values in row2, column2, value2 array
    readfile(filename, row2, column2, value2);
}

//----------------------Main() Executation-----------------------

int main(int argc, char* argv[])
{
    //fileinput one
    allocate_space_file1(argv[1]);
    //fileinput two
    allocate_space_file2(argv[2]);
    char matrixMultiplicationComputation[] = "matrixmultiplication.txt";
    multiply();
    compute_matrix_multiplication(matrixMultiplicationComputation);
    return 0;
}
