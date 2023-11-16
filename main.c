#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<time.h>
#include<signal.h>
#include<sys/select.h>

#define total_childProcesses 5
#define duration_childProrcess 30
#define READ 0
#define WRITE 1

struct timeval start;

double startTime;

void forkChild(int childNo, int pipe);

int main(int argc, char **argv) {

	gettimeofday(&start,NULL);
	startTime = (double)start.tv_sec + (double)start.tv_usec/1000000;

	int childNo = 1;
	int pipeFD[total_childProcesses][2];	//Pipe has 2 file descripter, 0 - read & 1 - write
	
	fd_set rdfs;
	FD_ZERO(&rdfs);
	int maxFD = 0;
	int isChild = 0;
	int childID = -1;

	//Create pipes
	for(int i=0;i < total_childProcesses;i++) {
		pipe(pipeFD[i]);
	}
	maxFD = pipeFD[total_childProcesses-1][READ] + 1;
	
	// Parent process creates 5 child
	while(childNo <= total_childProcesses) {
		//printf("Main Process :: childNo %d\n",childNo);

		//Forking child
		int pid = fork();
		if(pid == 0) {
			isChild = 1;
			break;
		}
		if(childNo == total_childProcesses) {
			childID = pid;
		}

		childNo++;
	}
	if(isChild) {
		//Closed unused pipe FileDescriptor
		for(int i=0;i<total_childProcesses;i++) {
			if(i != childNo - 1) {
				close(pipeFD[i][WRITE]);
			}
			close(pipeFD[i][READ]);
		}
		forkChild(childNo,pipeFD[childNo-1][WRITE]);	//Passing write fd of pipe
		return 0;
	}else{

    }
}




void forkChild(int child_no, int pipe_fd) {
    int random_wait_time = 0; // 0, 1, or 2 seconds
    int n;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    /* Initializes random number generator */
    time_t seed;
    srand((unsigned)time(&seed));

    double currentTime = (double)current_time.tv_sec + (double)current_time.tv_usec / 1000000;
    int messageNo = 1;

    if (child_no == total_childProcesses) {
        fd_set read_fds;
        FD_ZERO(&read_fds);

        struct timeval timeout;

        printf("Child 5 >> ");
        while ((currentTime - startTime) < duration_childProrcess) {
            char message[255];
            fflush(stdout);
            FD_ZERO(&read_fds);
            FD_SET(0, &read_fds);
            timeout.tv_sec = 0;
            timeout.tv_usec = 1;

            if (select(1, &read_fds, NULL, NULL, &timeout) > 0) {
                n = read(0, message, 255);
                message[n - 1] = '\0'; // remove the newline and set the ending character
                char bufferedMessage[255];
                sprintf(bufferedMessage, "0:%.3f: Child %d %s\n", (currentTime - startTime), child_no, message);
                write(pipe_fd, bufferedMessage, sizeof(char) * strlen(bufferedMessage) + 1);
                printf("Child 5 >> ");
            }
            gettimeofday(&current_time, NULL);
            currentTime = (double)current_time.tv_sec + (double)current_time.tv_usec / 1000000;
        }
        printf("\n");
    } else {
        while ((currentTime - startTime) < duration_childProrcess) {
            /* Initializes random number generator */
            random_wait_time = (rand() % 4);

            char message[255];
            sprintf(message, "0:%.3f: Child %d message %d\n", (currentTime - startTime), child_no, messageNo);
            write(pipe_fd, message, sizeof(char) * strlen(message) + 1);
            messageNo++;

            sleep(random_wait_time);
            gettimeofday(&current_time, NULL);
            currentTime = (double)current_time.tv_sec + (double)current_time.tv_usec / 1000000;
        }
    }
    close(pipe_fd);
}