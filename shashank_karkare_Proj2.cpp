// Including the header files 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

using namespace std;


// Defining the file size and other constants
#define	FILE_SIZE	100
#define TOKEN_SIZE	50
#define READ_SIZE	1
#define SPL_CHAR_NUM	5
#define TERMINAL_NUM	34
#define ERRCODE0	"ERROR CODE 0"
#define ERRCODE1	"ERROR CODE 1"
#define ERRCODE2	"ERROR CODE 2"




// Declaring Global Variables

// Declaring the class stack
class stack{
		private:
		 	char paren[100];
			int  top;
		public:
			stack(void);
			void push(char);
			char pop(void);
			int get_top(void);
	};


// Structure for holding the grammar rules
struct node{
		char token[TOKEN_SIZE];
		int  optional;
		int  zero_or_more;
		struct node* next;
	   };

struct first{
		char token[TOKEN_SIZE];
		struct first* next;
	    };

struct follow{
		char token[TOKEN_SIZE];
		struct follow* next;
	     };

int	error_code = -1;					// Stores the error code if grammmar rules are not properly given
FILE* 	in_fp;							// Input file pointer
FILE* 	out_fp;							// Output file pointer
char*   set[100];
char*	token ;							// Stores token
const char* spl_char[]   = {"{","}","[","]","|"};		// List of all special characters in the grammer
const char* terminals[TERMINAL_NUM] = {"VAR","BEGIN","END","ASSIGN","IF","WHILE","DO","THEN","PRINT","INT","REAL","STRING",
				      "PLUS","MINUS","UNDERSCORE","DIV","MULT","EQUAL","COLON","COMMA","SEMICOLON","LBRAC",
				      "RBRAC","LPAREN","RPAREN","NOTEQUAL","GREATER","LESS","LTEQ","GTEQ","DOT","ID","NUM","REALNUM"};	// List of all Terminals 

vector<struct node*>   rules;		 	// Holds the grammar rules
vector<struct first*>  nt_first;		// Holds the first set of each non terminal
vector<struct follow*> nt_follow;	 	// Holds the follow set of each non terminal



// Declaring all the functions
void create_out_filename(char*,char*);		// Adds .out in place of .in in the input filename
void grammar_input(void);			// Extract grammar rules from the file and stores them  
void grammar_check(void);			// Checks the grammar rules for error
void print_error(const char*);			// Prints the error code int he output file and exit
int check_for_terminal(char*);			// Checks if the given token is a terminal or not
int check_alphabets(char);			// Checks if the given character is in the alphabets
void find_first_and_follow_set(void);
void find_first(struct node*);
void remove_brac_from_token(char*);

// Start of main function
int main(int argc,char** argv)
{

	// If no argument is given or more argument is given the print the error
	if(argc != 2)
	{
		printf("This progran need exactly 1 argument.\n");
		printf("<program name> <test filename>\n");
		exit(1);
	}

	// Declaring all the variables
	char* 	in_filename  = NULL;		// To store input filename
	char* 	out_filename = NULL;		// To store output filename
	char	ch;

	
	// Allocating memory to all pointer variables
	in_filename 	= (char*)malloc(sizeof(char*)* FILE_SIZE);
	out_filename	= (char*)malloc(sizeof(char*)* FILE_SIZE);
	token		= (char*)malloc(sizeof(char*)* TOKEN_SIZE);

	// Copying the argument in in_filename
	strcpy(in_filename,argv[1]);

	//Creating output filename based on input filename
	create_out_filename(in_filename,out_filename);

	// Opening both the files 
	in_fp = fopen(in_filename,"r");
	if(in_fp == NULL)
	{
		fprintf(stderr,"Error opening %s : %s\n",in_filename,strerror(errno));
		free(in_filename);
		free(out_filename);
		exit(1);
	}

	out_fp = fopen(out_filename,"w");
	if(out_fp == NULL)
	{
		fprintf(stderr,"Error opening %s : %s\n",out_filename,strerror(errno));
		free(in_filename);
		free(out_filename);
		fclose(in_fp);
		exit(1);
	}

	// Calling the lexics function to do the rest of the processing
	grammar_input();

	// Managing the grammar rules
//	manage_grammar_rules();

	//Finding the first set
//	find_first_and_follow_set();


	// Freeing allocated memory 
	free(in_filename);
	free(out_filename);
	fclose(in_fp);
	fclose(out_fp);
	return 0;
}



