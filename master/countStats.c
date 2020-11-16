#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"
#include "functions.h"

void summaryStatistics(Hashtable **table, int size, int bucketEntryCapacity, int fileCount, int fdWrite, int bufferSize, char**files, char country[32]){
  int i, j, z, k, count, ***ageCount;
  char **diseases, message[256];
  Hashtable *cur;

  count = 0;
  for(j=0; j<size; j++){
    cur = table[j];
    while(cur->next != NULL){
      i = 0;
      while(i < bucketEntryCapacity && cur->entries[i].name != NULL){
        count++;
        i++;
      }
      cur = cur->next;
    }
  }
//  printf("%s has %d diseases\n", country, count);
  diseases = malloc(count*sizeof(char *));

  ageCount = malloc(fileCount*sizeof(int **));
  for(i=0; i<fileCount; i++){
    ageCount[i] = malloc(count*sizeof(int *));
    for(j=0; j<count; j++){
      ageCount[i][j] = malloc(4*sizeof(int));
    }
  }
  for(i=0; i<fileCount; i++){
    for(j=0; j<count; j++){
      for(z=0; z<4; z++){
        ageCount[i][j][z] = 0;
      }
    }
  }

  for(i=0; i<count; i++)
    diseases[i] = malloc(32*sizeof(char));
  count = 0;
  for(j=0; j<size; j++){
    cur = table[j];
    while(cur->next != NULL){
      i = 0;
      while(i < bucketEntryCapacity && cur->entries[i].name != NULL){
        strcpy(diseases[count], cur->entries[i].name);
        count++;
        i++;
      }
      cur = cur->next;
    }
  }
  for(z=0; z<fileCount; z++){
    for(k=0; k<count; k++){
      cur = table[Hashfunction(diseases[k], size)];
      while(cur->next != NULL){
        i = 0;
        while(i < bucketEntryCapacity && cur->entries[i].name != NULL){
          if(strcmp(cur->entries[i].name, diseases[k]) == 0){
            AVLcountDatesAge(cur->entries[i].tree, files[z], ageCount[z][k]);    // Metrisi dentrou
          }
          i++;
        }
        cur = cur->next;
      }
    }
  }
//  printf("%s\n", country);
  sprintf(message, "c %s", country);
  write(fdWrite, message, SOCKET_BUFFER_SIZE);
  for(z=0; z<fileCount; z++){
//    printf("%s\n", files[z]);
    sprintf(message, "f %s", files[z]);
    write(fdWrite, message, SOCKET_BUFFER_SIZE);
    for(k=0; k<count; k++){
//      printf("%s: %d %d %d %d\n", diseases[k], ageCount[z][k][0], ageCount[z][k][1], ageCount[z][k][2], ageCount[z][k][3]);
      sprintf(message, "d %s %d %d %d %d", diseases[k], ageCount[z][k][0], ageCount[z][k][1], ageCount[z][k][2], ageCount[z][k][3]);
      write(fdWrite, message, SOCKET_BUFFER_SIZE);
    }
  }

  for(i=0; i<count; i++)
    free(diseases[i]);
  free(diseases);
}
