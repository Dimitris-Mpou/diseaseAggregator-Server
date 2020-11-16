#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

void countInput(char patientRecordsFile[256], int *inputSize){
	int count;
	char ch;
	FILE *fp;

	fp = fopen(patientRecordsFile,"r");
	count = 0;
	while(1){
		ch = fgetc(fp);
		if(ch == '\n'){
				count++;
		}else if(ch == EOF){
			break;
		}
	}

	*inputSize = count;
	fclose(fp);
}

int compareDates(char Date1[11], char Date2[11]){
	int i;
	char temp1[5], temp2[5];

	if (Date2[0] == '-') {	// if date2 == - return 1
		return 1;
	}
	if(Date1[0] == '-')			// if date2 == - return 1
		return 2;
	// Checking the format
	for(i=0; i<2; i++){
		if (Date1[i]<'0'|| Date1[i]>'9' || Date2[i]<'0' || Date2[i]>'9') {
			printf("Wrong date format programm exiting\n");
			return -1;
		}
	}
	if (Date1[i] != '-' || Date2[i] != '-') {
		printf("Wrong date format programm exiting\n");
		return -1;
	}
	for(i=3; i<5; i++){
		if (Date1[i]<'0'|| Date1[i]>'9' || Date2[i]<'0' || Date2[i]>'9') {
			printf("Wrong date format programm exiting\n");
			return -1;
		}
	}
	if (Date1[i] != '-' || Date2[i] != '-') {
		printf("Wrong date format programm exiting\n");
		return -1;
	}
	for(i=6; i<10; i++){
		if (Date1[i]<'0'|| Date1[i]>'9' || Date2[i]<'0' || Date2[i]>'9') {
			printf("Wrong date format programm exiting\n");
			return -1;
		}
	}

	for(i = 6; i < 10; i++) {
		temp1[i-6] = Date1[i];
		temp2[i-6] = Date2[i];
	}
	temp1[4] = '\0';
	temp2[4] = '\0';
	if(atoi(temp1) < atoi(temp2)){
		return 1;
	}else if(atoi(temp1) > atoi(temp2)){
		return 2;
	}else{
		temp1[0] = Date1[3];
		temp1[1] = Date1[4];
		temp1[2] = '\0';
		temp2[0] = Date2[3];
		temp2[1] = Date2[4];
		temp2[2] = '\0';
		if(atoi(temp1) < atoi(temp2)){
			return 1;
		}else if (atoi(temp1) > atoi(temp2)){
			return 2;
		}else{
			temp1[0] = Date1[0];
			temp1[1] = Date1[1];
			temp1[2] = '\0';
			temp2[0] = Date2[0];
			temp2[1] = Date2[1];
			temp2[2] = '\0';
			if(atoi(temp1) < atoi(temp2)){
				return 1;
			}else if(atoi(temp1) > atoi(temp2)){
				return 2;
			}else{
				return 0;
			}
		}
	}
}

int input(char patientRecordsFile[256], dataHT **data, int size, char date[11]){
	int i, count, flag, bool;
	char ch, temp[3];
	Record temp_record;
	FILE *fp;

	fp = fopen(patientRecordsFile,"r");
	count = 1;

	while(1){
		ch = fgetc(fp); // Diabazw to id
    if(ch == EOF){
			break;
		}
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.recordID[i] = ch;
      i++;
      temp_record.recordID[i] = '\0';
      ch = fgetc(fp);
    }
		if(flag){continue;}

		ch = fgetc(fp); // Diabazw to ENTER / EXIT
		ch = fgetc(fp);
		if(ch=='N')
			bool = 1;
		else
		 bool = 0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      ch = fgetc(fp);
    }
		if(flag){continue;}

    ch = fgetc(fp); // Diabazw to patientFirstName
		if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.patientFirstName[i] = ch;
      i++;
      temp_record.patientFirstName[i] = '\0';
      ch = fgetc(fp);
			if(ch>='A' && ch <= 'Z'){ch += ('a'-'A');}	// lowerCase
    }
		if(flag){continue;}
    ch = fgetc(fp); // Diabazw to patientLastName
		if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.patientLastName[i] = ch;
      i++;
      temp_record.patientLastName[i] = '\0';
      ch = fgetc(fp);
			if(ch>='A' && ch <= 'Z'){ch += ('a'-'A');}	// lowerCase
    }
		if(flag){continue;}
    ch = fgetc(fp); // Diabazw to diseaseID
		if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.diseaseID[i] = ch;
      i++;
      temp_record.diseaseID[i] = '\0';
      ch = fgetc(fp);
			if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    }
		if(flag){continue;}

		ch = fgetc(fp); // Diabazw to age
    i=0;
    while(ch != ' ' && ch!='\n' && ch!=EOF){
      temp[i] = ch;
      i++;
      temp[i] = '\0';
      ch = fgetc(fp);
    }
		temp_record.age = atoi(temp);

		if(ch != '\n' && ch != EOF){
			while(ch != '\n' && ch != EOF){		// Agnow pithana lathos stoixeia (pianei periptwseis eggrafwn me perissotera i ligotera pedia)
				ch = fgetc(fp);
			}
		}else if(ch == '\n'){
//			printf("Calling insert for %s\n", temp_record.recordID);
			if(dataHashtableInsert(data, size, temp_record, bool, date, NULL)){ 			// simainei oti uparxei idi
				return 2;
			}
      count++;
		}else if(ch == EOF){
			if(dataHashtableInsert(data, size, temp_record, bool, date, NULL)){
				return 2;
			}
      count++;
			break;
		}else{		// Unexpected error
      return 1;
		}
	}
	fclose(fp);

  return 0;
}

