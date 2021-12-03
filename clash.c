#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define UserInputLength 1337 
#define ArgumentLength 80
#define MaxArraySize ((UserInputLength/ArgumentLength) +1)
  
static void error(char*);
static void getPrompt();
static void getArguments(char [MaxArraySize][ArgumentLength], int *, char *);

//----------------------------------------------------------------------

int main(int argc, char*argv[]){
	
	char args[MaxArraySize][ArgumentLength];
	char **arguments=(char**)calloc(MaxArraySize,sizeof(arguments)); 
	char userInput [UserInputLength];

	int numberArgs =0;
	
		while(1){
			getPrompt();
				getArguments(args,&numberArgs, userInput);
			
			
			if (numberArgs==0){
				continue;
			}
				
			for(int i =0; i < numberArgs; i++)
				arguments[i]=args [i];
				
			pid_t pid= fork(); 
		
			if (pid < 0){
				error("fork");
			}else if (pid > 0){
				
				//printf("Elternprozess");
				int status;
				
				if ((waitpid (pid, &status, 0))== -1){
					error("waitpid");
				}
				
				if (WIFEXITED(status)){
				
					printf("Exit Status: [ %s ] = %d\n", userInput, WEXITSTATUS(status));
					fflush(stdout);
					
				}
				
			
				
			}else{
				
				printf("Kindprozess");
				// Load a new program in the child prozess
				// nameOfCommand, agruments
				execvp(arguments[0], arguments);
				
				// in the case of failure excex return
				error("execvp");
			}
			
				
		}
		
		
		for(int i =0; i <=numberArgs; i++)
				free(arguments[i]);
				
		free(arguments);
			
	
}

//----------------------------------------------------------------------



static void getArguments(char args[MaxArraySize][ArgumentLength], int *number, char *input){
	

	int numberOfArguments=0;
	char userInput[UserInputLength+2];
	char c;
	
	
	

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
		
	}else if( strlen(userInput)> 2 && strlen(userInput)< UserInputLength) {
		userInput[strlen(userInput)-1]='\0';
	}else{
		numberOfArguments = -1; 
		
	}


	if (sprintf(input,"%s", userInput )< 0){
		error("sprintf");
	}
	
 
	// Spiliting up the input 
	char *temp=strtok(userInput, " \t");
	if(snprintf(args[numberOfArguments++],ArgumentLength, "%s",temp )< 0){
		error("snprintf");
	}
	
	while((temp=strtok(NULL, " \t"))!= NULL){
		
		if (snprintf(args[numberOfArguments++],ArgumentLength, "%s",temp ) < 0){
			error("snprintf");
		}

		// Exceed maximal Array Size --> too many Arguments 
		if (numberOfArguments == MaxArraySize-1){
			if(fprintf(stderr, "Too many arguments") < 0)
				error("fprintf");
			exit(EXIT_FAILURE);
		}

	} 
	
	*number = numberOfArguments;
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


