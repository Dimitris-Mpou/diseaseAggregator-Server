#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "structs.h"
#include "functions.h"

int serverPort, numThreads, querySum, count;
char serverIP[32], queryFile[SOCKET_BUFFER_SIZE], **queries;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char const *argv[]) {
  int i, j, err;
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
        strcpy(queryFile, argv[i+1]);
      }else if(argv[i][0]=='-' && argv[i][1]=='s') {
        if(argv[i][2]=='p')
          serverPort = atoi(argv[i+1]);
        else if(argv[i][2]=='i' && argv[i][3]=='p')
          strcpy(serverIP, argv[i+1]);
      }else if(argv[i][0]=='-' && argv[i][1]=='w') {
        numThreads = atoi(argv[i+1]);
        if(numThreads < 1){
          printf("numWorkers should be at least 1!\n");
          return 1;
        }
      }else{
        printf("Wrong command line argument, programm exiting\n");
        return 2;
      }
    }
  }
  printf("IP: %s, Port: %d\n", serverIP, serverPort);

  count = 0;

  querySum = countQueries(queryFile);
  queries = malloc(querySum * sizeof(char *));
  for(i=0; i<querySum; i++)
    queries[i] = malloc(SOCKET_BUFFER_SIZE * sizeof(char));
  thId = malloc(numThreads*sizeof(pthread_t));
  input(queryFile, queries);

  // Creation of threads
  for(i=0; i<numThreads; i++){
    if(err = pthread_create(thId+i , NULL , threadClient , &j)) {printf("pthread_create: %d\n", err); fflush(stdout); exit(1);}
  }

  for(i=0; i<numThreads; i++)
    if(err = pthread_join(thId[i] , NULL) < 0) {perror("join"); exit(1);}

  for(i=0; i<querySum; i++)
    free(queries[i]);
  free(queries);
  free(thId);
	pthread_exit(NULL);

}

void *threadClient(void *arg){
	int i, err, sock, gotNew;
  char myQuery[SOCKET_BUFFER_SIZE], result[SOCKET_BUFFER_SIZE];
  struct sockaddr_in server;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(serverIP);
  server.sin_port = htons(serverPort);

  while(1){
    gotNew = 0;
    pthread_mutex_lock(&mtx);
    if(count < querySum){
      strcpy(myQuery, queries[count]);
      count++;
      gotNew = 1;
      pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&mtx);
    if(gotNew){
      pthread_mutex_lock(&mtx);
      while(count < querySum && count < numThreads)
        pthread_cond_wait(&cond, &mtx);             // Molis ola ta threads paroun apo mia erwtisi
      pthread_mutex_unlock(&mtx);

      if((sock = socket(AF_INET , SOCK_STREAM , 0)) < 0){perror("socket");}
      if(connect(sock, (struct sockaddr  *) &server, sizeof(server)) < 0){ perror("connect");exit(0);}
      write(sock, myQuery, SOCKET_BUFFER_SIZE);     // Send query
      read(sock, result, SOCKET_BUFFER_SIZE);       // Receive result
      printf("%s\n%s\n", myQuery, result);
      close(sock);
    }

    pthread_mutex_lock(&mtx);
    if(count >= querySum){
      pthread_mutex_unlock(&mtx);
      break;
    }
    pthread_mutex_unlock(&mtx);
  }
	pthread_exit(NULL);
}

// ./whoClient -w 5 -sip 192.168.1.3 -sp 1061 -q /home/family/Documents/dokimi/QueryFile