int update(char patientRecordsFile[256], dataHT **data, int size, Hashtable **disease, int diseaseHashtableNumOfEntries, int bucketEntryCapacity, char date[11]){
	int i, count, flag, bool;
	char ch, temp[3];
	dataHT *cur;
	Record temp_record, *recAddress;
	FILE *fp;

	fp = fopen(patientRecordsFile,"r");
	count = 1;

	while(1){
		ch = fgetc(fp); // Diabazw to id
    if(ch == EOF){
			break;
		}
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.recordID[i] = ch;
      i++;
      temp_record.recordID[i] = '\0';
      ch = fgetc(fp);
    }
		if(flag){continue;}

		flag = 0;
		ch = fgetc(fp); // Diabazw to ENTER / EXIT
		ch = fgetc(fp);
		if(ch=='N'){	//	ENTER
			bool = 1;
			cur = data[Hashfunction(temp_record.recordID, size)];
			while(cur->next != NULL){
		    if(strcmp(temp_record.recordID, cur->record.recordID) == 0){
					flag = 1;		// An uparxei idi i eggrafi
				}
				cur = cur->next;
			}
		}else{				// 	EXIT
		 bool = 0;
	 }
	 if(flag){
		 ch = fgetc(fp);
		 while(ch!='\n' && ch!=EOF){
			 ch = fgetc(fp);
		 }
		 if(ch==EOF)
		 	return 0;
		 continue;
	 }	// paw stin epomeni

		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      ch = fgetc(fp);
    }
		if(flag){continue;}

    ch = fgetc(fp); // Diabazw to patientFirstName
		if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.patientFirstName[i] = ch;
      i++;
      temp_record.patientFirstName[i] = '\0';
      ch = fgetc(fp);
			if(ch>='A' && ch <= 'Z'){ch += ('a'-'A');}	// lowerCase
    }
		if(flag){continue;}
    ch = fgetc(fp); // Diabazw to patientLastName
		if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.patientLastName[i] = ch;
      i++;
      temp_record.patientLastName[i] = '\0';
      ch = fgetc(fp);
			if(ch>='A' && ch <= 'Z'){ch += ('a'-'A');}	// lowerCase
    }
		if(flag){continue;}
    ch = fgetc(fp); // Diabazw to diseaseID
		if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    i=0;
		flag = 0;
    while(ch != ' '){
			if(ch == '\n'){	// An leipei to trexwn i to epomeno pedio
//				printf("An wrong record was ignored!\n");
				flag = 1;
				break;
			}
      temp_record.diseaseID[i] = ch;
      i++;
      temp_record.diseaseID[i] = '\0';
      ch = fgetc(fp);
			if(ch>='a' && ch <= 'z'){ch -= ('a'-'A');}	// upperCase
    }
		if(flag){continue;}

		ch = fgetc(fp); // Diabazw to age
    i=0;
    while(ch != ' ' && ch!='\n' && ch!=EOF){
      temp[i] = ch;
      i++;
      temp[i] = '\0';
      ch = fgetc(fp);
    }
		temp_record.age = atoi(temp);

		if(ch != '\n' && ch != EOF){
			while(ch != '\n' && ch != EOF){		// Agnow pithana lathos stoixeia (pianei periptwseis eggrafwn me perissotera i ligotera pedia)
				ch = fgetc(fp);
			}
		}else if(ch == '\n'){
//			printf("Calling insert for %s\n", temp_record.recordID);
			if(dataHashtableInsert(data, size, temp_record, bool, date, &recAddress)){ 			// simainei oti uparxei idi
				return 2;
			}
			if(bool){
				HashtableInsert(disease, diseaseHashtableNumOfEntries, bucketEntryCapacity, recAddress, recAddress->diseaseID, 'd');
			}
      count++;
		}else if(ch == EOF){
			if(dataHashtableInsert(data, size, temp_record, bool, date, &recAddress)){
				return 2;
			}
			if(bool){
				HashtableInsert(disease, diseaseHashtableNumOfEntries, bucketEntryCapacity, recAddress, recAddress->diseaseID, 'd');
			}
      count++;
			break;
		}else{		// Unexpected error
      return 1;
		}
	}
	fclose(fp);

  return 0;
}
