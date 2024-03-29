#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Defining the file size and other constants
#define	FILE_SIZE	100
#define TOKEN_SIZE	30
#define READ_SIZE	1
#define SPL_CHAR_NUM	12
#define KEYWORDS_NUM	9
#define CODE1		"reserved word"
#define CODE2		"integer"
#define CODE3		"float"
#define CODE4		"math operator"
#define CODE5		"comparision operator"
#define CODE6		"shift operator"
#define CODE7		"grouping operator"
#define CODE8		"identifier"
#define ERRCODE0	"ERROR CODE 0: Lexical error: invalid character found in token"
#define ERRCODE1	"ERROR CODE 1: Lexical error: token not valid in language"


// Declaring Global Variables
FILE* 	in_fp;		// Input file pointer
FILE* 	out_fp;		// Output file pointer
char*	token ;		// Stores token
const char* spl_char[]   = {".","+","-","*","/","=",">","<","{","}","[","]"};		// List of all special characters in the grammer
const char* keywords[KEYWORDS_NUM] = {"begin","end","if","then","for","while","print","int","float"};	// List of all keywords 



// Declaring all the functions
void create_out_filename(char*,char*);		// Adds .out in place of .in in the input filename
void lexical_checking(void);			// Creats token and place them in their respective category 
char read_char_from_file(void);			// Reads a character from the given file and return the character
void output_token(char*);			// Prints the token and its category in the file
void keyword_identifier_check(char);		// Checks if the token with the given char is a keyword or identifier
void int_float_check(char);			// Checks if the token with the given char is a integer or float
void special_char_check(char);			// Checks for the special characters
void add_char_to_token(char);			// Adds the new character to the token
int  check_special_token();			// Checks for the token if first letter is special charcter 
void invalid_char_token(char);			// Checks for if token starts with invalid token

