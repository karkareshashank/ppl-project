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


// Defining the structures
struct node{
		char name[75];					// Stores variable name
		char type[10];					// Stores the type of name
		int  array;					// 1 - if array else 0
		int  array_size;				// Stores size of array else -1
	   };
	
struct assign{
		char token[75];
		struct assign* next;
	     };



FILE* in_fp;							// Input File pointer
vector<struct node*>declarations;				// Stores the declarations 
struct node* ptr;				
struct assign* head;
const char*  types[] = {"bool","int","double","string"};	// Stores all the types used


// Declaring the functions
void store_declarations(void);					// Stores the declaration in teh vector for future use
void manage_array_variable(void);				// If the declarations has array then properly add the size in the vector
int  check_type(struct node*);					// removes the [] from the array declarations and add the flag in the vector
void type_checking(void);					// Calls type_check_assignment() function on each assignment given 
int type_check_assignment(struct assign*,struct assign*);	// Recursively calculate the return type of the assignment
int operator_priority(char*);					// Return the priority of a particular operator
int type_number(char*);						// Return the number assigned to the type 
int compare_operators(char*);					// Return 1 if the given token is compare operator or not
int math_operators(char*);					// Return 1 if the given token is math operator or not


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
	char* 	in_filename  = NULL;				// To store input filename
	vector<struct node*>::iterator i;			// iterator to travese each value stored in vector
	

	
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
	int j;
	vector<struct node*>::iterator i;
	struct node* temp;
	


	for(i = declarations.begin(); i != declarations.end();i++)
	{
		temp = *i;
		j = check_type(temp);		
	}
}



