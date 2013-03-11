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

// Declaring the global variables
#define FILE_SIZE   100
#define TOKEN_SIZE  50
#define TYPES_NUM   4
#define ERRCODE1    "ERROR 1: Undeclared identifier in expression"	
#define ERRCODE2    "ERROR 2: Type mismatch in expression"
#define ERRCODE3    "ERROR 3: Type mismatch in assignment"

struct node{
		char name[75];			// Stores variable name
		char type[10];			// Stores the type of name
		int  array;			// 1 - if array else 0
		int  array_size;		// Stores size of array else -1
	   };
	
struct assign{
		char token[75];
		struct assign* next;
	     };

FILE* in_fp;
vector<struct node*>declarations;
struct node* ptr;
char*  types[] = {"bool","int","double","string"};


/*class stack{
	private:
		char*  array[100];
		int top = -1;
	public:
		void  push(char*);
		void  pop(char*);
		      stack();
	   }; 

*/


// Declaring the functions
void store_declarations(void);
void manage_array_variable(void);
int  check_type(struct node*);
void type_checking(void);
//void print_error(char*);
int operator_priority(char*);


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
	vector<struct node*>::iterator i;
	struct node* temp;

	
	// Allocating memory to all pointer variables
	in_filename 	= (char*)malloc(sizeof(char*)* FILE_SIZE);

	// Copying the argument in in_filename
	strcpy(in_filename,argv[1]);


	// Opening the input file to read from it
	in_fp = fopen(in_filename,"r");
	if(in_fp == NULL)
	{
		fprintf(stderr,"Error opening %s : %s\n",in_filename,strerror(errno));
		free(in_filename);
		exit(1);
	}


	// Storing the declarations provided in the file
	store_declarations(); 

	// manage the array variables
	manage_array_variable();

	// type_checking
	type_checking();

	// Printing the declarations
	for(i = declarations.begin(); i != declarations.end();i++)
	{
		temp = *i;
		printf("%s -- %s -- %d -- %d \n",temp->name,temp->type,temp->array,temp->array_size);

	}


	// Freeing allocated memory 
	free(in_filename);
	fclose(in_fp);
	return 0;
}




// Defining the function store_declarations()
void store_declarations()
{
	char ch;
	int  char_num = 0;
	int  token_end = 0;
	int  token_num = 0;
	char temp_token[TOKEN_SIZE];
	
	while(!feof(in_fp))
	{
		char_num = 0;
		ch = getc(in_fp);
	
		if(char_num == 0 && ch == '\n')
			break;
		else
		{
			ptr = (struct node*)malloc(sizeof(struct node));
			ptr->array = 0;
			ptr->array_size = 0;
			while(ch != '\n')
			{
				char_num++;
				while(ch == ' ' || ch == '\t')
				{
					ch = getc(in_fp);
					char_num++;
				}

				while(ch != ' ' && ch != '\t' && ch  != '\n')
				{
					temp_token[token_end] = ch;
					ch = getc(in_fp);
					token_end++;
					char_num++;
				}
				temp_token[token_end] = '\0';

				if(token_num == 0 )
					strcpy(ptr->type,temp_token);	
				else
					strcpy(ptr->name,temp_token);
				token_num++;
				token_end = 0;
			}
			declarations.push_back(ptr);
			token_num = 0;
		}


	}
}




// Defining the function check_type()
int check_type(struct node* token)
{
	int i;
	int first_paran = 0;
	int len;
	int size = 0;
	char ch_size[10];
	for(i = 0; i < TYPES_NUM; i++)
	{
		if(strcmp(types[i],token->type) == 0)
			return 1;
	}
	
	len = strlen(token->type);
	for(first_paran = 0 ; (token->type)[first_paran] != '[' && first_paran < len ; first_paran++);	
	for(i = 0;i < TYPES_NUM;i++)
	{
		if(strncmp(types[i],token->type,first_paran) == 0)	
		{
			strcpy(ch_size,(token->type)+first_paran+1);
			ch_size[strlen(ch_size)-1] = '\0';
			size = atoi(ch_size);
			token->array = 1;
			token->array_size = size;
			token->type[first_paran] = '\0';
			return 2;
		}
	}
	
	return 0;
}





