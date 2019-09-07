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
#include "init_shell.h"
#include "BG_HANDLE.h"
#include "PIPEHANDLE.h"
#include<pwd.h>
#include <fstream>
#include <map> 
#include<algorithm>
#include <iterator> 


#define maxi 4096 
#define clear() printf("\033[H\033[J") 

using namespace std;
map <string,string> Alias_map;


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






void Execute_Command(char **buffer)
{

	char *opfile;
	int fd1;
	int redirect=Check_redirect(buffer,&opfile);
	int bg =Check_bg(buffer);
	int status;
	pid_t pid;
	pid_t ppid=getpid();

	pid=fork();

	if(pid<0)
	{
		cout<<"CHILD PROCESS FAILED";
		exit(1);
	}

	

	else if(pid==0 && redirect==0)
	{
		// cout<<"enter child ";
		//  setpgid(getpid(),getpid());     
		// tcsetpgrp(STDIN_FILENO,getpid());
		int st=execvp(*buffer,buffer);

		if(st<0)
		{
			perror("ERROR");
			exit(127);

		}

		

	}

		
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
						exit(127);

					}

	}



	else
	{
			      
		if(bg==0)
		{
			waitpid(pid,&status,0);

			if(WIFEXITED(status))
			{
				exit_status_child=WEXITSTATUS(status);
			}
			
		}
		}
	
}



				
		
 void Execute_pipe(int pipes, char **pipecommand)
 {
 	int count=0;
 	int fd[2];
 	pid_t pid;
 	int status;
 	int fdchange;
 	char pipiei[1000];
 	char *bufferpipe[1000];

 	while(count<pipes+1)
	{
		pipe(fd);

	 	pid=fork();

	 	if(pid<0)
		{
			cout<<"CHILD PROCESS FAILED";
			exit(1);
		}

		else if(pid==0)
		{

			dup2(fdchange,0);

			if(count<pipes)
				dup2(fd[1],1);

			//else if()

			close(fd[0]);
			strcpy(pipiei,pipecommand[count]);
			Parse_Input(pipiei,bufferpipe);

			//for all intermediate pipes
			if(count<pipes)
			{
			int st=execvp(*bufferpipe,bufferpipe);

					if(st<0)
					{
						perror("ERROR");
						exit(127);

					}
			}

			//for final pipe
			else if(count==pipes)
			{

				char *opfile1;
				int fd1;
				int redirect=Check_redirect(bufferpipe,&opfile1);

				//no redirection in last pipe
					if(redirect==0)
					{
							int st=execvp(*bufferpipe,bufferpipe);

						if(st<0)
						{
							cout<<"EXECTION FAILED\n";
							exit(127);

						}
					}

			//if redirect in last pipe

					else if(redirect!=0 )
					{

						//tcsetpgrp(STDIN_FILENO,getpid());
								if(redirect==1)
								fd1=open(opfile1, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);

								else if(redirect==2)
							fd1=open(opfile1, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);

								if(fd1==-1)
								{
									cout<<"UNABLE TO OPEN FILE FOR WRITING OUTPUT|n";
									exit(1);
								}

							dup2(fd1,1);

						int st=execvp(*bufferpipe,bufferpipe);

								if(st<0)
								{
									cout<<"EXECTION FAILED\n";
									exit(127);

								}

					}


			}

		}

		else
		{
			waitpid(pid,&status,0);

				if(WIFEXITED(status))
				{
					exit_status_child=WEXITSTATUS(status);
				}
			close(fd[1]);
			fdchange=fd[0];
			count++;
		}

	}
 }


