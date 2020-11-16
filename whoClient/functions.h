#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void *threadClient(void *);
int countQueries(char *);
void input(char *, char **);

#endif

#ifndef SOCKET_BUFFER_SIZE
#define SOCKET_BUFFER_SIZE 1024
#endif
