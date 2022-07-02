#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Declare variables to be used
double matrixC[1024][1024]={0.0};
  int rowsA, colsA;
  int rowsB, colsB;
  int rowsC,colsC;
  double matrixA[1024][1024]={0.0};
  double matrixB[1024][1024]={0.0};
typedef struct data{
    int thread_num;
    int startRow;
    int startCol;
    int endRow;
    int endCol;
} data;

void* multiply(void* p) //method to multiply the matrices
{
    data* thread_data = (data*)p;
    int rowSizeC=rowsA;
    int colSizeC=colsB;
     //printf("\ninthread\nstart row: %d,end row:%d, start col:%d,end col:%d\n",thread_data->startRow,thread_data->endRow,thread_data->startCol,thread_data->endCol);
    for(int row=thread_data->startRow;row<thread_data->endRow;row++){
            int col=0;
        if(row==thread_data->startRow)
            col=thread_data->startCol;
        for(;col<colSizeC;col++){
            double sum=0.0;
            for(int i=0;i<rowSizeC;i++){
                sum+=(matrixA[row][i])*(matrixB[i][col]); //multiply row of matrix A by columnn of matrix B
              //  printf("%lf,%lf,%lf\n",sum,matrixA[row][i],matrixB[i][col]);
            }
           matrixC[row][col]=sum;
        }
    }
    //for last row of each thread
        for(int col=0;col<thread_data->endCol+1;col++){
            double sum=0;
            for(int i=0;i<rowSizeC;i++){
                sum+=(matrixA[thread_data->endRow][i])*(matrixB[i][col]);
            }
           matrixC[thread_data->endRow][col]=sum;

        }

}

int main(){
	//Ask the user for the number of threads
    int numOfThread;
    printf("Enter the number of threads :\n");
   scanf("%d",&numOfThread);
  FILE *fp = NULL;
  int row, col;
 FILE *fptr;

    // opening file
    fptr = fopen("results.txt", "w");
      if(fptr == NULL)
   {
       perror("Error");
      exit(1);
   }

  fp = fopen("matrices.txt", "r");
  while(!feof(fp)){
    //taking first matrix as input
    fscanf(fp,"%d,%d",&rowsA, &colsA);

    for(row = 0; row < rowsA; row++){
      for(col = 0; col < colsA-1; col++){
        fscanf(fp,"%lf,",&matrixA[row][col]);
      }
      fscanf(fp,"%lf\n",&matrixA[row][colsA-1]);
    }
  //taking second matrix as input
    fscanf(fp,"%d,%d",&rowsB, &colsB);

    for(row = 0; row < rowsB; row++){
      for(col = 0; col < colsB-1; col++){
        fscanf(fp,"%lf,",&matrixB[row][col]);
      }
      fscanf(fp,"%lf\n",&matrixB[row][colsB-1]);
    }
    if(colsA!=rowsB)
    printf("Matrices cannot be multiplied\n");
    else{
        if(numOfThread>rowsA*colsB){
            numOfThread=rowsA*colsB;
        }
        rowsC=rowsA;
        colsC=colsB;
        int sizeMatrixC=rowsC*colsC;
        int eachThreadPart=sizeMatrixC/numOfThread;
        int startRow=0;
        int startCol=0;
    //printf("each thread part:%d,rowsC:%d,colsC:%d\n",eachThreadPart,rowsC,colsC);
       pthread_t threads[numOfThread];
        //declaring arguments for thread
        data thread_data[numOfThread];
        int remainder=eachThreadPart%rowsC;
        for (int i = 0; i < numOfThread; i++){
        thread_data[i].thread_num = i;
        thread_data[i].startRow=startRow;
        thread_data[i].startCol=startCol;
        if(remainder==0){
            thread_data[i].endRow=startRow+(eachThreadPart/rowsC)-1;
            thread_data[i].endCol=colsC-1;
            startRow=thread_data[i].endRow+1;
        }
        else{

            thread_data[i].endRow=startRow+eachThreadPart/rowsC;
            thread_data[i].endCol=(startCol+remainder-1)%colsC;
            if((startCol+remainder)>colsC)
                thread_data[i].endRow+=1;
            startRow=thread_data[i].endRow;
            startCol=(thread_data[i].endCol+1)%colsC;
        }
     //   printf("start row:%d, start col:%d,end row:%d,endcol:%d\n",thread_data[i].startRow,thread_data[i].startCol,thread_data[i].endRow,thread_data[i].endCol);
    }
     thread_data[numOfThread-1].endRow=rowsC-1;
     thread_data[numOfThread-1].endCol=colsC-1;
    // Creating n threads
    for (int i = 0; i < numOfThread; i++)
        pthread_create(&threads[i], NULL, multiply, &thread_data[i]);

    // joining 4 threads i.e. waiting for all 4 threads to complete
    for (int i = 0; i < numOfThread; i++)
        pthread_join(threads[i], NULL);
    fprintf(fptr,"%d,%d\n",rowsC,colsC);
      for(int i=0;i<rowsC;i++){
        for(int j=0;j<colsC;j++){
           // printf("%lf",matrixC[i][j]);
            if(j==colsC-1)
                fprintf(fptr, "%lf\n", matrixC[i][j]);
            else
                fprintf(fptr, "%lf,", matrixC[i][j]);

        }
        printf("Done, results output to file\n");
      }

    }


  }
  fclose(fptr);
  fclose(fp);
}
