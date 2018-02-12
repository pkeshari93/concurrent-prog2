#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char ** argv){

	int shmIDA, shmIDX, shmIDY, shmIDF, size, temp, i, j, m, n, biggest;
	int * shmPtrA;
	int* shmPtrX;
    int* shmPtrY;
    int* shmPtrF;

	char * token;
	char* out = malloc(sizeof(char)*1024); 
	char* str = malloc(sizeof(char)*1024);
	char* str1 = malloc(sizeof(char)*1024);
	char* str2 = malloc(sizeof(char)*1024);
	char* str3 = malloc(sizeof(char)*1024);
	char* str4 = malloc(sizeof(char)*1024);
    char* str5 = malloc(sizeof(char)*512);
    char* str6 = malloc(sizeof(char)*512);
    char* str7 = malloc(sizeof(char)*512);
    char* str8 = malloc(sizeof(char)*512);
	char* tempstr = malloc(sizeof(char)*1024);
	char* strBig = malloc(sizeof(char)*1024);
	key_t keyA, keyX, keyY, keyF;
	char * argsQ[5];
	char * argsM[7];
	pid_t childQ, childM, parent;





	/*A = Qsort, X = merge 1, Y = merge 2, F = merge final*/
    keyA = ftok("./", 'a');
    sprintf(out, "*** MAIN: qsort shared memory key  = %d\n", keyA);
    shmIDA = shmget(keyA, sizeof(int)*1024, IPC_CREAT | 0666);
    sprintf(str, "*** MAIN: qsort shared memory created\n");
    strcat(out, str);
    shmPtrA = (int*)shmat(shmIDA, NULL, 0);
    sprintf(str, "*** MAIN: qsort shared memory is attached and ready to use\n\n");
    //printf("gvcghdsvcghdsgchdsghcdgshcdghsvcghvdsgc\n");
    strcat(out, str);
    write(1, out, strlen(out));

    keyX = ftok("./", 'x');
    sprintf(out, "*** MAIN: merge x[] shared memory key  = %d\n", keyX);
    shmIDX = shmget(keyX, sizeof(int)*1024, IPC_CREAT | 0666);
    sprintf(str, "*** MAIN: merge x[] shared memory created\n");
    strcat(out, str);
    shmPtrX = (int*)shmat(shmIDX, NULL, 0);
    sprintf(str, "*** MAIN: merge x[] shared memory is attached and ready to use\n\n");
    strcat(out, str);
    write(1, out, strlen(out));

    keyY = ftok("./", 'y');
    sprintf(out, "*** MAIN: merge y[] shared memory key  = %d\n", keyY);
    shmIDY = shmget(keyY, sizeof(int)*1024, IPC_CREAT | 0666);
    sprintf(str, "*** MAIN: merge y[] shared memory created\n");
    strcat(out, str);
    shmPtrY = (int*)shmat(shmIDY, NULL, 0);
    sprintf(str, "*** MAIN: merge y[] shared memory is attached and ready to use\n\n");
    strcat(out, str);
    write(1, out, strlen(out));

    keyF = ftok("./", 'f');
    sprintf(out, "*** MAIN: merge f[] shared memory key  = %d\n", keyF);
    shmIDF = shmget(keyF, sizeof(int)*2048, IPC_CREAT | 0666);
    sprintf(str, "*** MAIN: merge f[] shared memory created\n");
    strcat(out, str);
    shmPtrF = (int*)shmat(shmIDF, NULL, 0);
    sprintf(str, "*** MAIN: merge f[] shared memory is attached and ready to use\n\n");
    strcat(out, str);
    write(1, out, strlen(out));

    while(read(0, tempstr, 1) > 0){
        strcat(strBig, tempstr);
    }

    /*READING IN ARRAY A*/
    token = strtok(strBig, " \n\t");
    tempstr = strdup(token);
    size = atoi(tempstr);
    sprintf(out, "*** MAIN: Input array for qsort has %d elements:\n\t", size); 

    for(i = 0; i < size; i++){
        token = strtok(NULL, " \n\t");
        tempstr = strdup(token);        
        temp       = atoi(tempstr);
        sprintf(str, "%d ", temp);
        strcat(out, str);
        shmPtrA[i] = temp;
    }
    strcat(out, "\n");
    write(1, out, strlen(out));

    /*READING IN ARRAY X*/
    token = strtok(NULL, " \n\t");
    tempstr = strdup(token);
    m = atoi(tempstr);
    sprintf(out, "*** MAIN: Input array for x[] has %d elements:\n\t", m); 

    for(i = 0; i < m; i++){
        token = strtok(NULL, " \n\t");
        tempstr = strdup(token);
        temp       = atoi(tempstr);
        sprintf(str, "%d ", temp);
        strcat(out, str);
        shmPtrX[i] = temp;
    }
    strcat(out, "\n");
    write(1, out, strlen(out));

     /*QSORT ARGUMENTS*/
    argsQ[0] = "./qsort";  
    sprintf(str1, "%d", 0);
    argsQ[1] =  str1;
    sprintf(str2, "%d", size-1);
    argsQ[2] = str2;
    sprintf(str3, "%d", shmIDA);
    argsQ[3] = str3;
    argsQ[4] = NULL;

    /*READING IN ARRAY Y*/
    token = strtok(NULL, " \n\t");
    tempstr = strdup(token);
    n = atoi(tempstr);
    sprintf(out, "*** MAIN: Input array for y[] has %d elements:\n\t", n); 

    for(i = 0; i < n; i++){
        token = strtok(NULL, " \n\t");
        tempstr = strdup(token);
        temp       = atoi(tempstr);
        sprintf(str, "%d ", temp);
        strcat(out, str);
        shmPtrY[i] = temp;
    }
    strcat(out, "\n");
    write(1, out, strlen(out));

    /*finds biggest*/
    biggest = shmPtrX[m-1];
    if(shmPtrY[n-1] > biggest){
        biggest = shmPtrY[n-1];
    }

    /*MERGESORT ARGUMENTS*/
    argsM[0] = "./merge";  
    sprintf(str4, "%d", shmIDX);
    argsM[1] =  str4;

    sprintf(str5, "%d", shmIDY);
    argsM[2] = str5;

    sprintf(str6, "%d", shmIDF);
    argsM[3] = str6;

    sprintf(str7, "%d", m);
    argsM[4] = str7;

    sprintf(str8, "%d", n);
    argsM[5] = str8;

    argsM[6] = NULL;
    

    childQ = fork();

    if(childQ == 0){
    	sprintf(out, "*** MAIN: about to spawn the process for qsort\n");
    	write(1, out, strlen(out));
    	execvp(argsQ[0], argsQ);
    }else {
        childM = fork();
        if(childM == 0){
            sprintf(out, "*** MAIN: about to spawn the process for merge\n");
         write(1, out, strlen(out));
            execvp(argsM[0], argsM);
        } else {
            wait(NULL);
            wait(NULL);
        }
    }

    sprintf(out, "*** MAIN: sorted array by qsort:\n\t");
    for(j = 0; j < size; j++){
        sprintf(str, "%d  ", shmPtrA[j]);
        strcat(out, str);
    }
    strcat(out, "\n");
    write(1, out, strlen(out));

    sprintf(out, "*** MAIN: merged array:\n\t");
    for(j = 0; j < (m+n); j++){
        sprintf(str, "%d  ", shmPtrF[j]);
        strcat(out, str);
    }
    strcat(out, "\n");
    write(1, out, strlen(out));

    strcat(out, "\n");
    write(1, out, strlen(out));

    shmdt((void*) shmPtrA);
    sprintf(out, "*** MAIN: qsort shared memory successfully detached\n");
    write(1, out, strlen(out));

    shmdt((void*) shmPtrX);
    sprintf(out, "*** MAIN: merge x[] shared memory successfully detached\n");
    write(1, out, strlen(out));

    shmdt((void*) shmPtrY);
    sprintf(out, "*** MAIN: merge y[] shared memory successfully detached\n");
    write(1, out, strlen(out));

    shmdt((void*) shmPtrF);
    sprintf(out, "*** MAIN: merge f[] shared memory successfully detached\n");
    write(1, out, strlen(out));

    shmctl(shmIDA, IPC_RMID, NULL);
    sprintf(out, "*** MAIN: qsort shared memory successfully removed\n");
    write(1, out, strlen(out));

    shmctl(shmIDX, IPC_RMID, NULL);
    sprintf(out, "*** MAIN: merge x[] shared memory successfully removed\n");
    write(1, out, strlen(out));

    shmctl(shmIDY, IPC_RMID, NULL);
    sprintf(out, "*** MAIN: merge y[] shared memory successfully removed\n");
    write(1, out, strlen(out));

    shmctl(shmIDF, IPC_RMID, NULL);
    sprintf(outputString, "*** MAIN: merge f[] shared memory successfully removed\n");
    write(1, outputString, strlen(outputString));

    free(str);
    free(str1);
    free(str2);
    free(str3);
    free(str4);
    free(str5);
    free(str6);
    free(str7);
    free(str8);
    free(bigStr);
    free(tempstr);

    return 1;



}