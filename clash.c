#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UserInputLength 50


static void error(char*);
static void getPrompt();
static void getArguments();
static char ** createArray(unsigned int );

//----------------------------------------------------------------------

int main(int argc, char*argv[]){

	getPrompt();
	getArguments();

}

//----------------------------------------------------------------------


static char **createArray(unsigned int size){
	
	char **arguments= (char **)malloc (size *sizeof(*arguments));
	
	if (arguments==NULL)
		error("malloc");
	
	
	for (int i=0; i< size; i++){
		
		arguments[i]=(char *) calloc(UserInputLength, sizeof(arguments[i]));
		
		if (arguments[i]==NULL)
			error("calloc");
	
	}
	
	return arguments;
	

}

static void getArguments(){
	
	int arraySize = 5;
	int numberOfArguments=0;
	char c;
	
	char userInput[UserInputLength+1];
	char **arguments=createArray(arraySize);
	
	

	// Reading from Stdin --> Using maxLength 1337+2 for the valididation of the input length 
	if (fgets(userInput, UserInputLength+2, stdin) == NULL){
		error("fegts");
	}
		
	
	// Input has a size greater than 1337 then the input is too long --> Ignoring the following character
	if (strlen(userInput)> UserInputLength){
		
		while((c=getchar()) != '\n' && c!=EOF){
			continue;
		}
		userInput[UserInputLength]='\0';
		
	}else{
		userInput[strlen(userInput)-1]='\0';
	}
	
	
 
	// Spiliting up the input 
	
	arguments[numberOfArguments]=strtok(userInput, " \t");
	numberOfArguments++;
	
	
	while((arguments[numberOfArguments++]=strtok(NULL, " \t"))!= NULL){

		// Array has a small size --> realloc
		if(numberOfArguments== arraySize ){
			arraySize=arraySize *2;
			arguments=(char **) realloc(arguments,arraySize*sizeof(*arguments));
			if (arguments==NULL)
				error("realloc");
			
			for (int i= numberOfArguments+1; i< arraySize ; i++){
				arguments[i]=calloc(200,sizeof(arguments[i]));
				if (arguments[i]==NULL)
					error("calloc");
			
			}	
		
		}

	}
	
	numberOfArguments--; 
	
	for(int i =0; i==numberOfArguments; i++)
		printf("%s\n",arguments[i]);
	
	
	for (int j=0; j==numberOfArguments; j++){
		free(arguments[j]);
	}
	
	free(arguments);
	
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


