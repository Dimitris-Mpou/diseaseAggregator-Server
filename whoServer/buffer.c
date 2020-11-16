#include "structs.h"
#include "functions.h"

int bufferIsEmpty(int start, int end, int isFullFlag){
  if(start == end && !isFullFlag)
    return 1;
  else
    return 0;
}

void bufferInsert(int *buffer, int bufferSize, int *start, int end, int newSock, int *isFullFlag){
  buffer[(*start)++] = newSock;
  if((*start) == bufferSize)
    (*start) = 0;
  if((*start) == end)
    *isFullFlag = 1;
}

int bufferPop(int *buffer, int bufferSize, int *end, int *isFullFlag){
  int sock;

  sock = buffer[(*end)++];
  if((*end) == bufferSize)
  (*end) = 0;
  *isFullFlag = 0;

  return sock;
}