// Defining the function manage_array_variable
void manage_array_variable()
{
	vector<struct node*>::iterator i;
	struct node* temp;
	int j;


	for(i = declarations.begin(); i != declarations.end();i++)
	{
		temp = *i;
		j = check_type(temp);		
	}
}



int operator_priority(char* token)
{
	if(strcmp(token,"[]") == 0)
		return 1;
	else if(strcmp(token,"*") == 0 || strcmp(token,"/") == 0)
		return 2;
	else if(strcmp(token,"+") == 0 || strcmp(token,"-") == 0)
		return 3;
	else if(strcmp(token,"<") == 0 || strcmp(token,">") == 0 || strcmp(token,"<=") == 0 || strcmp(token,">=") == 0 || strcmp(token,"==") == 0)
		return 4;
	else if(strcmp(token,"=") == 0)
		return 5;
}



//Defining the function type_checking()
void type_checking()
{
	printf("Entering type_checking function! \n");
	char ch;
	size_t n,t;
	int  pos = 0;
	int  temp_pos = 0;
	int  temp_pos2 = 0;
        int  token_end = 0;
        int  token_num = 0;
	int  flag = 0;
	int  j = 0;
	char index[20];
	char* temp_line;
        char temp_token[TOKEN_SIZE];
	vector<struct node*>::iterator i;
	struct assign* temp;
	struct assign* head;
	struct assign* prev_ptr;
//	stack operators;
//	stack variables;

	temp_line = (char*)malloc(sizeof(char)*100);


	while(!feof(in_fp))
	{
		temp = head = prev_ptr = NULL;
		t = getline(&temp_line,&n,in_fp);
	//	printf("%s",temp_line);
		pos = 0;
		token_num = 0;
		while(temp_line[pos] != '\n')
		{
			token_end = 0;
			while(temp_line[pos] ==  ' ' || temp_line[pos] == '\t')
				pos++;
			while(temp_line[pos] != ' ' && temp_line[pos] != '\t' && temp_line[pos] != '\n')
			{
				temp_token[token_end] = temp_line[pos];
				token_end++;
				pos++;
			}
			temp_token[token_end] = '\0';
		
			temp = (struct assign*)malloc(sizeof(struct assign*));
			temp->next = NULL;
			strcpy(temp->token,temp_token);
		
			if(token_num == 0)
			{
				head = temp;
				prev_ptr = temp;
			}
			else
			{
				prev_ptr->next = temp;
				prev_ptr       = temp;
			}
			token_num++;
		}


		// Setting the tokens to proper state
		temp = prev_ptr = NULL;
		temp = head;
		pos = 0;
		temp_pos = 0;
		while(temp != NULL)
		{
			flag = 0;
	//		printf("%s\n",temp->token);
			pos = 0;
			temp_pos = 0;
			while(temp->token[pos] != '\0' && temp->token[pos] != '[')
				pos++;
			temp_pos2 = pos;
			if(pos < strlen(temp->token) )
			{
				prev_ptr = (struct assign*)malloc(sizeof(struct assign));
				prev_ptr->next = NULL;
				strcpy(prev_ptr->token,"[]");
				prev_ptr->next = temp->next;
				temp->next = prev_ptr;

				prev_ptr = (struct assign*)malloc(sizeof(struct assign));
				prev_ptr->next = NULL;
				for(j = pos+1; temp->token[j] != ']';j++)
					index[temp_pos++] = temp->token[j];
				index[temp_pos] = '\0';
	//			printf("index = %s\n",index);
				strcpy(prev_ptr->token,index);
				prev_ptr->next = temp->next->next;
				temp->next->next = prev_ptr;
				temp->token[temp_pos2] = '\0';


				temp = temp->next->next->next;
				flag = 1;
			}


			if(flag == 0)
				temp = temp->next;

			
		}


		// Printing the assignment
		temp = head;
		while(temp != NULL)
		{
			printf("%s ",temp->token);
			temp= temp->next;
		}
		printf("\n");


		// freeing the memory linklist
		temp = head;
		while(temp != NULL)
		{
			prev_ptr = temp->next;
			free(temp);
			temp = prev_ptr;
		}
		head = NULL;
	}

}
