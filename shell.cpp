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
#include<list>
#include<termios.h>
#include<dirent.h>
//#include "Triei.h"


#define maxi 4096 
#define clearscre() printf("\033[H\033[J") 

using namespace std;
//map <string,string> Alias_map;
list <string> command_history;
struct Trie
{
	struct Trie* char_map[128];
 
	
	int is_end;

	
};
typedef struct Trie trie;
//trie *root_t;

trie* get_node()
{
	trie *node = new Trie();
	for(int i=0;i<128;i++)
		node->char_map[i]=NULL;

	node->is_end=0;
	return node;
}

trie* root_t;
trie* history_t=get_node();

void insert_trie(trie *root,string s)
{
	int i;
	trie * traverse=root;

	for(i=0;i<s.length();i++)
	{

		int ic=s[i];
		//cout<<ic<<" ";
		trie *checknode= traverse->char_map[ic];
		if(checknode==NULL)
	 	{
	 		traverse->char_map[ic]=get_node();
		}

		 traverse=traverse->char_map[ic];


	 }

	traverse->is_end=1;

}

string search(trie *root,string s)
 {
 	char c;
	string f="";
	trie* traverse=root;
	
	for(int i=0;i<s.length();i++)
	{

		int ic=s[i];
		//cout<<ic<<" ";

		trie *checknode= traverse->char_map[ic];
		if(checknode==NULL || checknode->is_end==1)
	 	{
	 		return "";
		}


		 traverse=traverse->char_map[ic];

	 }

	 int check_count=0;

	 while(traverse->is_end!=1)
	 {
	 	check_count=0;

	 	for(int i=0;i<128;i++)
	 	{ 
		   if(traverse->char_map[i]!=NULL)
		   {
		   		check_count++;
		  
            }
	     }

	     if(check_count>=2)
	     	return "";

	     for(int i=0;i<128;i++)
	 	{ 
		   if(traverse->char_map[i]!=NULL)
		   {
		   	    c=i;
		   		f=f+c;
		  
            }
	     } 
	     	int mg=c;
	     	traverse=traverse->char_map[mg];
	 }

	 	return f;


}


void Raw_Mode()
{
	
	struct termios tm;
	tcgetattr(STDIN_FILENO, &tm);
	tm.c_lflag &= ~(ICANON);
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tm);

}

