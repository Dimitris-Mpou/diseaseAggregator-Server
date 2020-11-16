#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_BUFFER_SIZE 256

void readStats(int fdRead){
  int i, j, age[4];
  char message[256], country[32], disease[32], date[11], tempCount[6];

  read(fdRead, message, SOCKET_BUFFER_SIZE);  //readProtocol(fdRead, message, bufferSize);
//  printf("\t\t\tC or E %s\n", message);
  while(message[0]=='c'){
    i = 2;
    j = 0;
    while(message[i]!='\0' && message[i]!=' '){       // Diabazw tin country
      country[j] = message[i];
      i++;
      j++;
    }
    country[j] = '\0';
    read(fdRead, message, SOCKET_BUFFER_SIZE);  //readProtocol(fdRead, message, bufferSize);
//    printf("\t\t\tF %s\n", message);
    while(message[0]=='f'){
      i = 2;
      j = 0;
      while(message[i]!='\0' && message[i]!=' '){       // Diabazw tin date
        date[j] = message[i];
        i++;
        j++;
      }
      date[j] = '\0';
      printf("%s\n%s\n", date, country);
      read(fdRead, message, SOCKET_BUFFER_SIZE);  // readProtocol(fdRead, message, bufferSize);
      if(strcmp(country, "USA")==0){
        printf("\t\t\tI got %s\n", message);
      }
      while(message[0]=='d'){
        i = 2;
        j = 0;
        while(message[i]!=' '){       // Diabazw to disease
          disease[j] = message[i];
          i++;
          j++;
        }
        disease[j] = '\0';
        i++;
        j = 0;
        while(message[i]!=' '){       // Age[0]
          tempCount[j] = message[i];
          i++;
          j++;
        }
        tempCount[j] = '\0';
        age[0] = atoi(tempCount);
        i++;
        j = 0;
        while(message[i]!=' '){       // Age[1]
          tempCount[j] = message[i];
          i++;
          j++;
        }
        tempCount[j] = '\0';
        age[1] = atoi(tempCount);
        i++;
        j = 0;
        while(message[i]!=' '){       // Age[2]
          tempCount[j] = message[i];
          i++;
          j++;
        }
        tempCount[j] = '\0';
        age[2] = atoi(tempCount);
        i++;
        j = 0;
        while(message[i]!='\0' && message[i]!=' '){       // Age[3]
          tempCount[j] = message[i];
          i++;
          j++;
        }
        tempCount[j] = '\0';
        age[3] = atoi(tempCount);
        i++;
        j = 0;
        if(age[0] || age[1] || age[2] || age[3]){
          printf("%s\nAge range 0-20: %d cases\nAge range 21-40: %d cases\nAge range 41-60: %d cases\nAge range 60+: %d cases\n\n", disease, age[0], age[1], age[2], age[3]); fflush(stdout);
        }
        if(strcmp(date, "22-05-2020")==0 && strcmp(country, "USA")==0){
          printf("I send %s\n\n", message);
          exit(1);
        }

        read(fdRead, message, SOCKET_BUFFER_SIZE);
      }
    }
  }

}
