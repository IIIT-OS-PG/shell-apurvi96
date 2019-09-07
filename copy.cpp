#include<iostream>
#include<string>
#include<stdio.h>
#include<stdlib.h>//getenv
#include<unistd.h>//gethostname getcwd
#include<sys/types.h> 
#include "global.h"
#include<cstring>
#include<sys/stat.h> 
#include<sys/wait.h> 
#include<cstdio>
#include<fcntl.h>
#include "BUILTIN.h"


#define maxi 4096 
#define clear() printf("\033[H\033[J") 


using namespace std;



void setvariables()
{
	
	PATH=getenv("PATH");
	USER=getenv("USER");
	HOME=getenv("HOME");
	gethostname(HOSTNAME,1024);

	//SETTING PS1 VALUE
	string hostname=string(HOSTNAME);
	string user=string(USER);

	string ans =user+"@"+hostname+":";

	char *ans1=&*ans.begin();
	setenv("PS1",ans1,1);
	PS1=getenv("PS1");
	


}
void Print_prompt()
{
	string symbol;

	
	getcwd(currdir,sizeof(currdir));
	//cout<<currentdir;

	string ps1=string(PS1);
	string dir=string(currdir);

	if(strcmp(USER,"root")!=0)
	{
		symbol="$";
	}

	else
	{
		symbol="#";
	}


	string prompt=ps1+currdir+symbol+" ";
	cout<<prompt;
}

void sig_handler(int sig)
{
	if(sig==SIGINT)
	{
		cout<<"\n";
		Print_prompt();
		fflush(stdout);
	}

	if(sig==SIGQUIT)
	{
		cout<<"QUIT\n";
		Print_prompt();
		fflush(stdout);

	}

	if(sig==SIGTSTP)
	{
		cout<<"STOP\n";
		Print_prompt();
		fflush(stdout);

	}


}

void Parse_Input(char *input,char **buffer)
{
	

	while(*input!='\0')
	{
		
		while(*input==' ' || *input=='\n' || *input=='\t') //Seperating each word of command and giving address in buffer (pointer array)
		{
			*input++='\0';
			//cout<<"hi";
			while(*input==' ' || *input=='\t')
			{
				input++;
				//continue;
			}

			

			
			
		}

		if(*input!='\0')
		*buffer++=input;

		while(*input!=' ' && *input!='\n' && *input!='\t' && *input!='\0')
			input++;
	}
		*buffer=NULL;
}


int Check_redirect(char **buffer, char **opfilep)
{
	int flag=0;

	for(int i=0;buffer[i]!=NULL;i++)
		{
			//cout<<"buffer value "<<i<<" "<<buffer[i]<<"\n";
			//char *check;
			//strcpy(check,buffer[i]);
				if(strcmp(buffer[i],">")==0 || strcmp(buffer[i],">>")==0)

				{
						if(strcmp(buffer[i],">")==0)
						{
							flag=1;
						}
						
						else
							flag=2;
			
					
							*opfilep=buffer[i+1];
							*(buffer+i)=NULL;


				}
		}

			return flag;

}


void Execute_Command(char **buffer)
{

	char *opfile;
	int fd1;
	int redirect=Check_redirect(buffer,&opfile);

	int status;
	pid_t pid;
	pid_t ppid=getpid();

	pid=fork();

	if(pid<0)
	{
		cout<<"CHILD PROCESS FAILED";
		exit(1);
	}

	//COMMAND NOT BG NOT REDIRECT

	else if(pid==0 && redirect==0)
	{
		// cout<<"enter child ";
		//  setpgid(getpid(),getpid());     
		// tcsetpgrp(STDIN_FILENO,getpid());
		int st=execvp(*buffer,buffer);

		if(st<0)
		{
			perror("ERROR");
			exit(1);

		}

		

	}

		//COMMAND NOT BG BUT REDIRECT
	else if(pid==0 && redirect!=0 )
	{

			//tcsetpgrp(STDIN_FILENO,getpid());
			if(redirect==1)
			fd1=open(opfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);

			else if(redirect==2)
				fd1=open(opfile, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);

					if(fd1==-1)
					{
						cout<<"UNABLE TO OPEN FILE FOR WRITING OUTPUT|n";
						exit(1);
					}

			dup2(fd1,1);

			int st=execvp(*buffer,buffer);

					if(st<0)
					{
						cout<<"EXECTION FAILED\n";
						exit(1);

					}

	}



	else
	{
			      

		while(wait(&status)!=pid);
		
		}

	
}

int main (int argc,char *argv[])
{
	string input;
	char *buffer[1000];
	getcwd(SHELL_DIR,1000);
	setvariables();
	
	clear();
	while(1)
	{
		int bg=0;

		 if ( signal(SIGINT,sig_handler) == 0 )
            continue;
        if ( signal(SIGQUIT,sig_handler) == 0 )
            continue;
        if ( signal(SIGTSTP,sig_handler) == 0 )
            continue;

		Print_prompt();


		//PARSING INPUT AND STORING IN BUFFER
		getline(cin,input);
		char *input1=&*input.begin();
		Parse_Input(input1,buffer);
		int buffer_length=0;

		//FINDING BUFFER LENGTH
		for(int i=0;buffer[i]!=NULL;i++)
		{
			//cout<<" "<<buffer[i];
			buffer_length++;
		}


		if(strcmp(buffer[0],"exit")==0)
			_exit(0);


		 int builtin=check_builtin(buffer);

		 if(builtin==0)
		 	 Execute_Command(buffer);

		

		














		// //CHECKING IF BACKGROUND COMMND if as LS &
		// if(strcmp(buffer[buffer_length-1],"&")==0)
		// {
		// 	bg=1;
		// 	buffer[buffer_length-1]=NULL;
		// 	buffer_length--;
		// }

		// // if ls&
		// 	char checkbg[1000];
		// 	strcpy(checkbg,buffer[buffer_length-1]);
		// 	int len =strlen(checkbg);

		// 	if(checkbg[len-1]=='&')
		// 	{
		// 		bg=1;
		// 		buffer[buffer_length-1][len-1]='\0';
		// 	}

	
		//	cout<<buffer[buffer_length-1]<<" ";

		// cout<<"background "<<bg<<"\n";

	// if(bg==0)
		//  Execute_Command(buffer);

		// if(bg==1)
		// {

		// }

		// if(bg==1)
		// 	Execute_bagkground(buffer);
			
			


			



		// for(int i=0;buffer[i]!=NULL;i++)
		// {
		
		// cout<<" "<<buffer[i]<<"\n";
		// }

	
		

		
		
	}
	
	return 0;
}