// This function return the priority of the operator so that the split for the recurrance can be done accordingly
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
	size_t n,t;
	int  pos = 0;
	int  temp_pos = 0;
	int  temp_pos2 = 0;
        int  token_end = 0;
        int  token_num = 0;
	int type = 0;
	int  flag = 0;
	int  j = 0;
	char index[20];
	char* temp_line;
        char temp_token[TOKEN_SIZE];
	vector<struct node*>::iterator i;
	struct assign* temp;
	struct assign* prev_ptr;

	temp_line = (char*)malloc(sizeof(char)*100);


	while(!feof(in_fp))
	{
		temp = head = prev_ptr = NULL;
		t = getline(&temp_line,&n,in_fp);
		if(feof(in_fp))
			break;
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
		
			temp = (struct assign*)malloc(sizeof(struct assign));
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


		temp = head;
		while(temp->next != NULL)
			temp = temp->next;

		// Calculating the return type of the assignment statement
		type = type_check_assignment(head,temp);

		switch(type%100){
				case 14:
					printf("Array of string\n");break;
				case 13:
					printf("Array of double\n");break;
				case 12:
					printf("Array of int\n");break;
				case 11:
					printf("Array of bool\n");break;
				case 4:
					printf("string\n");break;
				case 3:
					printf("double\n");break;
				case 2:
					printf("int\n");break;
				case 1:
					printf("bool\n");break;
				case 15:
					printf("%s\n",ERRCODE1);break;
				case 16:
					printf("%s\n",ERRCODE2);break;
				case 17:
					printf("%s\n",ERRCODE3);break;
				}

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



// Defining the function isOperator()
int isOperator(char* token)
{
	const char* ops[11] = {"=","+","-","*","/","<",">","<=",">=","==","[]"};
	int i;

	for(i = 0; i < 11;i++)
		if(strcmp(token,ops[i]) == 0)
			return 1;
	return 0;

}


// Defining the function type_number
int type_number(char* type)
{
	if(strcmp(type,"bool") == 0)
		return 1;
	else if(strcmp(type,"int") == 0)
		return 2;
	else if(strcmp(type,"double") == 0)
		return 3;
	else if(strcmp(type,"string") == 0)
		return 4;

}


// Defining the function compare_operators()
int compare_operators(char* token)
{
	if(strcmp(token,">") == 0 || strcmp(token,"<") == 0 ||strcmp(token,">=") == 0 ||strcmp(token,"<=") == 0 ||strcmp(token,"==") == 0 )
		return 1;
	return 0;
}


// Defining the function math_operators()
int math_operators(char* token)
{
	if(strcmp(token,"+") == 0 ||strcmp(token,"-") == 0 ||strcmp(token,"*") == 0 ||strcmp(token,"/") == 0  )
		return 1;
	return 0;

}



// Defining the function type_check_assignment()
int type_check_assignment(struct assign* start,struct assign* end)
{
	struct assign* temp = NULL;
	struct assign* prev_temp = NULL;
	struct assign* split = NULL;
	struct assign* start1 = NULL;
	struct assign* start2 = NULL;
	struct assign* end1 = NULL;
	struct assign* end2 = NULL;
	struct assign* ops = NULL;
	vector<struct node*>::iterator i;
	struct node* node_temp;
	int    max = 0;
	int    count = 0;
	int    type_left = 0;
	int    type_right = 0;
	int    type_num = 0;
	int    temp_num = 0;
	int    arr_size = 0;
	int    l_array_size = 0;
	int    r_array_size = 0;
	temp = start->next;
	start1 = start;
	end2 = end;
	prev_temp = start;


	// Base Case
	if(start == end)
	{
		for(i = declarations.begin(); i !=  declarations.end();i++)
		{
			node_temp = *i;
			if(strcmp(start->token,node_temp->name) == 0)
			{
				type_num = type_number(node_temp->type);	
				temp_num = type_num;
				arr_size = node_temp->array_size;
				if(node_temp->array == 1)
				{
					arr_size = arr_size*100;
					switch (type_num){
								case 1:
									temp_num = arr_size + 11;break;
								case 2:
									temp_num = arr_size + 12;break;
								case 3:
									temp_num = arr_size + 13;break;
								case 4:
									temp_num = arr_size + 14;break;
							}

				}
				type_num = temp_num;
				return type_num;					// return proper identifier type
			}

		}
		for(count = 0 ; count < strlen(start->token);count++)
		{
			if(start->token[count]  >= '0' && start->token[count] <= '9')
				continue;
			else
				return 15;
		}
		return 2;
	}


	while(temp != end)
	{
		if(isOperator(temp->token) == 1)
		{
			if( operator_priority(temp->token) > max)
			{
				max = operator_priority(temp->token);
				split  = prev_temp;
			}
		}

		temp = temp->next;
		prev_temp= prev_temp->next;
	}
	end1 = split;
	ops  = split->next;
	start2 = split->next->next;



	// Divide	
	type_left  = type_check_assignment(start1,end1);
	type_right = type_check_assignment(start2,end2);
	


	l_array_size = type_left/100;
        type_left = type_left%100;
        r_array_size = type_right/100;
        type_right = type_right%100;
	
	// Conquer
	if(type_left == 15 || type_right == 15)
		return 15;
	if(type_left == 16 || type_right == 16)
		return 16;
	if(type_left == 17 || type_right == 17)
		return 17;
	
	

	if(strcmp(ops->token,"=") == 0)
	{
		if(type_left == type_right)
		{
			if(l_array_size == r_array_size)
                                return type_left;
                        else
                                return 17;
		}
		else if((type_left == 3 && type_right == 2))
			return type_left;
		else if( (type_left == 13 && type_right == 12))
		{
			if(l_array_size == r_array_size)
				return 13;
			else
				return 17;
		}
		else
			return 17;
	}
	else if(strcmp(ops->token,"[]") == 0)
	{
		if(type_left < 10)
			return 15;
		if(type_right != 2)
			return 15;
		else
		{
			switch(type_left){
					case 11:
						return 1;
					case 12:
						return 2;
					case 13:	
						return 3;
					case 14:
						return 4;
				}			
		}
	}
	else if(compare_operators(ops->token) == 1)
	{
		if(type_right == type_left)
			return 1;
		else if((type_right == 2 && type_left == 3)   || (type_right == 3 && type_left == 2))
			return 1;
		else if ((type_right == 13 && type_left == 12) || (type_right == 12 && type_left == 13))
		{
			if(l_array_size == r_array_size)
                                return 1;
                        else
                                return 16;
		}
		else
			return 16;
	}
	else if(math_operators(ops->token) == 1)
	{
		if(type_right == type_left)
		{
			if(l_array_size == r_array_size)
				return type_right + r_array_size*100;
			else
				return 16;
		}
		else if((type_right == 2 && type_left == 3)   || (type_right == 3 && type_left == 2))
			return 3;
		else if((type_right == 13 && type_left == 12) || (type_right == 12 && type_left == 13))
		{
			if(l_array_size == r_array_size)
                                return r_array_size*100 + 13;
                        else
                                return 16;
		}
		else
			return 16;
	}
}
