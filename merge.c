#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>


void child(char passedArray, int* myArray, int* other, int* final, int index, int left, int right){
    int p = (left+right)/2;
    char* str = malloc(sizeof(char)*512);
    pid_t myPID, parentPID;

    char notPassed;

    if(passedArray == 'x'){
    	notPassed = 'y';
    }

    if(passedArray == 'y'){
    	notPassed = 'x';
    }

    myPID = getpid();

    int input;

    if(left >= right){
        if(myArray[index] > other[p]){
            input = 1;
        } else {
        	input = 2;
        }
    } else if(myArray[index] < other[p]){
    	input = 3;
    } else if(myArray[index] > other[p]){
    	input =4;
    }

    switch(input){
    	case 1:
    		final[index+p+1] = myArray[index];
            sprintf(str, "      $$$ M-PROC(%d): about to write %c[%d] = %d into position %d of the output array\n", myPID, passedArray, index, myArray[index], (index+p+1));      
            write(1, str, strlen(str));
            exit(0);
            // break;

        case 2:
        	// input = 1;
	        final[index+p] = myArray[index];
	        sprintf(str, "      $$$ M-PROC(%d): about to write %c[%d] = %d into position %d of the output array\n", myPID, passedArray, index, myArray[index], (index+p));      
	        write(1, str, strlen(str));
	        exit(0); 
	        // break;

	    case 3:
	    	sprintf(str, "      $$$ M-PROC(%d): %c[%d] = %d is found to be between  %c[%d] = %d and %c[%d] = %d\n", myPID, passedArray, index, myArray[index], notPassed, p, other[p], notPassed, left, other[left]);
        	write(1, str, strlen(str));
        	child(passedArray, myArray, other, final, index, left, p-1);
        	break;

        case 4:
        	child(passedArray, myArray, other, final, index, p+1, right);
        	sprintf(str, "      $$$ M-PROC(%d): %c[%d] = %d is found to be between  %c[%d] = %d and %c[%d] = %d\n", myPID, passedArray, index, myArray[index], notPassed, p, other[p], notPassed, right, other[right]);
        	write(1, str, strlen(str));
        	break;

    }
    
    exit(0);
}




int main(int argc, char ** argv){

	int i, m, n, shmIDX, shmIDY, shmIDF, A;  
    int* shmPtrX;
    int* shmPtrY;
    int* shmPtrF;
    char* str = malloc(sizeof(char)*512);
    char* out  = malloc(sizeof(char)*1024);

    pid_t childX, childY, parentPID, myPID;

    shmIDX = atoi(argv[1]);
    shmIDY = atoi(argv[2]);
    shmIDF = atoi(argv[3]);
    m      = atoi(argv[4]);
    n      = atoi(argv[5]);

    shmPtrX = (int*) shmat(shmIDX, NULL, 0);
    shmPtrY = (int*) shmat(shmIDY, NULL, 0);
    shmPtrF = (int*) shmat(shmIDF, NULL, 0);
  
    parentPID = getppid();
    myPID     = getpid();

    for(i = 0; i< (m+n); i++){ 
        if(fork() == 0){

            write(1, str, strlen(str));
            if(i < m){
                sprintf(str, "      $$$ M-PROC(%d): handling x[%d] = %d\n", myPID, i, shmPtrX[i]); 
                write(1, str, strlen(str));
                child('x', shmPtrX, shmPtrY, shmPtrF, i, 0, n-1);
            } else {
                sprintf(str, "      $$$ M-PROC(%d): handling y[%d] = %d\n", myPID, i-m, shmPtrY[i-m]); 
                write(1, str, strlen(str));
                child('y', shmPtrY, shmPtrX, shmPtrF, i-m, 0, m-1);
            }
        } else {
            wait(NULL);
        }
    }

    shmdt((void*) shmPtrX);
    shmdt((void*) shmPtrY);
    shmdt((void*) shmPtrF);
    exit(1);
}














