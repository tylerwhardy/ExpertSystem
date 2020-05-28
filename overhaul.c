#include<stdio.h>
#include<stdlib.h>
#include "clips.h"

void AddMachineFact(void *, int, int, int, int);

int main(int argc, char *argv[])
{
	struct data_collect
	{
		int machineType;
		int machineSpeed;
		int machineSerial;
		int machineStart;
//		int machineFinish;
	};
	char buffer[21];
	FILE *filetarget;
	char *field;
	int row; 
	char *filename = "overhaul.clips";
	int bufferSerial;
	int bufferType;
	int bufferSpeed;
	int bufferStart;
//	int bufferFinish;
	
	void *theEnv;
	
	char *cs;
	
	printf("Opening CSV file...\n");	
	filetarget = fopen("database.csv","r");
	printf("Initializing records...\n");
	struct data_collect records[100];

	row = 0;
	
	while(fgets(buffer,21,filetarget)){

		field = strtok(buffer,",");
		records[row].machineType = atoi(field);
		field = strtok(NULL,",");
		records[row].machineSpeed = atoi(field);
		field = strtok(NULL,",");
		records[row].machineSerial = atoi(field);
		field = strtok(NULL,",");
		records[row].machineStart = atoi(field);
//		field = strtok(NULL,",");
//		records[row].machineFinish = atoi(field);
		++row;
	}

	fclose(filetarget);
	printf("Storage complete.\n");	
	// Duplicated in CLIPS 
	cs = "(deftemplate equipment"
	" (slot serial)"
	" (slot type"
	" (default 0)"
	" (type INTEGER))"
	" (slot speed"
	" (default 0)"
	" (type INTEGER))"
	" (slot start"
	" (type INTEGER))"
	" (slot risk"
	" (default no)"
	" (allowed-symbols yes no)))";	
	
	/* CLIPS Implementation */ 
	
	theEnv = CreateEnvironment();
	
	EnvLoad (theEnv, filename);
	
	printf("Preparing to build environment\n");
	EnvBuild(theEnv,cs);
	printf("Environment built.\n");
	
	EnvReset (theEnv);
	
	
	int outputcount;
	for ( outputcount = 1; outputcount < 100; outputcount++)
	{
		//printf("Machine Serial: %i\n", records[outputcount].machineSerial);
		
		bufferSerial = records[outputcount].machineSerial;
		//printf("Machine Type: %i\n", records[outputcount].machineType);
		bufferType = records[outputcount].machineType;
		//printf("Machine Speed: %i\n", records[outputcount].machineSpeed);
		bufferSpeed = records[outputcount].machineSpeed;
		//printf("Machine Start: %i\n", records[outputcount].machineStart);
		bufferStart = records[outputcount].machineStart;
		//printf("Machine Finish: %i\n\n", records[outputcount].machineFinish);
//		bufferFinish = records[outputcount].machineFinish;
		AddMachineFact(theEnv, bufferSerial, bufferType, bufferSpeed, bufferStart);
	}

	printf("Overhaul expert system loaded \n");
	printf("This will calculate machines at risk of running out of spares assuming 4 spares of each type and speed. \n");
	printf("Outputting results...\n");	
	//EnvAgenda (theEnv, "wdisplay", NULL);
	EnvRun (theEnv, -1);
	

	
	//EnvFacts (theEnv, "wdisplay",NULL,-1,-1,-1);
	
	printf("Cleaning up...\n");
	DestroyEnvironment (theEnv);
	
	printf("Press enter to exit...\n");
	getchar();
	
	return (1);
}
void AddMachineFact(void *environment, int factSerial, int factType, int factSpeed, int factStart)
{
	void *newFact;
	void *templatePtr;
	void *theMultifield;
	DATA_OBJECT theValue;

 //IncrementGCLocks(environment);

 /*==================*/
 /* Create the fact. */
 /*==================*/

	templatePtr = EnvFindDeftemplate(environment,"equipment");
	newFact = EnvCreateFact(environment,templatePtr);
	if (newFact == NULL) return;

 /*==============================*/
 /* Set the value of the serial slot. */
 /*==============================*/

	theValue.type = INTEGER;
	theValue.value = EnvAddLong(environment,factSerial);
	EnvPutFactSlot(environment,newFact,"serial",&theValue);

 /*==============================*/
 /* Set the value of the type slot. */
 /*==============================*/

	theValue.type = INTEGER;
	theValue.value = EnvAddLong(environment,factType);
	EnvPutFactSlot(environment,newFact,"type",&theValue);

 /*==============================*/
 /* Set the value of the speed slot. */
 /*==============================*/

	theValue.type = INTEGER;
	theValue.value = EnvAddLong(environment,factSpeed);
	EnvPutFactSlot(environment,newFact,"speed",&theValue);

 /*==============================*/
 /* Set the value of the start slot. */
 /*==============================*/

	theValue.type = INTEGER;
	theValue.value = EnvAddLong(environment,factStart);
	EnvPutFactSlot(environment,newFact,"start",&theValue);

 /*==============================*/
 /* Set the value of the finish slot. */
 /*==============================*/

//	theValue.type = INTEGER;
//	theValue.value = EnvAddLong(environment,factFinish);
//	EnvPutFactSlot(environment,newFact,"finish",&theValue);

 /*==============================*/
 /* Set the value of the risk slot. */
 /*==============================*/

	theValue.type = SYMBOL;
	theValue.value = EnvAddSymbol(environment,"no");
	EnvPutFactSlot(environment,newFact,"risk",&theValue);
	

 /*=================================*/
 /* Assign default values since all */
 /* slots were not initialized. */
 /*=================================*/

	EnvAssignFactSlotDefaults(environment,newFact);
 /*==========================================================*/
 /* Enable garbage collection. Each call to IncrementGCLocks */
 /* should have a corresponding call to DecrementGCLocks. */
 /*==========================================================*/
 //EnvDecrementGCLocks(environment);
 /*==================*/
 /* Assert the fact. */
 /*==================*/

	EnvAssert(environment,newFact);
}	