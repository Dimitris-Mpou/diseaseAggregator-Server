#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include  <sys/stat.h>
#include <sys/select.h>
#include  <fcntl.h>
#include <signal.h>
#include "structs.h"
#include "functions.h"

#define bucketEntryCapacity  4
#define diseaseHashtableNumOfEntries 3

int end = 0;
int pid = -1;

int main(int argc, char const *argv[]) {
  int numWorkers, bufferSize;  // Command Line numbers
  int i, j, z, *workerWrite, *workerRead, commandID, success, fail;
  pid_t *idChilds;
  char inputFile[254], command[256], message[256], name[15], serverIP[32], serverPort[16];
  char StrNumWorkers[6], StrBufferSize[8], StrId[6];      // Gia execlp
  DIR *data;
  struct dirent *direct;
  Record recData;
  FILE *fd;

    // Diabazw ta command line arguments
  if (argc!=11) {
    printf("Wrong number of command line arguments, programm exiting\n");
    return 2;
  } else {
    for(i = 1; i < argc; i+=2){
      if (argv[i][0]=='-' && argv[i][1]=='i') {
        strcpy(inputFile, argv[i+1]);
      }else if(argv[i][0]=='-' && argv[i][1]=='w') {
        numWorkers = atoi(argv[i+1]);
        if(numWorkers < 1){
          printf("numWorkers should be at least 1!\n");
          return 1;
        }
      }else if(argv[i][0]=='-' && argv[i][1]=='b') {
        bufferSize = atoi(argv[i+1]);
        if(bufferSize < 3){
          printf("bufferSize should be at least 3!\n");
          return 1;
        }
      }else if(argv[i][0]=='-' && argv[i][1]=='s') {
        strcpy(serverIP, argv[i+1]);
      }else if(argv[i][0]=='-' && argv[i][1]=='p') {
        strcpy(serverPort, argv[i+1]);
      }else{
        printf("Wrong command line argument, programm exiting\n");
        return 2;
      }
    }
  }

  sprintf(StrNumWorkers, "%d", numWorkers);
  sprintf(StrBufferSize, "%d", bufferSize);

    // Signals kai mkfofos
  signal(SIGINT, handlerIntQuit);
  signal(SIGQUIT, handlerIntQuit);
  signal(SIGCHLD, handlerChld);
  for (i=0; i<numWorkers; i++){
    sprintf(name, "workerWrite%d", i);
    mkfifo(name, 0666);
    sprintf(name, "workerRead%d", i);
    mkfifo(name, 0666);
  }
  idChilds = malloc(numWorkers * sizeof(pid_t));
  for(i=0; i<numWorkers; i++){          // Dimiourgia numWorkers workers
    if( !(idChilds[i] = fork()) ){ break; }
  }

  if(i == numWorkers){    /////  Parent
    int countryCount, *countryWorker, responses, *seen, count;
    char **countries;
    fd_set fds;
	  struct timeval timeout;

    success = 0;
    fail = 0;
    seen = malloc(numWorkers*sizeof(int));
    workerWrite = malloc(numWorkers*sizeof(int));
    workerRead = malloc(numWorkers*sizeof(int));

    if( (data = opendir(inputFile)) == NULL){printf ("Cannot open directory '%s'\n", inputFile);} // Metrima xwrwn
    countryCount = 0;
    while ((direct = readdir(data)) != NULL) {
      if(strcmp(direct->d_name, ".")==0 || strcmp(direct->d_name, "..")==0 )
        continue;
      countryCount++;
    }

    for (i=0; i<numWorkers; i++){         // Anoigma pipes
      sprintf(name, "workerWrite%d", i);
      workerWrite[i] = open(name,  O_WRONLY);
      if(workerWrite[i]<0){
        perror("Opening write pipe\n");
      }
      sprintf(name, "workerRead%d", i);
      workerRead[i] = open(name,  O_RDONLY);
      if(workerRead[i]<0){
        perror("Opening read pipe\n");
      }
    }

      // Stelnei stous workers ta IP kai Port tou server
    for(i=0; i<numWorkers; i++){
      writeProtocol(workerWrite[i], serverIP, bufferSize);
      writeProtocol(workerWrite[i], serverPort , bufferSize);
    }
      // Diabasma xwrwn kai antistoixisi stous workers
    countries = malloc(countryCount * sizeof(char *));
    for(i=0; i<countryCount; i++){
      countries[i] = malloc(32 * sizeof(char));
    }
    closedir(data);
    if( (data = opendir(inputFile)) == NULL){printf ("Cannot open directory '%s'\n", inputFile);}

    i = 0;
    while ((direct = readdir(data)) != NULL) {
      if(strcmp(direct->d_name, ".")==0 || strcmp(direct->d_name, "..")==0 )
        continue;
      strcpy(countries[i], direct->d_name);
      i++;
    }
    closedir(data);
    countryWorker = malloc(countryCount * sizeof(int));
    j = 0;
    for(i=0; i<countryCount; i++){
      countryWorker[i] = j;
      if(j==numWorkers-1)
        j = 0;
      else
        j++;
    }
    for(j=0; j<numWorkers; j++){
      for(i=0; i<countryCount; i++){
        if(countryWorker[i]==j){
          writeProtocol(workerWrite[j], countries[i] , bufferSize);           // Stelnei ta onomata twn xwrwn stous workers
        }
      }
    }

    printf("master's pid : %d\n\n", getpid());

    //    Anamoni gia anastasi paidiwn
    while(1){
//      printf("Waiting for kid to die or signal to terminate\n");
      sleep(1);
      if(end){                                         // An irthe SIGINT or SIGQUIT, exiting
        sprintf(message, "log_file.%d", getpid());    // Create logfile
        fd = fopen(message, "w");
        for(i=0; i<countryCount; i++){
          fprintf(fd,"%s\n", countries[i]);
        }
        fprintf(fd, "TOTAL %d\nSUCCESS %d\nFAIL %d\n", success+fail, success, fail);
        fclose(fd);
        pid = -2; // Gia na min diiourgisei alla stis theseis tous
        for(i=0; i<numWorkers; i++){
          kill(idChilds[i], SIGKILL);
        }
        break;
      }
      if(pid > 0){
        printf("Worker %d ended unexpected!\n", pid);
        for(i=0; i<numWorkers; i++){
          kill(idChilds[i], SIGKILL);
        }
        exit(1);
        for(i=0; i<numWorkers; i++){
  				if(pid == idChilds[i]){
  					if( !(idChilds[i] = fork()) ){
              sprintf(StrId, "%d", i);
  						execlp("./worker", inputFile, StrNumWorkers, StrBufferSize, StrId, "n", NULL);
  					}
            writeProtocol(workerWrite[i], serverIP, bufferSize);
            writeProtocol(workerWrite[i], serverPort , bufferSize);
            for(j=0; j<countryCount; j++){
              if(countryWorker[j]==i){
                close(workerWrite[i]);
                close(workerRead[i]);
                sprintf(name, "workerWrite%d", i);
                workerWrite[i] = open(name, O_WRONLY);
                sprintf(name, "workerRead%d", i);
                workerRead[i] = open(name, O_RDONLY);
                writeProtocol(workerWrite[i], countries[j] , bufferSize);
              }
            }
  				}
        }
        pid = -1;
      }
    } // while

    for(i=0; i<numWorkers; i++){
      close(workerWrite[i]);
      close(workerRead[i]);
      sprintf(name, "workerWrite%d", i);
      unlink(name);
      sprintf(name, "workerRead%d", i);
      unlink(name);
    }

    return 0;

  }else{                  // Workers
    sprintf(StrId, "%d", i);
    execlp("./worker", inputFile, StrNumWorkers, StrBufferSize, StrId, "k", NULL);
  }
}

void handlerIntQuit(int signum){
  if(signum == 2)
    signal(SIGINT, handlerIntQuit);
  else
    signal(SIGQUIT, handlerIntQuit);

  end = 1;
}

void handlerChld(int signum){
	signal(SIGCHLD, handlerChld);

	int i;

	if(pid != -2){
		pid = wait (&i);
	}
}

// ./master -w 3 -p 1050 -s 192.168.1.4 -b 50 -i /home/family/Documents/dokimi/data