//Defining the functions
void create_out_filename(char* in_filename,char* out_filename)
{
	// Declaring Variables
	int i;
	int len;
	char* temp;
	temp = (char*)malloc(sizeof(char*)* FILE_SIZE);

	len = strlen(in_filename);
	strcpy(temp,in_filename);

	// Removing .in part of the filename
	for(i = len-1; temp[i] != '.' ;i--)
	strncpy(temp,temp,i-1);
	temp[i] = '\0';

	// Concatinating it with .out and storing the result in out_filename
	strcat(temp,".out");
	strcpy(out_filename,temp);

	// Deallocating the memory
	free(temp);

}


// Defining the function grammar_input()
void grammar_input(void)
{
	int j,k;
	int opt_lvl , zer_lvl;
	struct node* temp = NULL;
	struct node* ptr_to_end = NULL;
	char*  temp_token;
	char   ch;
	int    token_end = 0;
	int    token_num = 0;
	vector<struct node*>::iterator i;

	temp_token = (char*)malloc(sizeof(char)*100);
	ch = fgetc(in_fp);

	while(!feof(in_fp))
	{
		while(ch == ' ' || ch == '\t')
			ch = fgetc(in_fp);
		
		token_end = 0;
		while(ch != ' ' && ch != '\t' && ch != '\n')
		{
			temp_token[token_end++] = ch;
			ch = fgetc(in_fp);
		}
	
		temp_token[token_end] = '\0';

		if(temp_token[0] != '\0')
		{
		
			token_num++;
	
			temp = (struct node*)malloc(sizeof(struct node));
			strcpy(temp->token,temp_token);
			temp->optional = 0;
			temp->zero_or_more = 0;
			temp->next = NULL;

			if(token_num == 1)
			{
				rules.push_back(temp);
				ptr_to_end = temp;
			}
			else
			{
				ptr_to_end->next = temp;
				ptr_to_end = temp;
			}
		}
		
		if(ch == '\n')
			token_num = 0;	
		
		token_end = 0;
		ch = fgetc(in_fp);
	}

	// Checking the grammmar rules for the error
	grammar_check();

	// Formatting the grammar filling the optional and zero_or_more data and removing the paren
	stack opt;
	stack zer;
	for(i = rules.begin();i != rules.end();i++)
	{
		zer_lvl = 0;
		opt_lvl = 0;
		temp = *i;
		while(temp != NULL)
		{	
			k = strlen(temp->token);
			for(j = 0; temp->token[j] != '\0' ;j++)
			{
				if(temp->token[j] == '{')
				{
					zer.push(temp->token[j]);
					zer_lvl = zer.get_top() + 1;
				}
				else if(temp->token[j] == '[')
				{
					opt.push(temp->token[j]);
					opt_lvl = opt.get_top() + 1;
				}
				else if(temp->token[j] == '}')
				{
					ch = zer.pop();
					zer_lvl = zer.get_top() + 1;
				}
				else if( temp->token[j] == ']')
				{
					ch = opt.pop();
					opt_lvl = opt.get_top() + 1;
				}
				else
				{
					temp->optional = opt_lvl;	
					temp->zero_or_more = zer_lvl;
				}
			}			
			remove_brac_from_token(temp->token);
			temp = temp->next;
			
		}
	}	

/*	// Printing the grammar rules
        temp = NULL;
        ptr_to_end = NULL;
        for(i = rules.begin(); i != rules.end();i++)
        {
                temp = *i;
                ptr_to_end = temp;
                while(ptr_to_end != NULL)
                {
                        printf("%s%d%d \t",ptr_to_end->token,ptr_to_end->optional,ptr_to_end->zero_or_more);
                        ptr_to_end = ptr_to_end->next;
                }
                printf("\n");
        }
	
*/

	find_first_and_follow_set();
}






