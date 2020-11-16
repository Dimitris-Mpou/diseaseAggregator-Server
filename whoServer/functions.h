#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int compareDates(char[11], char[11]);
int commandIdentifier(char[256], Record *);
void readStats(int);
void *threadServer(void *);
int bufferIsEmpty(int, int, int);
void bufferInsert(int *, int, int *, int, int, int *);
int bufferPop(int *, int, int *, int *);

#endif

#ifndef SOCKET_BUFFER_SIZE
#define SOCKET_BUFFER_SIZE 1024
#endif
