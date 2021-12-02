#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UserInputLength 1337 
#define ArgumentLength 80
#define MaxArraySize (UserInputLength/ArgumentLength)
  
static void error(char*);
static void getPrompt();
static void getArguments(char [MaxArraySize][ArgumentLength], int *);

//----------------------------------------------------------------------

int main(int argc, char*argv[]){
	
	char args[MaxArraySize][ArgumentLength];

	
	int numberArgs;
	

		getPrompt();
		getArguments(args,&numberArgs);
		
		
		
		for(int i =0; i <= numberArgs; i++)
			printf("%s\n",args[i]);
	
	
}

//----------------------------------------------------------------------



static void getArguments(char args[MaxArraySize][ArgumentLength], int *number){
	

	int numberOfArguments=0;
	char c;
	
	char userInput[UserInputLength+2];
	
	
	

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
	char *temp=strtok(userInput, " \t");
	strcpy(args[numberOfArguments++],temp);
	
	
	
	while((temp=strtok(NULL, " \t"))!= NULL){
		
		strcpy(args[numberOfArguments++],temp);
		
		
		if (numberOfArguments == MaxArraySize){
			if(fprintf(stderr, "Too many arguments") < 0)
				error("fprintf");
			exit(EXIT_FAILURE);
		}

	}
	
	numberOfArguments--; 
	*number =numberOfArguments;
	
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