// Defining the function print_error()
void print_error(const char* errcode)
{
	fprintf(out_fp,"%s\n",errcode);
	exit(0);
}

//Defining the function check_for_teminals
int check_for_terminals(char* token)
{
	int i;	
	for(i = 0 ; i < TERMINAL_NUM;i++)
		if(strcmp(token,terminals[i]) == 0)
			return 1;
	
	return 0;
}


// Defining the function check_for_special_char()
int check_for_special_char(char* token)
{
	int i;
	if(strcmp(token,"-") == 0)
		return 2;
	for(i=0; i < SPL_CHAR_NUM;i++)
		if(strcmp(token,spl_char[i]) == 0)
			return 1;

	return 0;
}


// Defining the function check_for_non_terminal()
int check_for_non_terminal(char* token)
{
	int i;
	int len = strlen(token);

	for(i = 0 ;i < len;i++)
	{
		if(i == 0 && !((token[i] >= 'a' && token[i] <='z') || (token[i] >= 'A' && token[i] <= 'Z')))
			return 0;
		if(i > 0 && !((token[i] >= 'a' && token[i] <= 'z') || (token[i] >= 'A' && token[i] <= 'Z') || (token[i] >= '0' && token[i] <= '9')))
			return 0;
	}

	return 1;
}


// Defining the function grammar_check()
void grammar_check()
{
	stack s;
	int k,len;
	char t1;
	vector<struct node*>::iterator i;
	vector<struct node*>::iterator j;
	struct node* temp;
	char ch_start , ch_end;


	// Check if grammar rules are made up of alphabets given
	for(i = rules.begin(); i != rules.end();i++)
	{
		temp = *i;
		while(temp != NULL)
		{			
			len = strlen(temp->token);
			for(k = 0;k < len;k++)
				if(check_alphabets(temp->token[k]) == 0)
					print_error(ERRCODE0);
			temp = temp->next;
		}

	}


	// Check if the rules is formatted correctly
	for( i = rules.begin(); i != rules.end();i++)
	{
		temp = *i;
		temp = temp->next;
		if(strcmp(temp->token,"-") != 0)
			print_error(ERRCODE0);
	}


	// Check if the left hand side is a valid non-terminal
	for(i = rules.begin(); i != rules.end();i++)
	{ 
		temp = *i;
		if(check_for_non_terminal(temp->token) == 1)
		{
			if(check_for_terminals(temp->token) == 1)
				error_code = 2;
		}
		else
			print_error(ERRCODE0);
	}


	// Check for the completness of the parenthesis
	for(i = rules.begin();i != rules.end();i++)
	{
		temp = *i;
		temp = temp->next->next;
		while(temp != NULL)
		{
			len = strlen(temp->token);
			for(k=0;k < len;k++)
			{
				if(temp->token[k] == '{' || temp->token[k] == '[')
				{
					s.push(temp->token[k]);
//					printf("%c -- ",temp->token[k]);
				}
				if(temp->token[k] == '}' || temp->token[k] == ']')
				{
					t1 = s.pop();					
//					printf("%c ++ ",t1);
					if(t1 == '*')
						print_error(ERRCODE0);
					if(temp->token[k] == '}')
					{
						if(t1 == '[')
							print_error(ERRCODE0);
					}
					else if(temp->token[k] == ']')
					{
						if(t1 == '{')
							print_error(ERRCODE0);
					}
				
				}	
			}
			
			temp = temp->next;
		}

		
	}


	// Check for the '|' symbol 
	for(i = rules.begin();i != rules.end();i++)
	{
		k = 0;
		temp = *i;
		temp = temp->next->next;
		while(temp != NULL)
		{
			if(strcmp(temp->token,"|") != 0)
			{
				len = strlen(temp->token);
				for(k = 0; k < len ; k++)
				{
					if(temp->token[k] == '|')
						print_error(ERRCODE0);
				}

			}
			
			temp = temp->next;
		}


	}

	if(error_code == 1)
		print_error(ERRCODE1);
	else if(error_code == 2)
		print_error(ERRCODE2);

}