void sig_handler(int sig)
{
	if(sig==SIGINT)
	{
		cout<<"\n";
		string pr=Print_prompt();
		cout<<pr<<" ";
		fflush(stdout);
		
	}

	if(sig==SIGQUIT)
	{
		cout<<"QUIT\n";
		string pr=Print_prompt();
		cout<<pr<<" ";
		fflush(stdout);


	}

	if(sig==SIGTSTP)
	{
		cout<<"STOP\n";
		string pr=Print_prompt();
		cout<<pr<<" ";
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
			//exit_status_child=errno;
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
						//exit_status_child=errno;
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
					//	exit_status_child=errno;
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
							//exit_status_child=errno;
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
									//exit_status_child=errno;
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

	//cout<<new_cmd<<"key \n";

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
	//	cout<<old_cmd<<"value \n";

		Alias_map[new_cmd]=old_cmd;







}
 
void History_file(string hist)
{
	//int HSIZE=5;

	if(command_history.size()!=HSIZE)
	{
		command_history.push_back(hist);
		//history_t
		FILE *fhist=fopen("history.txt","a+");
		fprintf(fhist,"%s\n",hist.c_str());
		fclose(fhist);
	
	}	
	else
	{
		command_history.pop_front();
		command_history.push_back(hist);
		FILE *fhist=fopen("history.txt","w");

		for(auto it=command_history.begin();it !=command_history.end();it++)
		{
			string History_to_add=*it;
			fprintf(fhist,"%s\n",History_to_add.c_str());
			//cout<<*it<<"\n";
		}
		
		fclose(fhist);
		
	}
		
}

void fill_history()
{
	//cout<<"enter fill history\n";

	char buff[4096];

	FILE *fhistre=fopen("history.txt","r");
	if(fhistre==NULL)
	{
		//cout<<"cannot open file\n";
		
	}
	else
	{
		//cout<<"enter else\n";

		while(fgets(buff,sizeof(buff),fhistre)!=NULL)
		{
			//cout<<"entered file\n";
				//fputs(buff,fhistre);
				//cout<<buff<<" ";
			int i;
			for(i=0;buff[i]!='\n';i++);
				buff[i]='\0';

				string fillh=(string)buff;

				command_history.push_back(fillh);
		}
	}

	fclose(fhistre);
	
}

void Execute_history()
{
	for(auto it=command_history.begin();it !=command_history.end();it++)
		{


			cout<<*it<<"\n";

		}
}

void execute_open1(string path_f,char *binary)
{
	//cout<<"path of player :"<<binary<<"";
	//cout<<"file : "<<path_f<<"";
	int status;
	pid_t pid;

	pid=fork();

	if(pid<0)
	{
		perror("ERROR NO CHILD");
	}

	else if(pid==0)
	{
		int st=execl(binary,"xdg-open",path_f.c_str(),(char *)0);

					if(st<0)
					{
						perror("ERROR");
						//exit_status_child=errno;
						exit(127);

					}
	}

	else
	{
		waitpid(pid,&status,0);

				if(WIFEXITED(status))
				{
					exit_status_child=WEXITSTATUS(status);
				}
	}

}
void parse_open(char **buffer)
{
	char *file_Name[3];
	char buff[1000];
	char *file_to_open=buffer[1];
	char *binary;
	string file_to_open1=(string)file_to_open;

	file_Name[0]=strtok(file_to_open,".");
	file_Name[1]=strtok(NULL,".");
	//cout<<file_Name[0]<<" ";
		//cout<<file_Name[1]<<" ";

	//get path from myrc
		FILE *mf=fopen("myrc.txt","r");

		while(fgets(buff,sizeof(buff),mf)!=NULL)
	{
		char *bufffilepath[4];
		bufffilepath[0]=strtok(buff," ");
		bufffilepath[1]=strtok(NULL," ");
		//cout<<"0: "<<bufffilepath[0]<<"";
		//cout<<"1: "<<bufffilepath[1]<<"";
		char type[1000],pathofplayer[1000];
		 strcpy(type,bufffilepath[0]);
		 strcpy(pathofplayer,bufffilepath[1]);
		

		if(strcmp(file_Name[1],"mp4")==0)
		{
			 if(strcmp(type,"VIDEO")==0)
			
			{
				//cout<<type<<" ";
				binary=pathofplayer;
				break;
			}

		}

		else if(strcmp(file_Name[1],"mp3")==0)
		{
			 if(strcmp(type,"SONG")==0)
			
			{
				//cout<<type<<" ";
				//cout<<pathofplayer<<" ";

				binary=(char *)pathofplayer;
				break;
				//cout<<binary<<" ";
			}

		}

		else if(strcmp(file_Name[1],"pdf")==0)
		{
			 if(strcmp(type,"PDF")==0)
			
			{
				//cout<<type<<" ";
				binary=pathofplayer;
				break;
			}

		}

		else if(strcmp(file_Name[1],"txt")==0)
		{
			 if(strcmp(type,"text")==0)
			
			{
				//cout<<type<<" ";
				binary=pathofplayer;
				break;
			}

		}

		
		
		//type[strlen(type)-1]='\0';
		//cout<<"type2 is "<<type<<"\n";
		

	}

	string path_of_myfile="/home/apurvi/Desktop/2019201093_assignment_1/"+file_to_open1;
		// cout<<"pis "<<path_of_myfile<<"\n";
		//cout<<"bis "<<binary<<"\n";
		fclose(mf);

		execute_open1(path_of_myfile,binary);
	//cout<<file_to_open<<" ";


}


void Create_Trie()
{
	char current_dir[1024];
	int i=0;
	root_t= get_node();
	//root_t=root_t1;
	DIR *di;
	struct dirent *dir;
	string dir_list[1000];

	getcwd(current_dir,1024);
	//cout<<current_dir<<"current dir \n";
	di=opendir(current_dir);

	if(di==NULL)
	{
		perror("CANNOT OPEN DIR");
		return;

	}

	while((dir=readdir(di))!=NULL)
	{
		if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0)
		{
			dir_list[i++]=dir->d_name;
		}
	}

	closedir(di);
	int List_Length=i;

	for(int k=0;k<i;k++)
	{
		//cout<<dir_list[k]<<"\n";
		insert_trie(root_t,dir_list[k]);
			
	}

		
	


}

