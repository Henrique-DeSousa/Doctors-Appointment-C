#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h> 
#include<sys/wait.h>

#include"Consulta.h"
#include"Tread.h"
#include"Unread.h"


Consulta cs;
Consulta lista_consultas[10];
int lost, a, b, c, count = 0;

//vacancy cheker, in case an appointment finishes.
int check(){
for(int i = 0; i < 10; i++){
	if(lista_consultas[i].type == -1)
		count = i;
		return 1;
	}
return 0;
}

//string divider
void subString(char line[], char result[], char sep, int indice){
int i, j = 0, my_indice = 0;
	for(i = 0; line[i] != '\0'; i++){
		if(line[i] == sep){
			my_indice++;
		}else if (my_indice == indice){
			result[j++]=line[i];
		}
	}
result[j]='\0';
}

//S2 registring the pid of Server
void registPID(){
int a = getpid();
FILE *fp;
if (( fp = fopen ("SrvConsultas.pid", "w")) != NULL ){
    fprintf(fp, "%d", a);
    fclose(fp);
}else{
    perror("File can't be opened!\nError: 404\n");
    exit(1);
    }
}

//S3.3 check for an open spot {} []
int vacancy(int pid){
if (check() != 1){
	printf("List is full!\n");
	lost++;
	kill(pid,SIGUSR2);
	return 1;
	}
return 0;
}

//S3.4 adding appointment to a room
void addRoom(int type, int pid){
pid_t ch_pid, wpid;
int stat = 0;
check();
lista_consultas[count] = cs;
printf("Appointment booked to room: %d\n", count);
//aux = count;
//incrementing the type of doctor's appointment
	switch(type){

		case 1:
			a++;
		break;
		case 2:
			b++;
		break;
		case 3:
			c++;
		break;
	}

int chld = fork();
if((chld) == 0){ //child process
	kill(pid,SIGHUP);
	sleep(10);
	printf("Appointment finished for room: %d\n", lista_consultas[count]);
	kill(pid,SIGTERM);
	exit(0);
}
while((wpid = wait(&stat)) > 0);
	lista_consultas[count].type = -1;
}

//treating the incoming signal
void treat(int signal){
int Max = 100;
char buff[Max];
char desc[Max];
char type[Max];
char type_used[Max];
char pid_chars[Max];
FILE *pc;

switch(signal){

	case 2:
		remove("SrvConsultas.pid");
		printf("\nServer shutting down...\n");
		FILE *Stc;
			
			Unread ur;
            Tread tr;		

		if((access( "StatsConsultas.dat", F_OK) != -1)){
			Stc=fopen("StatsConsultas.dat", "r");
			fread(&tr, sizeof(tr),1,Stc);
			fclose(Stc);

			ur.lost = tr.l + lost;
			ur.a = tr.a + a;
			ur.b = tr.b + b;
			ur.c = tr.c + c;			

			Stc=fopen("StatsConsultas.dat", "w");
            fwrite(&ur, sizeof(ur), 1, Stc);
            
			fclose(Stc);
             exit(0);
		}else{

			if((Stc=fopen("StatsConsultas.dat", "w")) != NULL){
			ur.lost = lost;
			ur.a = a;
            ur.b = b;
            ur.c = c;

				fwrite(&ur,sizeof(ur), 1, Stc);	
				
				fclose(Stc);
				exit(0);
			}else{
				perror("File can't be opened!\nError 34\n");
				exit(1);
			}
		}
	break;

	case 10:
	//S3.1
	//FILE *pc;
		 if((pc=fopen("PedidoConsulta.txt", "r")) != NULL){
			fgets(buff, 100, pc);
			fclose(pc);
		}else{
			perror("File can't be opened\nError 102\n");
			exit(1);
		}
	//S3.2
	subString(buff, desc, ' ', 0);
	subString(buff, type_used, ' ', 1);
	subString(buff, pid_chars, ' ', 2);

	int pid = atoi(pid_chars); // changing from chars to ints
	int	numb = atoi(type_used); // idem
	
	cs.pid_consult = pid;
	cs.type = numb;
	strcpy(cs.description,desc);

	if(vacancy(pid) != 1){ // if this doesn't return 1, procceds to the next bit
		
		switch(numb){

		case 1:
			strcpy(type, "Consulta Geral");
		break;
		case 2:
			strcpy(type, "COVID19");
		break;
		case 3:
			strcpy(type,"URGENT");
		break;
		default:
			strcpy(type,"Invalid type");
			exit(1);
		}
		printf("New appointment for %s booked, with the following description: %s , and PID: %d\n", type, desc, pid);
		addRoom(numb, pid);
	}
		break;
	}
}

//setting the type array to -1
void setArray(){
for(int i = 0; i < 10; i++){
	lista_consultas[i].type = -1;
//	printf("\n%d\n", lista_consultas[i].type);
	}
}


int main(){
registPID();
setArray();
//for(;;){
signal(SIGUSR1, treat);
signal(SIGINT, treat);
//}
for(;;);
//pause();
}