// Defining the function check_alphabets()
int check_alphabets(char ch)
{
	if(ch >= 'a' && ch <= 'z')
		return 1;
	else if(ch >= 'A' && ch <= 'Z')
		return 1;
	else if(ch >= '0' && ch <= '9')
		return 1;
	else if( ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '-' || ch == '|' )
		return 1;

	return 0;

}







// Defining  the stack class function
stack::stack(void)
{
	top = -1;
}


void stack::push(char ch)
{
	paren[++top] = ch;
}

char stack::pop(void)
{
	char ch;

	if(top == -1)
		return '*';

	ch = paren[top--];
	return ch;
}

int stack::get_top(void)
{
	return top;
	
}





// Defining the findfirst_and_follow_set()
void find_first_and_follow_set()
{
	
	int j,k;
	struct node*  temp;
	struct first* temp1;
	struct first* end_ptr;
	vector<struct node*>::iterator i;
	vector<struct first*>::iterator c;	

	for(j =0 ; j < 100 ;j++)
		set[j] = NULL;


	for(i = rules.begin(); i != rules.end() ; i++)
	{
		temp = *i;
		find_first(temp);

		// store first set in vector
		temp1 = (struct first*)malloc(sizeof(struct first));
		strcpy(temp1->token,temp->token);
		temp1->next = NULL;
		nt_first.push_back(temp1);
		end_ptr = temp1;

		for(j=0; set[j] != NULL;j++)
		{
			temp1 = (struct first*)malloc(sizeof(struct first));
			strcpy(temp1->token,set[j]);
	                temp1->next = NULL;
			end_ptr->next = temp1;
			end_ptr  = temp1;
		}

		for(j=0;j < 100 ;j++)
			set[j] = NULL;
	}	
	
	// Printing the value of first set in file
	int count = 0;
        for(c = nt_first.begin();c != nt_first.end();c++)
        {
	        temp1 = *c;
                fprintf(out_fp,"FIRST(%s)  =  {",temp1->token);
        	temp1=temp1->next;
                while(temp1 != NULL)
                {
			fprintf(out_fp,"%s",temp1->token);
			if(temp1->next == NULL)
                		fprintf(out_fp,"",temp1->token);
			else
				fprintf(out_fp,", ",temp1->token);
                        temp1 = temp1->next;
			count++;
                }
		fprintf(out_fp,"}\n");
		count = 0;
	}


//	find_follow();
	for(j=0;j < 100 ;j++)
                        set[j] = NULL;

	for(i = rules.begin();i != rules.end();i++)
	{
		temp = *i;
		find_follow(temp);

	}

}




