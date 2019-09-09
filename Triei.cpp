#include <bits/stdc++.h>
#include <map>
#include <string>
using namespace std;

struct Trie
{
	struct Trie* char_map[128];
 
	
	int is_end;

	
};
typedef struct Trie trie;

trie* get_node()
{
	trie *node = new Trie();
	for(int i=0;i<128;i++)
		node->char_map[i]=NULL;

	node->is_end=0;
	return node;
}

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
	string f=s;
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
int main()
{
	trie *shell_t= get_node();
	insert_trie(shell_t,"apurvi");
	insert_trie(shell_t,"apurv\\");
	string m=search(shell_t,"apurv");
	if(m!="")
	{
		cout<<"string   "<<m<<"\n";
	}


}