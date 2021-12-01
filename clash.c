#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UserInputLength 1337
#define NumberOfArguments 3


static void error(char*);
static void getPrompt();
static void get_Arguments();

//----------------------------------------------------------------------

int main(int argc, char*argv[]){

	getPrompt();
	get_Arguments();

}

//----------------------------------------------------------------------



static void get_Arguments(){
	
	char userInput[UserInputLength+1];
	char numberOfArguments = NumberOfArguments;
	
	if (fgets(userInput, UserInputLength, stdin) == NULL)
		error("fegts");
	
	
	// Damit nicht beim Schreiben eine Zeile zu viel entsteht
	if (userInput[strlen(userInput)-1]== 10)
		strtok(userInput, "\n"); 

	int i=0; 
	char **argv= (char **) malloc(numberOfArguments*sizeof(argv));
	
	argv[0]=strtok(userInput, " \t");
	
	while((argv[i++]=strtok(NULL, " \t"))!= NULL){
		
		if(i< NumberOfArguments-1 ){
			printf("Zu klein\n");
			numberOfArguments=numberOfArguments *2;
			argv= realloc(argv,numberOfArguments);
			if (argv==NULL)
				error("realloc");
			
			for (int j= i+1; j< NumberOfArguments ; j++){
				argv[j]=calloc(200,sizeof(argv[j]));
				if (argv[j]==NULL)
					error("calloc");
			
			}
		
		}

	
	}
	
	for (int j=0; j<i-1; j++){
		free(argv[j]);
	}
	
	free(argv);
	
}


static void getPrompt(){
	
	unsigned int dirLength=5;
    char *dir=NULL;
   
 
    while(1){
		
		char *tmp=getcwd(dir,dirLength);
		
		if ((tmp==NULL) && errno == ERANGE){
	
				dirLength= dirLength*2;
				dir = realloc(dir,dirLength);
				if (dir ==NULL)
					error("realloc");
				
		}else if (tmp==NULL && errno != ERANGE){
			error("getcwd");
		}else{
			break;
		}
		
	}
    

	printf("%s: ", dir);
	fflush(stdout);
	free(dir);
}

static void error(char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}