// Defining the find_first() function
void find_first(struct node* ptr)
{
	int i = 0;
	int prev_opt = 0;
	vector<struct node*>::iterator j;
	struct node* temp = NULL;
	struct node* nxt_nt = NULL;
	struct node* temp2;
	temp = ptr;
	temp = temp->next->next;


	while (temp != NULL)
	{
//		printf("1. %s \n",temp->token);
		while( temp != NULL && (!(strcmp(temp->token,"|") == 0 && temp->optional == 0)))
		{
			
			if(temp->optional == 0)
			{
				if(check_for_terminals(temp->token) == 1 )	
				{
//					printf("ptr 2\n");
					for(i = 0; set[i] != NULL;i++);
//					printf("ptr 3\n");
					set[i] = (char*)malloc(sizeof(char)*100);
//					printf("ptr 4\n");
					strcpy(set[i],temp->token);
//					printf("here !!");

				}				
				else
				{
					for(j = rules.begin();j != rules.end();j++)
					{
						temp2 = *j;
						if( strcmp(temp2->token,temp->token) == 0)
							nxt_nt = temp2;
					}		
					if(nxt_nt == NULL)
						print_error(ERRCODE1);	
					find_first(nxt_nt);
//					printf("%s \n",temp->token);
				}
				while( temp != NULL && (!(strcmp(temp->token,"|") == 0 && temp->optional == 0)))
					temp = temp->next;
			}
			else if( temp->optional != 0)
			{
				if(check_for_terminals(temp->token) == 1)
				{
					for(i = 0; set[i] != NULL;i++);
                                        set[i] = (char*)malloc(sizeof(char)*100);
                                        strcpy(set[i],temp->token);
				}
				else
                                {
                                        for(j = rules.begin();j != rules.end();j++)
                                        {
						temp2 = *j;
                                                if( strcmp(temp2->token,temp->token) == 0)
                                                        nxt_nt = temp2;
                                        }
                                        if(nxt_nt == NULL)
                                                print_error(ERRCODE1);
                                        find_first(nxt_nt);
                                }
				prev_opt = temp->optional;
				temp = temp->next;
			
				while( (temp != NULL) && (!(strcmp(temp->token,"|") == 0 && temp->optional == 0)) )
				{
					if(strcmp(temp->token,"|") != 0 && temp->optional < prev_opt)
					{
						if(check_for_terminals(temp->token) == 1)
		                                {
                		                        for(i = 0; set[i] != NULL;i++);
                                		        set[i] = (char*)malloc(sizeof(char)*100);
		                                        strcpy(set[i],temp->token);
                		                }
                                		else
		                                {
                			                for(j = rules.begin();j != rules.end();j++)
                                       		        {
								temp2 = *j;
		                                                if( strcmp(temp2->token,temp->token) == 0)
                		                                        nxt_nt = temp2;
                                		        }
		                                        if(nxt_nt == NULL)
                		                                print_error(ERRCODE1);
                                		        find_first(nxt_nt);
                                		}
						prev_opt = temp->optional;
					}
					else if(strcmp(temp->token,"|") == 0 && temp->optional == prev_opt)
					{		
						temp = temp->next;
						if(check_for_terminals(temp->token) == 1)
                                                {
                                                        for(i = 0; set[i] != NULL;i++);
                                                        set[i] = (char*)malloc(sizeof(char)*100);
                                                        strcpy(set[i],temp->token);
                                                }
                                                else
                                                {
                                                        for(j = rules.begin();j != rules.end();j++)
                                                        {
								temp2 = *j;
                                                                if( strcmp(temp2->token,temp->token) == 0)
                                                                        nxt_nt = temp2;
                                                        }
                                                        if(nxt_nt == NULL)
                                                                print_error(ERRCODE1);
                                                        find_first(nxt_nt);
                                                }
                                                prev_opt = temp->optional;
					}
					temp = temp->next;
				}
			}
		}
		if(temp != NULL)
			temp = temp->next;
	} 
	
}


// Defining the function remove from token()
void remove_brac_from_token(char* token)
{
        int i,j,len;
        len = strlen(token);

        i=0;
        while((token[i] == '{' || token[i] == '[') && token[i] != '\0')
                i++;
        if(token[i] == '\0')
                return;
        for(j=0;token[i] != '\0';j++)
        {
                token[j] = token[i];
                i = i +1;
        }
        token[j] = '\0';

        j = i =0;
        while((token[i] != '}' && token[i] != ']') && token[i] != '\0')
                i++;
        token[i] = '\0';
}






// Defining the function find_follow()
void find_follow(struct node* ptr)
{




}
