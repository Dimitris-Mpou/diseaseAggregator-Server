#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <netdb.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include  <sys/stat.h>
//#include <sys/select.h>
//#include  <fcntl.h>
#include "structs.h"
#include "functions.h"

int *buffer, bufferSize, start, end, isFullFlag, numWorkers, *workerPort, *myCountriesCount;
char workerIP[32], ***countries;
pthread_mutex_t mtxBuffer = PTHREAD_MUTEX_INITIALIZER, workersInfo = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condIsFull = PTHREAD_COND_INITIALIZER, condIsEmpty = PTHREAD_COND_INITIALIZER;

int main(int argc, char const *argv[]) {
  int queryPortNum, statisticsPortNum, numThreads;     // Command Line numbers
  char serverIP[32], serverPort[16];
  int i, j, err, workerSock, querySock, newSock;
  pthread_t *thId;
	struct sockaddr_in server, client;
	socklen_t clientlen;

    // Diabazw ta command line arguments
  if(argc!=9){
    printf("Wrong number of command line arguments, programm exiting\n");
    return 2;
  }else{
    for(i = 1; i < argc; i+=2){
      if (argv[i][0]=='-' && argv[i][1]=='q') {
        queryPortNum = atoi(argv[i+1]);
      }else if(argv[i][0]=='-' && argv[i][1]=='s') {
        statisticsPortNum = atoi(argv[i+1]);
      }else if(argv[i][0]=='-' && argv[i][1]=='w') {
        numThreads = atoi(argv[i+1]);
        if(numThreads < 1){
          printf("numWorkers should be at least 1!\n");
          return 1;
        }
      }else if(argv[i][0]=='-' && argv[i][1]=='b') {
        bufferSize = atoi(argv[i+1]);
        if(bufferSize < 1){
          printf("bufferSize should be at least 1!\n");
          return 1;
        }
      }else{
        printf("Wrong command line argument, programm exiting\n");
        return 2;
      }
    }
  }

  buffer = malloc(bufferSize*sizeof(int));
  for(i=0; i<bufferSize; i++)
    buffer[i] = -1;
  start = end = 0;
  isFullFlag = 0;
  numWorkers = 100;
  workerPort = NULL;
  strcpy(workerIP ,"Not yet");
  thId = malloc(numThreads*sizeof(pthread_t));
  clientlen = sizeof(client);

  // Creation of threads
  for(i=0; i<numThreads; i++){
    if(err = pthread_create(thId+i , NULL , threadServer , &j)) {printf("pthread_create: %d\n", err); fflush(stdout); exit(1);}
  }

  if((workerSock = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(statisticsPortNum);

	if(bind(workerSock, (struct sockaddr  *) &server ,sizeof(server)) < 0){perror("bind");}
	if(listen(workerSock , 897) < 0){perror("listen");}
	printf("Listening for connections\n\n");
  i = 0;
  while(i < numWorkers){
  	if((newSock = accept(workerSock, (struct sockaddr  *) &client, &clientlen)) < 0){perror("accept");}
//  	printf("Accepted\n");
    pthread_mutex_lock(&workersInfo);       // Insert at buffer
    strcpy(workerIP, inet_ntoa(client.sin_addr));
    pthread_mutex_unlock(&workersInfo);
    pthread_mutex_lock(&mtxBuffer);
    while(isFullFlag){
      pthread_cond_wait(&condIsFull, &mtxBuffer);
    }
    bufferInsert(buffer, bufferSize, &start, end, newSock, &isFullFlag);
//    printf("Inserted %d, [%d, %d], >%d\n", newSock, start, end, isFullFlag); fflush(stdout);
    pthread_mutex_unlock(&mtxBuffer);
    pthread_cond_signal(&condIsEmpty);
    if(!i)
      usleep(5000);   // Perimenei wste to thread pou exipiretei ton 1o worker na enimrwsei tin numWorkers
    i++;
  }
  close(workerSock);



  if((querySock = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
  server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(queryPortNum);  /////////
	if(bind(querySock, (struct sockaddr  *) &server ,sizeof(server)) < 0){perror("bind");}
	if(listen(querySock , 897) < 0){perror("listen");}
  printf("Listening for query connections\n"); fflush(stdout);

  while(1){
    if((newSock = accept(querySock, (struct sockaddr  *) &client, &clientlen)) < 0){perror("accept");}
//  	printf("Accepted\n");
    pthread_mutex_lock(&workersInfo);
    strcpy(workerIP, inet_ntoa(client.sin_addr));
    pthread_mutex_unlock(&workersInfo);
    // put at buffer
    pthread_mutex_lock(&mtxBuffer);
    while(isFullFlag){
      pthread_cond_wait(&condIsFull, &mtxBuffer);
    }
    bufferInsert(buffer, bufferSize, &start, end, newSock, &isFullFlag);
//    printf("Inserted %d, [%d, %d], >%d\n", newSock, start, end, isFullFlag); fflush(stdout);
    pthread_mutex_unlock(&mtxBuffer);
    pthread_cond_signal(&condIsEmpty);
  }

  free(thId);
	pthread_exit(NULL);

}

void *threadServer(void *arg){
	int i, j, z, err, sock, myWorkerPort, newSock, commandID, count, *workerSock, *seen, responses, flag;
  char message[SOCKET_BUFFER_SIZE], quer[SOCKET_BUFFER_SIZE], numPat[64], numPatFull[1024];
  struct sockaddr_in worker;
  fd_set fds;
	struct timeval timeout;
  Record recData;

  while(1){
    // Pop from buffer
    pthread_mutex_lock(&mtxBuffer);
    while(bufferIsEmpty(start, end, isFullFlag)){
      pthread_cond_wait(&condIsEmpty, &mtxBuffer);
    }
    sock = bufferPop(buffer, bufferSize, &end, &isFullFlag);
//    printf("Thread %lu got sock %d\n", pthread_self(), sock); fflush(stdout);
    pthread_mutex_unlock(&mtxBuffer);
    pthread_cond_signal(&condIsFull);

    err = read(sock, message, SOCKET_BUFFER_SIZE); if(err <= 0){perror("READ");exit(1);}  // statistics or query
    if(message[0]=='s'){
      err = read(sock, message, SOCKET_BUFFER_SIZE); if(err <= 0){perror("read");exit(1);}  // Worker port
      myWorkerPort = atoi(message);
      err = read(sock, message, SOCKET_BUFFER_SIZE); if(err <= 0){perror("read");exit(1);}  // numWorkers

      pthread_mutex_lock(&workersInfo);
      numWorkers = atoi(message);
      err = read(sock, message, SOCKET_BUFFER_SIZE); if(err <= 0){perror("read");exit(1);}  // Countries
      if(workerPort == NULL){
        workerPort = malloc(numWorkers * sizeof(int));
        myCountriesCount = malloc(numWorkers * sizeof(int));
        countries = malloc(numWorkers*sizeof(char **));
        for(i=0; i<numWorkers; i++)
          workerPort[i] = -1;
        i = 0;
        workerPort[i] = myWorkerPort;
        myCountriesCount[i] = atoi(message);
      }else{
        for(i=0; i<numWorkers; i++){
          if(workerPort[i] == -1){
            workerPort[i] = myWorkerPort;
            myCountriesCount[i] = atoi(message);
            break;
          }
        }
      }
      countries[i] = malloc(myCountriesCount[i]*sizeof(char *));
      for(j=0; j<myCountriesCount[i]; j++){
        countries[i][j] = malloc(32*sizeof(char));
      }
      err = read(sock, message, SOCKET_BUFFER_SIZE); if(err <= 0){perror("read");exit(1);}  // Countries
      j = 0;
      while(message[0]!='^'){
        strcpy(countries[i][j], message);
        err = read(sock, message, SOCKET_BUFFER_SIZE); if(err <= 0){perror("read");exit(1);}  // Countries
        j++;
      }
      pthread_mutex_unlock(&workersInfo);
//      readStats(sock);

    }else{        // query

      commandID = commandIdentifier(message, &recData);
      workerSock = malloc(numWorkers*sizeof(int));
      if(commandID == 1){                           //      /diseaseFrequency xwris country
        seen = malloc(numWorkers*sizeof(int));
        strcpy(quer, message);
        sprintf(message, "1 %s %s %s", recData.diseaseID, recData.entryDate, recData.exitDate);
        for(i=0; i<numWorkers; i++){
          if((workerSock[i] = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
        	worker.sin_family = AF_INET;
        	worker.sin_addr.s_addr = inet_addr(workerIP);
          worker.sin_port = htons(workerPort[i]);
          if(connect(workerSock[i], (struct sockaddr  *) &worker, sizeof(worker)) < 0){printf("Here\n");fflush(stdout); perror("connect");exit(0);}
          write(workerSock[i], message, SOCKET_BUFFER_SIZE);   //  Prowthei tin erwtisi stous workers
        }
        count = 0;
        responses = 0;
        for(i=0; i<numWorkers; i++)
          seen[i] = 0;
        while(responses < numWorkers){
          FD_ZERO(&fds);
          for(i=0; i<numWorkers; i++){
            if(!seen[i])
              FD_SET(workerSock[i], &fds);
          }
          timeout.tv_sec = 2;
          timeout.tv_usec = 0;
          i = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
          if(i<0)
            printf("Error in select\n");

          for(i=0; i<numWorkers; i++){
            if( FD_ISSET(workerSock[i], &fds) ){
              responses++;
              seen[i] = 1;
              read(workerSock[i], message, SOCKET_BUFFER_SIZE);
              count += atoi(message);
            }
          }
        }
        printf("%s\n%d\n\n", quer, count); fflush(stdout);     // Ektupwnei to apotelesma
        sprintf(message, "%d\n", count);
        write(sock, message, SOCKET_BUFFER_SIZE); // To stelnei kai ston client

        for(i=0; i<numWorkers; i++)
          close(workerSock[i]);
        free(seen);
      }else if(commandID == 2){                           //      /diseaseFrequency me country
        strcpy(quer, message);
        sprintf(message, "2 %s %s %s %s", recData.diseaseID, recData.entryDate, recData.exitDate, recData.country);
        flag = 0;
        for(i=0; i<numWorkers; i++){
          for(j=0; j<myCountriesCount[i]; j++){
            if(strcmp(countries[i][j], recData.country) == 0){
              if((workerSock[i] = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
            	worker.sin_family = AF_INET;
            	worker.sin_addr.s_addr = inet_addr(workerIP);
              worker.sin_port = htons(workerPort[i]);   ///
              if(connect(workerSock[i], (struct sockaddr  *) &worker, sizeof(worker)) < 0){ perror("connect");exit(0);}
              write(workerSock[i], message, SOCKET_BUFFER_SIZE);   //  Prowthei tin erwtisi stous workers
              read(workerSock[i], message, SOCKET_BUFFER_SIZE);
              printf("%s\n%s\n", quer, message); fflush(stdout);  // Ektupwnei to apotelesma
              write(sock, message, SOCKET_BUFFER_SIZE);           // To stelnei kai ston client
              close(workerSock[i]);
              flag = 1;
              break;
            }
          }
          if(flag)
            break;
        }
      }else if(commandID == 3){                     // topk-ageRanges
        strcpy(quer, message);
        sprintf(message, "3 %s %s %s %s %s", recData.recordID, recData.country, recData.diseaseID, recData.entryDate, recData.exitDate);
        flag = 0;
        for(i=0; i<numWorkers; i++){
          for(j=0; j<myCountriesCount[i]; j++){
            if(strcmp(countries[i][j], recData.country) == 0){
              if((workerSock[i] = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
            	worker.sin_family = AF_INET;
            	worker.sin_addr.s_addr = inet_addr(workerIP);
              worker.sin_port = htons(workerPort[i]);   ///
              if(connect(workerSock[i], (struct sockaddr  *) &worker, sizeof(worker)) < 0){ perror("connect");exit(0);}
              write(workerSock[i], message, SOCKET_BUFFER_SIZE);   //  Prowthei tin erwtisi ston worker
              read(workerSock[i], message, SOCKET_BUFFER_SIZE);
              printf("%s\n%s\n", quer, message); fflush(stdout);    // Ektupwnei to apotelesma
              write(sock, message, SOCKET_BUFFER_SIZE);           // To stelnei kai ston client
              close(workerSock[i]);
              flag = 1;
              break;
            }
          }
          if(flag)
            break;
        }
        if(!flag){
          printf("%s\nCan't found country %s\n\n", quer, recData.country); fflush(stdout);  // send to client
          write(sock, "Incorrect Country\n", SOCKET_BUFFER_SIZE);           // To stelnei kai ston client
        }
      }else if(commandID == 4){                           //    /searchPatientRecord
        flag = 0;
        seen = malloc(numWorkers*sizeof(int));
        strcpy(quer, message);
        sprintf(message, "4 %s", recData.recordID);
        for(i=0; i<numWorkers; i++){
          if((workerSock[i] = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
        	worker.sin_family = AF_INET;
        	worker.sin_addr.s_addr = inet_addr(workerIP);
          worker.sin_port = htons(workerPort[i]);   ///
          if(connect(workerSock[i], (struct sockaddr  *) &worker, sizeof(worker)) < 0){ perror("connect");exit(0);}
          write(workerSock[i], message, SOCKET_BUFFER_SIZE);   //  Prowthei tin erwtisi stous workers
        }
        responses = 0;
        for(i=0; i<numWorkers; i++)
          seen[i] = 0;
        while(responses < numWorkers){
          FD_ZERO(&fds);
          for(i=0; i<numWorkers; i++){
            if(!seen[i])
              FD_SET(workerSock[i], &fds);
          }
          timeout.tv_sec = 2;
          timeout.tv_usec = 0;
          i = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
          if(i<0)
            printf("Error\n");

          for(i=0; i<numWorkers; i++){
            if( FD_ISSET(workerSock[i], &fds) ){
              responses++;
              seen[i] = 1;
              err = read(workerSock[i], message, SOCKET_BUFFER_SIZE);
              if(strcmp(message, "Not Found") != 0){
                printf("%s\n%s\n\n", quer, message); fflush(stdout);    // Ektupwnei to apotelesma
                write(sock, message, SOCKET_BUFFER_SIZE);   // To stelnei kai ston client
                flag = 1;
              }
            }
          }
        }
        for(i=0; i<numWorkers; i++)
          close(workerSock[i]);
        free(seen);
        if(!flag){
          printf("%s\n%s\n", quer, message); fflush(stdout);    // Ektupwnei to apotelesma
          write(sock, message, SOCKET_BUFFER_SIZE);   // To stelnei kai ston client
        }

      }else if(commandID == 5 || commandID==7){           //      /numPatientAdmissions  kai numPatientDischarges xwris country
        seen = malloc(numWorkers*sizeof(int));
        strcpy(quer, message);
        for(i=0; i<numWorkers; i++){
          sprintf(message, "%d %s %s %s", commandID, recData.diseaseID, recData.entryDate, recData.exitDate);
          if((workerSock[i] = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
        	worker.sin_family = AF_INET;
        	worker.sin_addr.s_addr = inet_addr(workerIP);
          worker.sin_port = htons(workerPort[i]);   ///
          if(connect(workerSock[i], (struct sockaddr  *) &worker, sizeof(worker)) < 0){ perror("connect");exit(0);}
          write(workerSock[i], message, SOCKET_BUFFER_SIZE);   //  Prowthei tin erwtisi ston worker
        }
        count = 0;
        responses = 0;
        for(i=0; i<numWorkers; i++)
          seen[i] = 0;

        strcpy(numPatFull, "");
        while(responses < numWorkers){
          FD_ZERO(&fds);
          for(i=0; i<numWorkers; i++){
            if(!seen[i])
              FD_SET(workerSock[i], &fds);
          }
          timeout.tv_sec = 2;
          timeout.tv_usec = 0;
          i = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
          if(i<0)
            printf("Error\n");

          for(i=0; i<numWorkers; i++){
            if( FD_ISSET(workerSock[i], &fds) ){
              responses++;
              seen[i] = 1;
              read(workerSock[i], message, SOCKET_BUFFER_SIZE);
              j = 0;
              z = 0;
              while(message[j]!='^'){
                if(message[j]=='_'){
                  numPat[z++] = '\n';
                  numPat[z] = '\0';
                  strcat(numPatFull, numPat);
                  z = 0;
                }else{
                  numPat[z++] = message[j];
                  numPat[z] = '\0';
                }
                j++;
              }
            }
          }
        }
        printf("%s\n%s\n", quer, numPatFull); fflush(stdout); // Ektupwnei to apotelesma
        write(sock, numPatFull, SOCKET_BUFFER_SIZE);        // To stelnei kai ston client
        for(i=0; i<numWorkers; i++)
          close(workerSock[i]);
        free(seen);
      }else if(commandID == 6 || commandID == 8){         //      /numPatientAdmissions  kai numPatientDischarges me country
        strcpy(quer, message);
        sprintf(message, "%d %s %s %s %s", commandID, recData.diseaseID, recData.entryDate, recData.exitDate, recData.country);
        flag = 0;
        for(i=0; i<numWorkers; i++){
          for(j=0; j<myCountriesCount[i]; j++){
            if(strcmp(countries[i][j], recData.country) == 0){
              if((workerSock[i] = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
            	worker.sin_family = AF_INET;
            	worker.sin_addr.s_addr = inet_addr(workerIP);
              worker.sin_port = htons(workerPort[i]);   ///
              if(connect(workerSock[i], (struct sockaddr  *) &worker, sizeof(worker)) < 0){ perror("connect");exit(0);}
              write(workerSock[i], message, SOCKET_BUFFER_SIZE);   //  Prowthei tin erwtisi ston worker
              read(workerSock[i], message, SOCKET_BUFFER_SIZE);
              printf("%s\n%s\n", quer, message); fflush(stdout);  // Ektupwnei to apotelesma
              write(sock, message, SOCKET_BUFFER_SIZE);        // To stelnei kai ston client
              close(workerSock[i]);
              flag = 1;
              break;
            }
          }
          if(flag)
            break;
        }
      }else{
        printf("%s\nWrong command\n\n", message);               // Ektupwnei to apotelesma
        write(sock, "Wrong command\n", SOCKET_BUFFER_SIZE);    // To stelnei kai ston client
      }
    }
    free(workerSock);

    if(strcmp(workerIP, "Not yet")==0){
      printf("Sleeping for IP\n"); fflush(stdout);
      usleep(5000);
    }
    close(sock);
  }

	pthread_exit(NULL);
}

// ./whoServer -w 3 -q 1061 -s 1050 -b 10
