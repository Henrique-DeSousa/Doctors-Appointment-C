#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<sys/wait.h>
#include<ctype.h>
#include "Consulta.h"

Consulta c;
int pidSC;
int isIn = 0;

//C2 Writing the file if it doesn't exist
void fileWrt(){
FILE *fp;
    if ((fp = fopen ("PedidoConsulta.txt", "w")) != NULL){
        fprintf(fp, "%s %d %d", c.description, c.type, c.pid_consult);
        fclose(fp);
        kill(pidSC,SIGUSR1);
     }else{
        perror("File can't be opened!\n");
        exit(1);
     }
}

int acss(){ //simpler function for access
if((access("PedidoConsulta.txt", R_OK) == 0)){
	return 1;
	}
return 0;
}

//checking the state of the file
void fileCheck(){
if(acss() == 1){
        printf("There's already an appointment ongoing. Please wait!\n");
}
 while(acss() == 1){
    sleep(10);
 }
fileWrt();
}

//C3
void readPid(){
char r[100];
FILE *sc;
if((sc=fopen("SrvConsultas.pid" , "r")) != NULL){
	fgets(r, 100, sc);
	pidSC = atoi(r);
	fclose(sc);
}else{
    perror("File can't be opened\n");
    exit(1);
    }

printf("Caught PID: %d\n", pidSC);
}


//Treating the several signals we can get
void signalTreat(int signal){
int isIn;
	
	switch(signal){
	//C4
	case 1:
		printf("Appointment iniciated for the process: %d\n", c.pid_consult);
		isIn = 1;
		break;
	//C7
	case 2:
		printf("\nCanceling on user request!\n");
		remove("PedidoConsulta.txt");
		exit(0);
        break;
	//C5
	case 15:

		switch(isIn){
			case 1:
				printf("Appointment concluded for the process: %d\n", c.pid_consult);;
				remove("PedidoConsulta.txt");
				isIn = 0;
				exit(0);
			break;

			case 0:
				perror("Appointment not possible!\nReason: Hasn't been started!\n");
				remove("PedidoConsulta.txt");
				exit(1);
			break;
		}
	//C6
	case 12:
		printf("Appointment not possible for the process: %d!\n", c.pid_consult);
		remove("PedidoConsulta.txt");
		exit(0);
	break;
	}
}

//C4
void signalReceive(){
    signal(SIGHUP, signalTreat);
    signal(SIGTERM, signalTreat);
    signal(SIGUSR2, signalTreat);
}

//C1
void tipoConsulta(){
c.pid_consult;// PID do processo que quer fazer a consulta

//choosing type of appointment
printf("Select the type of your visit: \n1- Clinica Geral\n2- COVID19\n3- Urgente\n");
scanf("%d", &c.type);
if(c.type > 3 || c.type <= 0){ // if value higher than what we provide, breaks
	perror("Invalid selection!\n");
	exit(1);
}
//read the description 
printf("Describe your symptoms: ");
scanf("%s", &c.description);
for(int i = 0, s = strlen(c.description); i < s; i++){
	if(!isalpha(c.description[i])){
		perror("Please describe your symptoms!\n");
		exit(1);
	}
}
c.pid_consult = getpid();
printf("Booked an appointment of type: %d, with the following symptoms: %s.\n", c.type, c.description);
}


int main(void){
tipoConsulta();
readPid();
fileCheck();
signalReceive();
signal(SIGINT, signalTreat);
while(1){
	pause();
	}
}
