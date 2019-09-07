#ifndef init_shell_h
#define init_shell_h
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
map <string,string> envi;

void setvariables()
{
	FILE *fi;
	struct passwd *pw;
	uid_t uid;

	uid=geteuid();
	pw = getpwuid(uid);

	PATH=getenv("PATH");
	USER=pw->pw_name;
	HOME=pw->pw_dir;
	gethostname(HOSTNAME,1024);
	

	//SETTING PS1 VALUE
	string hostname=string(HOSTNAME);
	string user=string(USER);
	string home=string(HOME);

	envi.insert({"HOME",home});
	envi.insert({"HOSTNAME",hostname});
	envi.insert({"USER",user});



	string ans =user+"@"+hostname+":";

	char *ans1=&*ans.begin();
	strcpy(promptpath,ans1);

	//cout<<"HOME"<<HOME<<"\n";
	fi=fopen("myrc.txt","w");
	if(fi!=NULL)
	{
	fprintf(fi,"HOME=%s\n",HOME);
	fprintf(fi,"USER=%s\n",USER);
	fprintf(fi,"HOSTNAME=%s\n",HOSTNAME);
	}
	fclose(fi);


}
void Print_prompt()
{
	string symbol;
	int flag=0;

	getcwd(currdir,sizeof(currdir));
	char tilthome[1000]="~";
	int i;

	if(strlen(currdir)<strlen(HOME))
		flag=1;
	else
	{
			for(i=0;HOME[i]!='\0';i++)
			{
				if(currdir[i]==HOME[i])
					continue;
				else
				{
					flag=1;
					break;
				}
			}
	}

	if(flag==0)
	{
		int j,k=1;

		for(j=i;currdir[j]!='\0';j++)
			tilthome[k++]=currdir[j];

			tilthome[k]='\0';
			strcpy(currdir,tilthome);
	}
	
	//cout<<tilthome<<"hi";

	string prom=string(promptpath);
	string dir=string(currdir);

	if(strcmp(USER,"root")!=0)
	{
		symbol="$";
		strcpy(PS1,"$");
		envi.insert({"PS1",PS1});

	}

	else
	{
		symbol="#";
		strcpy(PS1,"#");
		envi.insert({"PS1",PS1});

	}


	string prompt=prom+currdir+symbol+" ";
	cout<<prompt;
}
#endif