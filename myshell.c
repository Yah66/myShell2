#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

static sigjmp_buf env;
static volatile sig_atomic_t jmp_active = 0;
static char **history;
static int histLength = 0;
static char **args;

// this for cd mehod
int cd(char *path)
{
	return chdir(path);
}



// take the input and turn it to array
char **get_input(char *input)
{
	char **command = malloc(8 * sizeof(char *));
	if (command == NULL)
	{
		perror("Malloc faild");
		exit(1);
	}
	
	char *separator = " ";
	char *p;
	int index = 0;

	p= strtok(input, separator);// breaks string str into a series of tokens using the delimiter
	
	while (p != NULL) // check the end of the array
	{
		command[index] = p;
		index++;
		p = strtok(NULL,separator);
	}
	
	command[index] = NULL; // put the last index with NULL
	return command;
}


// print the current working directory
void printCurrWorkDir()
{
	char currWorkDir[1024];
	// char *getcwd(char *buf, size_t size);
	getcwd(currWorkDir, sizeof(currWorkDir));  //return string containing an absolute pathname that is the current working directory of the calling process
	printf("\033[1;32m"); // bold green
	printf("yahiaHalaby@shell");
	printf("\033[0m");
	printf(":");
	printf("\033[0;31m"); // bold blue
	printf("~%s$ ", currWorkDir);
	printf("\033[0m"); // reset
}

// add the typed commands to your history
int addToHistory(char *input)
{
	// initialize
	if (history == NULL) {
		history = calloc(sizeof(char *) * 100, 1);
		if (history == NULL) {
			fprintf(stderr, "error: memory alloc error\n");
			return 0;
		}
	}

	// copy the input
	char *line;
	line = strdup(input);
	if (line == NULL){
		return 0;
	}
	history[histLength++] = line;
	return 1;
}


// print all the history
void showHistory()
{
	for (int i = 0; i < histLength; i++) {
		printf("%d\t%s\n",i + 1,history[i]);
	}
}

// clear your history
int emptyHistory(void)
{
	for (int i = 0; i < histLength; i++){
		free(history[i]);
	}
	
	histLength = 0;
	return 0;
}

void help()
{

	printf("\033[0;33m"); // yellow
	puts("\t\t😊😀😀😀😀😀😀😀😀😀😀😀😀😀😀😀😊");  
	printf("\t\t*** Hello in Yahia shell ***\n\n\t\tThese shell commands are defined internally.\n\t\t\thistory\tquit\tpause \n\t\t\tcd\texit\tsudo \n\t\t\tgrep\tmkdir \ttest \n\t\t\techo\ttime\ttrue \n\t\t\tfalse\texit\thelp \n\t\t\tcat\tgedit\tls \n\t\t\tclear\tman\tcp \n\t\t\tmkdir\trm\trmdir \n\t\t\tgcc\tand more...\n\n");
	puts("\t\t😊😀😀😀😀😀😀😀😀😀😀😀😀😀😀😀😊"); 
	printf("\033[0m"); // reset
}


/*void sigint_handler(int signo) {
	if (!jmp_active)
	{
		return;
	}
    siglongjmp(env, 42);
}*/

// main method
int main (int argc, char *argv[])
{

	char **command;
	char *input;
	pid_t child_pid;
	int stat_loc;
	int ch;
 	char* hisUp;
	/*struct sigaction parent;
	parent.sa_handler = sigint_handler;
	sigemptyset(&parent.sa_mask);
	parent.sa_flags = SA_RESTART;
	sigaction(SIGINT, &parent, NULL);*/

	


	if (argc == 2){
           //to associates a new filename for reading with the given open stream and at the same time closes stdin
		freopen(argv[1], "r", stdin);
         }
        
	while (!feof(stdin))       //tests the end-of-file indicator for the given stream.
	{
		  printCurrWorkDir();
		//if (sigsetjmp(env,1) == 42) {
		  //  printf("\n");
		  //  continue;
		//}
		//jmp_active = 1;

	
				
		 input = readline(""); 

		/*if (input == NULL) { // exit if you pressed CTRL + D
		    printf("\n");
		    exit(0);
		}*/
		addToHistory(strdup(input));
		command = get_input(input);
			
		if (!command[0]) // when press enter
		{
		    free(input);
		   free(command);
		    printf("\n");
		    continue;
		}
        
            // printf("!command[0]: %s\n",command[0]);
		if (strcmp(command[0],"exit") == 0) // exit if you typed exit
		{
			exit(0);
		}
		
		// print help menu
		if (strcmp(command[0],"help") == 0) // help menu 
		{
			help();//execute help method 
			continue;
		}
		if (strcmp(command[0],"clr") == 0) // clear command
		{
			system("clear");
			continue;
		}
                if (strcmp(command[0],"environ") == 0) // environ
		{
			system("set");
			//system("set");
			continue;
		}
               if (strcmp(command[0],"echo") == 0) // echo command to print comment
		{
			if (command[1] == NULL)
			{
				continue;
			}
			else{
			   int c=1;
			   while(command[c]!=NULL){
			    printf("%s ",command[c++]);
			    }
                          printf("\n\n");
			}
			continue;
		}

	        if (!strcmp(command[0],"pause"))   // "pause" command we use "https://en.wikibooks.org/wiki/C_Programming/stdio.h/getchar"
		{
 			printf("continue in the shell by Enter !");
 			int k;
			while ((k= getchar()) != '\n'  &&  k != EOF) { }
			//getpass("continue in the shell by Enter !");
			continue;
		}
	        if (!strcmp(command[0],"quit"))   // "quit" command
		{
 			exit(0);
		}
	   
		// check if the input is cd ?		
		if (strcmp(command[0],"cd") == 0)
		{
			// check if the next input is empty ?
			if (command[1] == NULL)
			{
				continue;
			}
			// check if the next input is the correct direction ?
			else if (cd(command[1]) < 0)
			{
				perror(command[1]); // error message
			}
			else if(cd(command[1]) ==0){
			   cd(command[1]);
			}
			continue;
		}
		/*if ((strcmp(command[0],"history") == 0) && (strcmp(command[1],"-c") == 0))
		{
			emptyHistory();
			//continue;
		}*/
		if (strcmp(command[0],"history") == 0)///history command to print last comands on shell 
		{
			showHistory();
			continue;
		}
		
		child_pid = fork(); // fork the operation
		
		// check if the Fork operation is failed!
		if (child_pid < 0)
		{
			perror("Fork failed!");
			exit(1);
		}
		if (child_pid == 0)
		{
			/*struct sigaction child;
			child.sa_handler = sigint_handler;
			sigemptyset(&child.sa_mask);
			child.sa_flags = SA_RESTART;
			sigaction(SIGINT, &child, NULL);*/
			
			// check if the command if exist or not ?
			if (execvp(command[0],command) < 0)
			{
				perror(command[0]);
				exit(0);
			}
			// printf("won't be printed\n");
		}
		else
		{
			waitpid(child_pid,&stat_loc,WUNTRACED);
		}
		// free the ram in order to reuse the same memory location
		free(input);
		free(command);
		
	}
	return 0;
}