// Start of main function
int main(int argc,char** argv)
{

	// If no argument is given or more argument is given the print the error
	if(argc != 2)
	{
		printf("This progran need only 1 argument.\n");
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
		exit(1);
	}

	// Calling the lexics function to do the rest of the processing
	lexical_checking();

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


// Defining the function lexical_checking
void lexical_checking(void)
{
	char ch;

	while(!feof(in_fp))
	{
		ch = read_char_from_file();
		while((ch == ' ' || ch == '\n' || ch == '\t') &&  !feof(in_fp))
			ch = read_char_from_file();

		if(feof(in_fp) || ch == -1)
			break;
		if((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
		{
			keyword_identifier_check(ch);
		}	
		else if(ch >= 48 && ch <= 57)
		{
			int_float_check(ch);
		}
		else if( (ch >= 42 && ch <=47 && ch != 44) || (ch >= 60 && ch <= 62) || (ch == 91 || ch == 93 || ch == 123 || ch == 125) )
		{
			special_char_check(ch);
		}
		else
			invalid_char_token(ch);
	}

}


//Defining the function read_char_from_file
// It reads single character from the input file and returns it
char read_char_from_file(void)
{
	char buf;
	buf = fgetc(in_fp);
	return buf;
}



// Printing the output to the output file
void output_token(char*  msg_code)
{
	fprintf(out_fp,"%s\t%s\n",token,msg_code);
	token[0] = '\0';
}


// Defining the function add_char_to_token
// It adds given character to the token string
void add_char_to_token(char ch)
{
	int i;
	for(i = 0; token[i] != '\0';i++);
	token[i] = ch;
	token[i+1] ='\0';
}


// Defining the function character_check
// It checks for whether the given character is a number,
// an alphabet , an special character or a character not defined 
// in the alphabet of the language
int character_check(char ch)
{
	int i;
	if(( ch >= 48 && ch <= 57))						// If number
		return 2;
	else if ((ch >= 65 && ch <= 90) || (ch >= 97  &&  ch <= 122  ))		// If alphabets
		return 1;
	else				 					// For special character
	{
		for(i = 0;i < SPL_CHAR_NUM;i++)
			if(spl_char[i][0] == ch)
				return 3;
		return 4; 
	}


}

// Defining the function check_for_keyword
// This function check whether the token is a match to all
// given keywords of the language 
int check_for_keyword()
{
	int i = 0;
	for(i = 0; i < KEYWORDS_NUM;i++)
	{
		if(strcmp(token,keywords[i]) == 0)
			return 1;
	}
	return 0;

}



// Defining the function keyword_identifier_check
// This function checks and builds a token if the first character of the 
// token starts with an alphabet
void keyword_identifier_check(char ch)
{
	int  err_code = -1;
	char new_ch;
	int  alpha_type;
	add_char_to_token(ch);

	while( ch != ' ' && ch != '\n' && ch != '\t')
	{
		new_ch = read_char_from_file();

		if(new_ch == ' ' || new_ch == '\n' || new_ch == '\t')
		{
			if(check_for_keyword() == 1)
				output_token(CODE1);
			else if(err_code == -1)
				output_token(CODE8);
			else
			{
				if(err_code == 0)
					output_token(ERRCODE0);
				else
					output_token(ERRCODE1);
			}
		}	
		else
		{
			alpha_type = character_check(new_ch);
			switch(alpha_type)
			{
				case 1:						// For alphabet
					add_char_to_token(new_ch);
					break;
				case 2:						// For number
					add_char_to_token(new_ch);		
					break;
				case 3:						// For special Character
					add_char_to_token(new_ch);
					if(err_code != 0)
						err_code = 1;				
					break;		
				case 4:						// For invalid character
					add_char_to_token(new_ch);
					err_code = 0;
					break;
			};

		}
		ch  = new_ch;
	}


}


// Defining the function int_float_check
// This function checks and build token if the first
// character of the token is a number
void int_float_check(char ch)
{
	int dot_count = 0;
	int err_code = -1;
	int int_or_float = -1;	
	char new_ch;
	int alpha_type;

	add_char_to_token(ch);

	while(ch != ' ' && ch != '\n' && ch != '\t')
	{
		new_ch = read_char_from_file();
		if(new_ch == ' ' || new_ch == '\n' || new_ch == '\t')
		{
			if(err_code == -1)
			{
				if(int_or_float == 0)			// For float
					output_token(CODE3);
				else					// For int
					output_token(CODE2);		
			}
			else
			{
				if(err_code == 0)
					output_token(ERRCODE0);
				else
					output_token(ERRCODE1);
			}
		}
		else
		{
			alpha_type = character_check(new_ch);
                        switch(alpha_type)
                        {
                                case 1:                                         // For alphabet
                                        add_char_to_token(new_ch);
					err_code = 1;
                                        break;
                                case 2:                                         // For number
                                        add_char_to_token(new_ch);
					if(ch == '.' && dot_count == 1)
					{
						int_or_float = 0;
						err_code = -1;
					}
					else if(dot_count > 1)
						err_code = 1;
					if(int_or_float != 0)
						int_or_float = 1;
                                        break;
                                case 3:                                         // For special Character
                                        add_char_to_token(new_ch);
					if(new_ch == '.')
					{
						dot_count++;
						err_code  = 1;
					}
					else
	                                       if(err_code != 0)
        	                                       err_code = 1;
                                        break;
                                case 4:                                         // For invalid character
                                        add_char_to_token(new_ch);
                                        err_code = 0;
                                        break;
                        };
		
		}
		ch = new_ch;

	}
}


// Defining the function check_special_token()
// This function classifies the given special character
// into different kinds defined in the alphabets of langauge
int check_special_token()
{
	int i;
	char* grouping_token[] = {"{}","[]"};
	char* shift_token[]    = {"<<",">>"};

	for(i = 0; i < SPL_CHAR_NUM;i++)
	{
		if(strcmp(token,spl_char[i]) == 0)
		{
			if( i >= 0 && i <= 5)
				return 4;
			else if(i ==6 || i == 7)
				return 5;
			else if(i >= 8 && i<= 11)
				return 7;
		}
	}

	if(strcmp(token,grouping_token[0]) == 0  || strcmp(token,grouping_token[1]) == 0)
			return 7;
	if(strcmp(token,shift_token[0]) == 0  || strcmp(token,shift_token[1]) == 0)
			return 6;


	return 0;
}



// Defining the function special_char_check 
// This function checks and builds the token
// if the first character of the token is a special character 
// defined in the alphabet of the language
void special_char_check(char ch)
{
	int 	err_code = -1;
	char	new_ch;
	int 	alpha_type;
	int 	result ;
	int 	float_chk = 0;
	add_char_to_token(ch);


	if(ch == '.')
	{
		while(ch != ' ' && ch != '\n' && ch != '\t')
		{
			new_ch = read_char_from_file();
			if(new_ch == ' ' || new_ch == '\n' || new_ch == '\t')
			{
				if(err_code == -1)
				{
					if(float_chk == 1)
						output_token(CODE3);
					else
						output_token(CODE4);
				}
				else
				{	
					if(err_code == 0)
						output_token(ERRCODE0);
					else
						output_token(ERRCODE1);
				}
			}
			else
			{
				alpha_type = character_check(new_ch);
				switch(alpha_type)
				{
					case 1:			// For alphabet
						add_char_to_token(new_ch);
						if(err_code != 0)
							err_code = 1;
						break;
					case 2:			// For number
						add_char_to_token(new_ch);
						float_chk = 1;
						break;
					case 3:			// For special character	
						add_char_to_token(new_ch);
						if(err_code != 0)
							err_code = 1;
						break;
					case 4:			// Unknown character
						add_char_to_token(new_ch);
						err_code = 0;
						break;
				};


			}
			ch = new_ch;
		}

	}
	else
	{
		while( ch != ' ' && ch != '\n' && ch != '\t')
		{
			new_ch = read_char_from_file();		
			if(new_ch == ' ' || new_ch == '\n' || new_ch == '\t')
			{
				if(err_code == -1)
				{
					result = check_special_token();
					switch(result)
					{
						case 0:
							output_token(ERRCODE1);
							break;
						case 4:
							output_token(CODE4);
							break;
						case 5:
							output_token(CODE5);
							break;
						case 6:
							output_token(CODE6);
							break;
						case 7:
							output_token(CODE7);
							break;
					};		
				}
				else
				{
					if(err_code == 0)
						output_token(ERRCODE0);
					else
						output_token(ERRCODE1);
				}
			}
			else
			{
				alpha_type = character_check(new_ch);
				switch(alpha_type)
				{
					case 1:				// For alphabet
					case 2:				// For number
						add_char_to_token(new_ch);	
						if(err_code != 0)
							err_code = 1;
						break;
					case 3:				// For special Character
						add_char_to_token(new_ch);
						break;
					case 4:				// For unknown character
						add_char_to_token(new_ch);
						err_code = 0;
						break;
				};
			}
			ch = new_ch;
		}
	}
}




// Defining function invalid_char_token()
// This function takes care if the first character of the 
// token starts with invalid character .
void invalid_char_token(char ch)
{
	char new_ch;
	add_char_to_token(ch);
	while(ch != ' ' && ch != '\t' && ch != '\n')
	{
		new_ch = read_char_from_file();
		if( new_ch == ' ' || new_ch == '\n' || new_ch == '\t')
		{
			output_token(ERRCODE0);
		}
		else
		{
			add_char_to_token(new_ch);
		}
		ch = new_ch;
	}


}
