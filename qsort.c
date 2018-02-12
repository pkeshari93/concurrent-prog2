#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>



int partition(int a[], int left, int right){
	int pivot = a[right];
	int pindex = left;
	int i, temp=0;

	for (i = left; i <right ; i++){
		if (a[i] <  pivot) {
			temp = a[i];
			a[i] = a[pindex];
			a[pindex] = temp;
			pindex++;
		}
	}	
	temp = a[pindex];
	a[pindex] = a[right];
	a[right] = temp;
	return pindex;
}

int main(int argc, char * argv[]){

	int Left, Right, shmIDA, pivot, i;
	int * shmPtrA;
	char * str = malloc(sizeof(char)*1024);
	char * str1 = malloc(sizeof(char)*1024); 
	char * str2 = malloc(sizeof(char)*1024); 
	char * str3 = malloc(sizeof(char)*1024);
	char * out = malloc(sizeof(char)*1024);
	char * args[5];
	pid_t pid, ppid, lchild, rchild;
	key_t key;

	Left = atoi(argv[1]);
	Right = atoi(argv[2]);
	shmIDA = atoi(argv[3]);

	shmPtrA = (int*) shmat(atoi(argv[3]), NULL, 0);

	/*
	 * Use -1 to initialize because fork() uses 0 to identify a process as a
	 * child.
	 */


	

	if (Left<Right) {
		sprintf(out, "   ### Q-PROC(%d): entering with a[%d...%d]\n   \t", pid, Left, Right);
	    for(i = Left; i <= Right; i++){
	        sprintf(str, "%d  ", shmPtrA[i]);
	        strcat(out, str);
	    }
	    strcat(out, "\n");
	    write(1, out, strlen(out));
		// int status; /* For waitpid() only. */

		pivot = partition(shmPtrA, Left, Right);

		/*
		 * Parallize by processing the left and right partion siultaneously.
		 * Start by spawning the 'left' child.
		 */

		args[0] = "./qsort";
		sprintf(str3, "%d", shmIDA);
		args[3] = str3; 
		args[4] = NULL;

		sprintf(out, "   ### QPROC(%d): pivot element is a[%d] = %d\n", pid, pivot, shmPtrA[pivot]);
        write(1, out, strlen(out));

		lchild = fork();
		// if (lchild < 0) {
		// 	perror("fork");
		// 	exit(1);
		// }
		if (lchild == 0) {
			/*forks into child on left side of pivot*/
            sprintf(str1, "%d", Left);
            args[1] = str1;
            sprintf(str2, "%d", pivot-1);
            args[2] = str2;
            sprintf(out, "   ### QPROC(%d): section a[%d...%d] sorted\n   \t", pid, Left, Right);
    
            for(i = Left; i <= Right; i++){
                sprintf(str, "%d  ", shmPtrA[i]);
                strcat(out, str);
            }
            strcat(out, "\n");
            write(1, out, strlen(out));

            sprintf(out, "   ### QPROC(%d): exits\n", pid);
            write(1, out, strlen(out));
			execvp(args[0], args);
		} else {
			/* The parent spawns the 'right' child. */
			rchild = fork();
			// if (rchild < 0) {
			// 	perror("fork");
			// 	exit(1);
			// }
			if (rchild == 0) {
				/*forks into child on right side of pivot*/
                sprintf(str1, "%d", pivot+1);
                args[1] = str1;
                sprintf(str2, "%d", Right);
                args[2] = str2;
                sprintf(out, "   ### QPROC(%d): section a[%d...%d] sorted\n   \t", pid, Left, Right);
    
                for(i = Left; i <= Right; i++){
                    sprintf(str, "%d  ", shmPtrA[i]);
                    strcat(out, str);
                }
                strcat(out, "\n");
                write(1, out, strlen(out));

                sprintf(out, "   ### Q PROC(%d): exits\n", pid);
                write(1, out, strlen(out));				
                execvp(args[0], args);
			} else {
				/* Parent waits for children to finish. */
				wait(&lchild);
				wait(&rchild);
			}
		} 
	}
	free(out);
    free(str);
    free(str1);
    free(str2);
    free(str3);
    shmdt((void*) shmPtrA);

	exit(1);
}