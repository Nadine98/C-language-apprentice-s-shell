#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "plist.h"

#define UserInputLength 1337 
#define ArgumentLength 80
#define MaxArraySize ((UserInputLength/ArgumentLength) +1)
  
static void error(char*);
static void getPrompt();
static void getArguments(char [MaxArraySize][ArgumentLength], int *, char *);
static void parentProcess(pid_t ,int ,char *);
static void zombie();
static int callback(pid_t, const char *);

//----------------------------------------------------------------------

int main(int argc, char*argv[]){
	
	char args[MaxArraySize][ArgumentLength];
	char **arguments=(char**)calloc(MaxArraySize,sizeof(arguments)); 
	char userInput [UserInputLength];

	int numberArgs =0;
	int background =0;
	
	while(1){
		
		zombie();
		getPrompt();
		getArguments(args,&numberArgs, userInput);
		
		
		// if a blank line appears then continue with the next prompt 
		if (numberArgs<2)
			continue;
		
		
		// Checking if the process is put in the background
		if((strcmp(args[numberArgs-1],"&")) ==0){
			
			background=1;
			
			char *c=strchr(userInput,'&');
			*(c-1)='\0';
				
			numberArgs=numberArgs-1;
		}
		
		
		// copy the arguments in an array of pointer for the exec function 
		for(int i =0; i < numberArgs; i++)
			arguments[i]=args[i];
			
			
		// Creating a child process	
		pid_t pid= fork(); 
		
		
		// Error occurs by the execution of fork
		if (pid < 0){
			error("fork");
		}
		// Being in the parent process 
		else if (pid > 0){
			parentProcess(pid,background,userInput );
			background=0;
		}
		
		// Being in the child process 
		else{
			// Checking if the cd-command is used
			if((strcmp(arguments[0], "cd")) == 0){
				if ( (chdir(args[1])) == -1 ){
					error("chdir");
				}
				exit(EXIT_SUCCESS);
			}
			
			// Checking if the jobs-command is used
			else if ((strcmp(arguments[0], "jobs")) == 0){
				walkList(callback);
			}
			
			else{
				// Load a new program in the child prozess
				execvp(arguments[0], arguments);
				error("execvp");
			}
		 
		}
		
			
	}
	
		
	for(int i =0; i <=numberArgs; i++)
			free(arguments[i]);
	free(arguments);
		
	
}

//----------------------------------------------------------------------


static int callback(pid_t pid, const char *cmdLine){
	printf("%d %s\n", pid, cmdLine);
	return 0;
}


static void zombie (){
	
	int bgStatus;
	pid_t bgpid;
	

		while (1){
			
			bgpid=waitpid(-1, &bgStatus,WNOHANG );
			
			
			// status is not available for any existing process --> Continue with printing prompt
			if(bgpid == 0){
				break;
			}
			// Error occurs
			else if(bgpid < 0){
				// no child process exists 
				if (errno == ECHILD)
					break;
				// error during the process execution 
				error("waitpid");
			}
			
			//status is available for a background process
			else{
				char command[UserInputLength+1];
				
				if((removeElement(bgpid,command,UserInputLength+1)) < 0){
					continue;
				}
				
				if (WIFEXITED(bgStatus)){
					printf("Exit Status: [%s] = %d\n", command, WEXITSTATUS(bgStatus));

				}
				
			}
		}
}

static void parentProcess(pid_t pid, int background,char *commandLine){


	// Foreground process
	if(!background ){

		int status;
		if ((waitpid (pid, &status, 0))== -1){
			error("waitpid");
		}	
		
		if (WIFEXITED(status)){
			printf("Exit Status: [%s] = %d\n", commandLine, WEXITSTATUS(status));
		}
	}
	// Background process
	else{
		
		// Insert the process in the list
		int x=insertElement(pid, commandLine);
		
		if(x==-2){
			error("insertElement - full memeory");
		}
		
	}	
}

static void getArguments(char args[MaxArraySize][ArgumentLength], int *number, char *input){
	

	int numberOfArguments=0;
	char userInput[UserInputLength+2];
	char c;
	
	

	// Reading from Stdin --> Using maxLength 1337+2 for the valididation of the input length 
	if (fgets(userInput, UserInputLength+2, stdin) == NULL){
		if(feof(stdin)>0){
			printf("close clash \n");
			exit(EXIT_SUCCESS);
		}
		error("fegts");
	}
	
	fflush (stdin);	
	
	// Input has a size greater than 1337 then the input is too long --> Ignoring the following character
	if (strlen(userInput)> UserInputLength){
		
		while((c=getchar()) != '\n' && c!=EOF){
			continue;
		}
		userInput[UserInputLength]='\0';
		
	}else if( strlen(userInput)> 1 && strlen(userInput)< UserInputLength) {
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
	
    unsigned int dirLength=10;
    char *dir=NULL;
   
 
    while(1){
		// fetch the directory
		char *tmp=getcwd(dir,dirLength);

		// Checking if the lenght of the directory is too long for dir 
		if ((tmp==NULL) && errno == ERANGE){

				dirLength= dirLength*2;
				dir = realloc(dir,dirLength);
				if (dir ==NULL)
					error("realloc");
		}
	    
		// Checking if an error occurs during the execution of getcwd    
		else if (tmp==NULL && errno != ERANGE){
			error("getcwd");
		}
	    
	    	// No problems during the execution of getcwd
	    	else{
			break;
		}
		
	}
    

	printf("%s: ", dir);
	fflush(stdout);
	free(dir);
}

// Error handling
static void error(char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}