void execute_cd(char **buffer)
{
	
	char pathdir[2000];
	int len =strlen(HOME);
	int i;
	strcpy(pathdir,HOME);

	if(buffer[1]==NULL)
	{
		Create_Trie();
		chdir(HOME);
		exit_status_child=0;
	}

	else if(strcmp(buffer[1],"~")==0)
	{
		Create_Trie();
		chdir(HOME);
	}


	else if(buffer[1][0]=='~')
	{
		for( i=1 ; buffer[1][i]!='\0' ; i++ )
        {
            pathdir[i+len-1] = buffer[1][i];
        }

        pathdir[i+len-1]='\0';

        if( chdir(pathdir) != 0 )
        {

            perror("Error");
           exit_status_child=errno;
        }

        else
        {
        	Create_Trie();
			exit_status_child=0;
	    }
	}

	else if( chdir(buffer[1]) != 0)
	{
		 perror("Error");
		  exit_status_child=errno;
	}

	else
	{
		Create_Trie();
	   exit_status_child=0;
    }

}



int check_builtin(char **buffer)
{
	if(strcmp(buffer[0],"cd")==0)
	{
		
			execute_cd(buffer);
			return 1;
		}

	else if(strcmp(buffer[0],"echo")==0)
	{
		
			execute_echo(buffer);
			return 1;
		}


		else
			return 0;

}

// void printtrie(trie *root)
// {
// int index;
// for(int i=0; i<128; i++)
// { trie *head=root;
// index=i;
// if(head->char_map[index] == NULL)
// {
// index=i;
// char s=index;
// cout<<endl;
// head = head->char_map[index];
// }

// }

// }

int main (int argc,char *argv[])
{
	Raw_Mode();
	
	char *buffer[1000];
	char *pipecommand[1000];
	getcwd(SHELL_DIR,1000);
	Create_Trie( );
	//printtrie(root_t);
	//string m=search(root_t,"song.mp");

	// if(m!="")
	// {
	// 	cout<<m<<"\n";
	// }

	setvariables();
	fill_history();

	
	clearscre();
	while(1)
	{
		

		 if ( signal(SIGINT,sig_handler) == 0 )
            continue;
        if ( signal(SIGQUIT,sig_handler) == 0 )
            continue;
        if ( signal(SIGTSTP,sig_handler) == 0 )
            continue;
		string input="";
		string pr=Print_prompt();
		cout<<pr;


		//PARSING INPUT AND STORING IN BUFFER
		char c=0;
		while((c=getchar()) != '\n')
		{
			if(c==127)
			{
				printf("\r                                                                                                              ");
				//fflush(stdout);
				int le=input.length();
				//cout<<input<<" \n";
				//cout<<le<<"lenght is \n";
				//input[le-1]='\0';
				
				string subin=input.substr(0,le-1);
				int lg=subin.length();
				//cout<<"new lenght "<<lg<<"\n";
				//cout<<subin<<" \n";
				string pq=Print_prompt();
				input=subin;
				subin=pq+subin;
				printf("\r%s",subin.c_str());

				continue;
			}

			if(c==9)
			{
				string check_dir="";
				int le1=input.length();

				for(int k=le1-1;input[k]!=' ';k--)
				{
					check_dir=check_dir+input[k];
				}
				reverse(check_dir.begin(),check_dir.end());
				//cout<<check_dir<<" ";
				string m=search(root_t,check_dir);
				//cout<<m<<"\n";
				printf("\r                                                                                                                                                      ");
				//fflush(stdout);
				//printf("                                                                                                                                                    ");
				//
				int le=input.length();
				
				string pq=Print_prompt();
				
				pq=pq+input+m;
				
				//cout<<pq<<"\n";
				printf("\r%s",pq.c_str());

				input=input+m;
				//cout<<m<<" ";
				// if(m!="")
				// 		{
				// 			printf(m);
				// 		}


				continue;
			}

			if(c==18)
			{
				string m=search(history_t,input);
				string pq=Print_prompt();
				
				pq=pq+input+m;
				
				//cout<<pq<<"\n";
				printf("\r%s",pq.c_str());

				input=input+m;
				continue;
			}
			input=input+c;
		}
		//cout<<input<<" ";
		//getline(cin,input);
		char input2[1000];
		
		History_file(input);
		insert_trie(history_t,input);


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

		if(strcmp(buffer[0],"history")==0)
			{
				Execute_history();
				//cout<<"\n";
				continue;
			}

		if(strcmp(buffer[0],"open")==0)
			{
				parse_open(buffer);
				cout<<"\n";
				continue;
			}

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
