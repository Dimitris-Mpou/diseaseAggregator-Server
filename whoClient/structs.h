#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct Records{                          // Domi eggrafwn
  char recordID[32];
  char patientFirstName[32];
  char patientLastName[32];
  char diseaseID[32];
  char country[32];
  char entryDate[11];
  char exitDate[11];
  int age;
}Record;

#endif
