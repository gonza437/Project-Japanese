//=================================================                  
// File parser.cpp written by Group Number: 11
//=================================================
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

//---Gobal Varibles-----------------------------------------------
string verb[4] = {"masu", "masen", "mashita", "masendeshita"};          //Reserved Words
string connector[4] = {"mata", "soshite", "shikashi", "dakara"};
string object[5] = {"desu", "deshita", "o", "wa", "ni"};
string pronoun[5] = {"watashi", "anata", "kare", "kanojo", "sore"};

enum token_type {ERROR, WORD1, WORD2, PERIOD, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR};
string token[15] = {"ERROR", "WORD1", "WORD2", "PERIOD", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG","IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR"};
token_type saved_lexeme;
bool saved_token;
string print_word;
ifstream fin;

//---Prototypes list-----------------------------------
int scanner(token_type& the_type, string& w);
bool reservedWord(string x, token_type& the_type);
bool word(string s, int& which);
bool period(string a);

void syntax_error1(token_type c);    //Syntax error in treminal 
void syntax_error2(string where);    //Syntax error in Non terminal
bool match(token_type expected);     //Match incomeing token with expected token
token_type next_token();             //Check if next token exist
void stroy();                        //Rule 1.<story> ::= <s1> {<s1>}
void s1();                           //Rule 2.<s1> ::= [CONNECTOR] < noun> SUBJECT <s2>
void s2();                           //Rule 3.<s2>::= <verb> <tense> PERIOD | <noun> <s3>
void s3();                           //Rule 4.<s3>::= <be> PERIOD | DESTINATION <verb> <tense> PERIOD | OBJECT [ <noun> DESTINATION] <verb> <tense> PERIOD
void Tnoun();                         //Rules 5.<noun>::= WORD1 | PRONOUN
void Tverb();                         //Rule 7.<be>::= IS | WAS
void Tbe();                           //Rule 8.<tense>:= VERBPAST | VERBPASTNEG | VERB | VERBNEG
void Ttense();

// The test driver to start the parser
int main()
{
  string file_name;
  
  cout << "Enter the file name:\t";
  getline(cin, file_name);
  
  fin.open(file_name.c_str());
  
  if(!fin)
    {
      cout << "ERROR: File dosen't exist" << endl;
      exit(1);
    }

  stroy();  //Begin parser

  fin.close();
}// end

//====================================================================================
//PURPOSE: Match incomeing token with expected token
//PARAM: expected token
//ALGORITHM: if incoming token is expected cout match
//     else sytax error exit program
bool match(token_type expected)
{
  if(next_token() != expected)
    {
      syntax_error1(expected);      
      return false;
    }
  else 
    {
      cout << "Matched lexeme " << print_word << " to token " << token[expected] << endl << endl;
      saved_token = false;
      return true;
    }
}

//PURPOSE: Check if next token exist 
//ALGORITHM: if token_avaible then call scanner
token_type next_token()
{
  string lexeme;
  if(!saved_token)
    {
      scanner(saved_lexeme, lexeme);
      saved_token = true;
    }

  return saved_lexeme;
}

//===========================================================================
//PURPOSE: Syntax error in terminal                                                 
//PARAM: Token                              
//ALGORITHM: cout token type and print the lexeme
void syntax_error1(token_type c)
{
  cout << "SYNTAX ERROR: expected " << token[c] << " but found " << print_word  << endl;
  exit(1);
}

//PURPOSE: Syntax error in Non terminal            
//PARAM: string name of non termianl                  
//ALGORITHM: cout lexeme and rule
void syntax_error2(string where)
{
  cout << "SYNTAX ERROR: unexpected " << print_word << " found in function " << where << endl;
  exit(1);
}
//=====================================================================================


//===================================================================================
//PURPOSE: Show rule 1. <story> ::= <s1> {<s1>}  
//ALGORITHM: do while loop
void stroy()
{
  bool Ts1;
  do
    {
      cout << "Processing <story>" << endl << endl;;
      s1();
      cout << "------------END----------------" << endl <<  endl << endl;;
      Ts1 = ((next_token() == CONNECTOR) || (next_token() == WORD1) || (next_token() == PRONOUN));

    }while(Ts1);
}

//PURPOSE: show rule 2. <s1> ::= [CONNECTOR] < noun> SUBJECT <s2> 
//ALGORITHM: if statment for [] optional 
//      call functions
void s1()
{
  cout << "========== Processing <s1> ==========" << endl;

  if(next_token() == CONNECTOR)
    match(CONNECTOR);

  Tnoun();
  
  match(SUBJECT);
  
  s2();
}

//PURPOSE: Show rule 3.<s2>::= <verb> <tense> PERIOD | <noun> <s3>
//ALGORITHM: switch statemt
//     call functions for specific case
void s2()
{
  cout << "Processing <s2> " << endl;

  switch(next_token())
    {
    case WORD2:
      Tverb();
      Ttense();
      match(PERIOD);
      break;
      
    case WORD1:
    case PRONOUN:
      Tnoun();
      s3();
      break;
      
    default:
      syntax_error2("<s2>" );
    }
}

//PURPOSE:  Show rule 4.<s3>::= <be> PERIOD | DESTINATION <verb> <tense> PERIOD | OBJECT [ <noun> DESTINATION] <verb> <tense> PERIOD
//ALGORITHM: switch statment for incomeing token 
//    call functions for specific case   
void s3()
{
  cout << "Processing <s3>" << endl;
  switch(next_token())
    {
    case IS :
    case WAS :
      Tbe();
      match(PERIOD);
      break;
      
    case DESTINATION:
      match(DESTINATION);
      Tverb();
      Ttense();
      match(PERIOD);
      break;
      
    case OBJECT:
      match(OBJECT);

      if(next_token() == WORD1 || next_token() == PRONOUN)
	{
	 Tnoun();
	  match(DESTINATION);
	}
      
      Tverb();
      Ttense();
      match(PERIOD);
      break;

    default:
      syntax_error2("<s3>" );
    }
}

//PURPOSE: show rule 5.<noun>::= WORD1 | PRONOUN
//ALGORITHM: //ALGORITHM: switch statment for incomeing token   
//    call functions for specific case   
void Tnoun()
{
  cout << "Processing <noun>" << endl;
  switch(next_token())
    {
    case WORD1:
      match(WORD1);
      break;
      
    case PRONOUN:
      match(PRONOUN);
      break;
      
    default:
      syntax_error2("<noun>" );
    }
}

//PURPOSE: Show rule 6.<verb>::= WORD2           
//ALGORITHM: if expected token is found call match function
//     else syntax error
void Tverb()
{
  cout  << "Processing <verb>" << endl;
  if(next_token() == WORD2)
    match(WORD2);
  else
    syntax_error2("<verb>" );
}

//PURPOSE:  show rule 7.<be>::= IS | WAS
//ALGORITHM: switch statment for incomeing token   
//    call functions for specific case   
void Tbe()
{
  cout << "Processing <be>" << endl;
  switch(next_token())
    {
    case IS:
      match(IS);
      break;
      
    case WAS:
      match(WAS);
      break;

    default:
      syntax_error2("<be>" );
    }
}

//PURPOSE: show rule 8.<tense>:= VERBPAST | VERBPASTNEG | VERB | VERBNEG
//ALGORITHM: switch statment for incomeing token   
//    call functions for specific case   
void Ttense()
{
  cout << "Processing <tense>" << endl;
  switch(next_token())
    {
    case VERBPAST:
      match(VERBPAST);
      break;

    case VERBPASTNEG:
      match(VERBPASTNEG);
      break;

    case VERB:
      match(VERB);
      break;
      
    case VERBNEG:
      match(VERBNEG);
      break;

    default:
      syntax_error2("<tense>" );
      return;
    }
}
//============================================================================================
