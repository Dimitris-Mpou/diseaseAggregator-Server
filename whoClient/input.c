#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int countQueries(char *queryFile){
  int count;
	char ch;
	FILE *fp;

	fp = fopen(queryFile,"r");
	count = 0;
	while(1){
		ch = fgetc(fp);
		if(ch == '\n'){
				count++;
		}else if(ch == EOF){
			break;
		}
	}

  fclose(fp);
	return count;
}

void input(char *queryFile, char **queries){
  int i, count, flag;
	char ch;
	FILE *fp;

  fp = fopen(queryFile,"r");

  i = 0;
  count = 0;
	while(1){
		ch = fgetc(fp);
		if(ch == '\n'){
				i++;
        count = 0;
		}else if(ch == EOF){
			break;
		}else{
      queries[i][count] = ch;
      count++;
      queries[i][count] = '\0';
    }
	}

  fclose(fp);
}
