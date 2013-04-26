#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

// Declaring global variable
string token;
unordered_map<std::string,int> symbol_table;
vector<std::string> prog;
vector<std::string>::iterator curr_token;
int flag = 0;


//Declaring the functions
void take_input();
void read_token();

void program();
void declarations();
void statements();
void statement();

void assign();
void IF();
void WHILE();
void print();

int  expression();
bool Conditional();
int  value();

int  op();
int  compare();
void identifier();
int  number();


int main(int argc,char** argv)
{

	vector<std::string>::iterator it;

	take_input();
	curr_token = prog.begin();


	program();
	
	return 0;
}




// Defining the function take_input()
void take_input()
{
	string a;
	string b = ";";

	while(!feof(stdin))
	{
		cin>>a;
		if(feof(stdin))
			break;
		if(a[a.length() - 1] == ';')
		{
			a.pop_back();
			prog.push_back(a);
			prog.push_back(b);
		}
		else
			prog.push_back(a);
	}
}




// Defining the function read_token()
void read_token()
{
	if(curr_token < prog.end())
	{	
		token = *curr_token;
		curr_token++;
	}
}



// Defining the function program()
void program()
{
	read_token();
	if(token == "DECL")
		declarations();

	if(token == "BODY")
		statements();
	
	
}


// Defining the function declarations()
void declarations()
{
	read_token();
	while(token == "VAR")
	{
		identifier();
		symbol_table.insert(unordered_map<std::string,int>::value_type(token,0));
		read_token();		// to remove ";"
		read_token();
	}
}



// Defining the function identifier()
void identifier()
{
	read_token();
}



// Defining the function statements()
void statements()
{
	while(curr_token < prog.end() )
	{
		read_token();
		if(token == "END")
		{break;}
		statement();
		
		
	}
	
}


// Defining the function statement()
void statement()
{
	if(token == "IF")
		IF();
	else if(token == "WHILE")
	{
		//cout<<"here \n";
		WHILE();
	}
	else if(token == "PRINT")
	{
		
		print();
	}
	else
	{
		if(token != "END")	
			assign();
	}
}


// Defining the function print()
void print()
{
	read_token();
	cout<<symbol_table.find(token)->second<<endl;
	read_token();		// Remove ";" amd mark as end of print statement
}



// Defining the function Conditional()
bool Conditional()
{
	int num1 , num2 , comp_op;
	
	num1      = value();
	comp_op   = compare();
	num2      = value();


	switch(comp_op){
		case 1:
			if(num1 < num2)
				return true;
			else
				return false;
		case 2:
			if(num1 > num2)
				return true;
			else
				return false;
		case 3:
			if(num1 >= num2)
				return true;
			else
				return false;
		case 4:
			if(num1 <= num2)
				return true;
			else
				return false;
		case 5:
			if(num1 == num2)
				return true;
			else
				return false;
		};
}





// Defining the function value()
int value()
{
	string iden;
	identifier();
	if(symbol_table.count(token) > 0)
	{
		return symbol_table.find(token)->second;
	}
	else
		return number();
}




// Defining the function compare()
int compare()
{
	read_token();
	if(token == "<")
		return 1;
	else if(token == ">")
		return 2;
	else if(token == ">=")
		return 3;
	else if(token == "<=")
		return 4;
	else if(token == "==")
		return 5;

}



// Defining the function assign()
void assign()
{
	string iden;
	int val;
	iden = token;

	read_token();

	if(token == "=")
		val = expression();


	symbol_table.erase(symbol_table.find(iden));
	symbol_table.insert(unordered_map<std::string,int>::value_type(iden,val));
	
}



// Defining the function number()
int number()
{
	return atoi(token.c_str());

}


// Defining the function expression()
int expression()
{
	int num1;
	int opr;
	int num2;
	num1 = value();	
	read_token();
	if(token == ";")
		return num1;
	else
	{
		curr_token--;
		opr  = op();
		num2 = value();
		read_token();	// to remove ";" at the end of expression
		switch(opr){
			case 6:
				return num1 + num2;
			case 7:
				return num1 - num2;
			case 8:
				return num1 * num2;
			case 9:
				return num1 / num2;
			};
	}
	
}


// Defining the function op()
int op()
{
	read_token();
	if(token == "+")
		return 6;
	else if(token == "-")
		return 7;
	else if(token == "*")
		return 8;
	else if(token == "/")
		return 9;
}


// Defining the function IF()
void IF()
{
	bool stat;
	stat = Conditional();

	int count = 1;

	if(stat == true)
	{
		read_token();
		if(token == "THEN")
			statements();
	}
	else
	{
				
		do
		{	read_token();
			if(token == "IF" || token == "WHILE")
				count++;
			if(token == "END")
				count--;
			
		}while(!(token == "END" && count == 0));
	}
	

}


// Defining the function WHILE()
void WHILE()
{
	vector<std::string>::iterator temp,temp1;
	temp = curr_token;
	bool stat;
	stat = Conditional();
	int count = 1;
	

	if(stat == true)
	{
		while(stat == true)
		{
			read_token();
			if(token == "DO")
				statements();
		
			temp1 = curr_token;
			curr_token = temp-1;
			read_token(); 
		
			stat = Conditional();
		}
		curr_token = temp1 -1;
		read_token();
		
	}
	else
	{
		do
		{	read_token();
			if(token == "IF" || token == "WHILE")
				count++;
			if(token == "END")
				count--;
			
		}while(!(token == "END" && count == 0));
	}
}