int Check_alias(char **buffer)
{
	char alias_string[1000];
	string new_cmd="";
	string old_cmd="";

	//cout<<buffer[2]<<"im 2 \n";

	int m=1;

	string value="";
	
	
	
	while(buffer[m]!=NULL)
	{
		//cout<<"enter ";
		char value1[1000];
	strcpy(value1,buffer[m]);
		//cout<<value1<<" substring\n ";
		string s1=value1;
		value=value+s1+" ";
		m++;
		
	}

	//cout<<value<<"hi";
	 	strcpy(alias_string,value.c_str());

	
	 int len=strlen(alias_string);
	 alias_string[len-1]='\0';
	 len=len-1;
	  //cout<<"string is "<<alias_string;
	// cout<<"len is "<<len<<"\n";
	if(alias_string[0]=='=')
			return -1;

	else if(alias_string[len-1]=='=')
			return -1;



		int i;
		int flageq=0;

	for(int e=0;alias_string[e]!='\0';e++)
	{
		if(alias_string[e]=='=')
			{
				
				flageq=1;
			}

	}

	if(flageq==0)
		return -1;

	for(i=0;alias_string[i]!='=';i++)
	{
			if(alias_string[i]!=' ')
			new_cmd=new_cmd+alias_string[i];
		
	}
			new_cmd[i]='\0';

			
			i++;
			while(alias_string[i]!='\0' )
			{
				if(alias_string[i]!=' ')
					break;

				i++;
			}

	cout<<new_cmd<<"key \n";

	if(alias_string[i]=='\"')
	{
		if(alias_string[len-1]!='\"' || i==len-1 )
			return -1;
	}

	if(alias_string[i]=='\'')

	{
		if(alias_string[len-1]!='\'' || i==len-1 )
			return -1;
	}

	int k;
	for(k=i;alias_string[k]!='\0' ;k++)
	{

		if(alias_string[k]=='\"' || alias_string[k]=='\'')
			continue;

			//if(alias_string[k]!=' ')
			old_cmd=old_cmd+alias_string[k];
	}
		old_cmd[k]='\0';
		cout<<old_cmd<<"value \n";

		Alias_map[new_cmd]=old_cmd;







}
 

int main (int argc,char *argv[])
{
	string input;
	char *buffer[1000];
	char *pipecommand[1000];
	getcwd(SHELL_DIR,1000);
	setvariables();
	
	clear();
	while(1)
	{
		

		 // if ( signal(SIGINT,sig_handler) == 0 )
   //          continue;
   //      if ( signal(SIGQUIT,sig_handler) == 0 )
   //          continue;
   //      if ( signal(SIGTSTP,sig_handler) == 0 )
   //          continue;

		Print_prompt();


		//PARSING INPUT AND STORING IN BUFFER
		getline(cin,input);
		char input2[1000];
		strcpy(input2,input.c_str());
		char *input1=&*input.begin();

		//cout<<input2<<"main\n";
		Parse_Input(input1,buffer);

		int buffer_length=0;

		//FINDING BUFFER LENGTH
		for(int i=0;buffer[i]!=NULL;i++)
		{
			//cout<<" "<<buffer[i];
			buffer_length++;
		}

		//handling aliases
			if(strcmp(buffer[0],"alias")==0)
			{
				string asd;
				int alias_flag=Check_alias(buffer);

				if(alias_flag==-1)
					cout<<"Wrong alias format";

				
				continue;

			}
		//checking if given command is an set alias 
			int aliasflag=0;
			string aliasinput="";
			string cmdal=(string)buffer[0];

			if(Alias_map[cmdal]!="")
				aliasflag=1;
			//cout<<aliasflag<<" ";

			//executing alias command

				if(aliasflag==1)
				{

					aliasinput=Alias_map[cmdal]+" ";
					//cout<<aliasinput<<" cmd is ";
					int m=1;


						while(buffer[m]!=NULL)
						{
							//cout<<"enter ";
							char value1[1000];
						strcpy(value1,buffer[m]);
							//cout<<value1<<" substring\n ";
							string s1=value1;
							aliasinput=aliasinput+s1+" ";
							m++;
							
						}

						char *aliasinput1=&*aliasinput.begin();
								strcpy(input2,aliasinput.c_str());

						Parse_Input(aliasinput1,buffer);
						//cout<<aliasinput<<" cmd is ";

				}
			

				
			















		//handling pipe
		int pipes= Count_pipe(buffer);
			
		if(pipes > 0)
		{
			
		Parse_pipe(input2,pipecommand);
		Execute_pipe(pipes,pipecommand);

		

		}

		else if(pipes==-1)
			cout<<"WRONG COMMAND FORMAT\n";


		else
		{
		//handling various commands
		if(strcmp(buffer[0],"exit")==0)
			_exit(0);


		 int builtin=check_builtin(buffer);

		 if(builtin==0)
		 	 Execute_Command(buffer);

		}

		
		//cout<<"exit status is "<<exit_status_child<<"\n";













		
			
			


			



		// for(int i=0;buffer[i]!=NULL;i++)
		// {
		
		// cout<<" "<<buffer[i]<<"\n";
		// }

	
		

		
		
	}
	
	return 0;
}
