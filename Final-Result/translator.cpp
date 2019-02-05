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
string english[9] ={"I/me", "you", "he/him", "she/her", "it", "Also", "Then", "However", "Therefore"};
int english_index = -1;

enum token_type {ERROR, WORD1, WORD2, PERIOD, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR, EOP};
string token[16] = {"ERROR", "WORD1", "WORD2", "PERIOD", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG","IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR", "EOP"};
token_type saved_lexeme;
string print_word;
string saved_E_word;
bool saved_token;

ifstream fin;
ofstream fout;
ofstream errorf;

//---Prototypes list-----------------------------------
int scanner(token_type& the_type, string& w);
bool reservedWord(string x, token_type& the_type);
bool word(string s, int& which);
bool period(string a);

void syntax_error1(token_type c);    //Syntax error in treminal
void syntax_error2(string where);    //Syntax error in Non terminal
bool match(token_type expected);     //Match incomeing token with expected token
token_type next_token();             //Check if next token exist

void story();                        //Rule 1.<story> ::= <BeginSentence> {<BeginSentence>}
void BeginSentence();                //Rule 2.<BeginSentence> ::= [CONNECTOR #getEword# #gen#] < noun>#getEword# SUBJECT #gen# <EndSentence_1>
void EndSentence_1();                //Rule 3.<EndSentence_1>::= <verb> #getEword# #gen# <tense> #gen# PERIOD | <noun> #getEword# <EndSentence_2>
void EndSentence_2();                //Rule 4.<EndSentence_2>::= <be> #gen# PERIOD | DESTINATION #gen# <verb> #getEword# #gen# <tense> #gen#PERIOD |OBJECT #gen#  [<noun> #getEword# #gen# DESTINATION #gen#]<verb> #getEword# #gen#  <tense> #gen#  PERIOD 
void Tnoun();                        //Rule 5.<noun>::= WORD1 | PRONOUN
void Tverb();                        //Rule 6.<be> ::=  IS | WAS
void Tbe();                          //Rule 7.<be>::= IS | WAS
void Ttense();                       //Rule 8.<tense>:= VERBPAST | VERBPASTNEG | VERB | VERBNEG
void getEword();                     //Get the Engilsh word
void gen(string line_type);          //Display the IR

bool trace = true;
bool recovery = true;

int main()
{
  string  userInput = "0";

  do
    {
      // MENU:
      cout << endl << "MENU" << endl << "------------------------------" << endl;
      cout << "1. Turn trace on/off --> ";

      if(trace)
	cout << "On " << endl;
      else if(!trace)
	cout << "Off " << endl;
 
      cout << "2. Syntax recovery on/off -->";
      if(recovery)
        cout << "On " << endl;
      else if(!recovery)
	cout <<"Off " << endl;

      cout << "3. Enter file " << endl;
      cout << "4. Exit" << endl;

      cout << "Enter input: \t";
      getline(cin, userInput);

      if(userInput == "4")
	break;

      else if(userInput == "1")
	{
	  if(trace)
	    trace  = false;
	  else if(!trace)
	    trace = true;
	}

      else if(userInput == "2")
	{
          if(recovery)
            recovery  = false;
          else if(!recovery)
            recovery = true;
	}

      else if(userInput == "3")
	{
	  string file_name;

	  cout << "Enter the file name:\t";
	  cin >> file_name;

	  fin.open(file_name.c_str());

	  if(!fin)
	    {
	      cout << "Error: File doesn't exist" << endl; // << "Please enter another file name" << endl;                                                                                   
	      exit(1); //goto MENU;                                                                                                                                                          
	    }

	  fout.open("Translated.txt");
	  if(!fout)
	    cout << "Error: Translated.txt not found" << endl << ". . . Creating Translated.txt" << endl;

	  errorf.open("error.txt",ofstream::out | ofstream::trunc);
	  
	  story();            //Beginning Paser                                                                                                                                              

	  fin.close();
	  fout.close();
	  errorf.close();
	  break;
	}
	 
      else
	cout << "Error: Incorrect input " << endl << endl;
    }while(true);
}// end

//Translator Functions
//===============================================================================================
//PURPOSE: Get the english word id it exist
//ALGORITHM: if the token has a english version saved it
//     else save the japanese word
void getEword()
{
  if(saved_lexeme == PRONOUN)
    saved_E_word = english[english_index];
    
  else if(saved_lexeme == CONNECTOR)
    {
      english_index += 5;
      saved_E_word = english[english_index];
    }
  else
    saved_E_word = print_word;
}

//PURPOSE: Find the internal representation
//PARAM: String varible
//ALGORITHM: if else statemenst to find the current internal representation
void gen(string line_type)
{
  if(line_type == "CONNECTOR")
    fout << "CONNECTOR: " << saved_E_word << endl;
    
  else if(line_type == "SUBJECT")
    fout << "ACTOR: " << saved_E_word << endl;
    
  else if(line_type == "BE")
    {
      fout << "DESCRIPTION: " << saved_E_word << endl;
      fout << "TENSE: " << token[saved_lexeme] << endl;
    }
    
  else if(line_type == "OBJECT")
    fout << "OBJECT: " << saved_E_word << endl;
    
  else if(line_type == "DESTINATION")
    fout << "TO: " << saved_E_word << endl;
    
  else if(line_type == "VERB")
    fout << "ACTION: "  << saved_E_word << endl;
    
  else if(line_type == "TENSE")
    fout << "TENSE: " << token[saved_lexeme] << endl;
}
//========================================================================


//Paser Functions
//===============================================================
//PURPOSE: Match the tokens
//PARAM:  tokeen_type
//ALGORITHM: if expected token is wrong return false
// else rturn true
bool match(token_type expected)
{
  if(next_token() != expected)
    {
      syntax_error1(expected);
      return false;
    }
  else
    {
      if(trace)
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

//------------------------------------------------------------------------------------
//PURPOSE: Syntax error in terminal
//PARAM: Token
//ALGORITHM: cout token type and print the lexeme
void syntax_error1(token_type c)
{
  cout << "SYNTAX ERROR: expected " << token[c] << " but found " << print_word << endl;
  errorf << "SYNTAX ERROR: expected " << token[c] << " but found " << print_word << endl;

  if(recovery)
    exit(1);
}

//PURPOSE: Syntax error in Non terminal
//PARAM: string name of non termianl
//ALGORITHM: cout lexeme and rule
void syntax_error2(string where)
{
  cout << "SYNTAX ERROR: unexpected " << print_word << " found in function " << where << endl;
  errorf << "SYNTAX ERROR: unexpected " << print_word << " found in function " << where << endl;

  if(!recovery)
    exit(1);
    
}

//Rules
//------------------------------------------------------------------
//PURPOSE: Show rule 1. <story> ::= <BeginSentence> {<BeginSentence>}
//ALGORITHM: do while loop
void story()
{
  bool Ts1;
    do
      {
	if(trace)
	  cout << "Processing <story>" << endl << endl;;
        BeginSentence();
        
        fout << endl;
	
	if(trace)
	  cout << "------------END----------------" << endl <<  endl << endl;

        Ts1 = ((next_token() == CONNECTOR) || (next_token() == WORD1) || (next_token() == PRONOUN));
        
      }while(Ts1);
}

//PURPOSE: show rule 2. <BeginSentence> ::= [CONNECTOR #getEword# #gen#] < noun>  #getEword# SUBJECT #gen# <EndSentence_1>
//ALGORITHM: if statment for [] optional
//      call functions
void BeginSentence()
{
  if(trace)
    cout << "========== Processing <BeginSentence> ==========" << endl;
    
  if(next_token() == CONNECTOR)
    {
      match(CONNECTOR);
      getEword();
      gen("CONNECTOR");
    }
    
  Tnoun();
  getEword();
    
  match(SUBJECT);
  gen("SUBJECT");
    
  EndSentence_1();
}

//PURPOSE: Show rule 3.<EndSentence_1>::= <verb> #getEword# #gen# <tense> #gen#  PERIOD | <noun> #getEword# <EndSentence_2>
//ALGORITHM: switch statemt
//     call functions for specific case
void EndSentence_1()
{
  if(trace)
    cout << "Processing <EndSentence_1> " << endl;
    
  switch(next_token())
    {
    case WORD2:
      Tverb();
      getEword();
      gen("VERB");
            
      Ttense();
      gen("TENSE");
            
      match(PERIOD);
      break;
            
    case WORD1:
    case PRONOUN:
      Tnoun();
      getEword();
            
      EndSentence_2();
      break;
            
    default:
      syntax_error2("<EndSentence_1>" );
    }
}

//PURPOSE:  Show rule 4.<EndSentence_2>::= <be> PERIOD |
//                              DESTINATION #gen# <verb> #getEword# #gen# <tense> #gen#  PERIOD |
//OBJECT #gen#  [<noun> #getEword# #gen# DESTINATION #gen#]<verb> #getEword# #gen#  <tense> #gen#  PERIOD
//ALGORITHM: switch statment for incomeing token
//    call functions for specific case
void EndSentence_2()
{
  if(trace)
    cout << "Processing <EndSentence_2>" << endl;

  switch(next_token())
    {
    case IS:
    case WAS:
      Tbe();
      gen("BE");
      match(PERIOD);
      break;
            
    case DESTINATION:
      match(DESTINATION);
      gen("DESTINATION");
            
      Tverb();
      getEword();
      gen("VERB");
            
      Ttense();
      gen("TENSE");
            
      match(PERIOD);
      break;
            
    case OBJECT:
      match(OBJECT);
      gen("OBJECT");
            
      if(next_token() == WORD1 || next_token() == PRONOUN)
	{
	  Tnoun();
	  getEword();
                
	  match(DESTINATION);
	  gen("DESTINATION");
	}
            
      Tverb();
      getEword();
      gen("VERB");
            
      Ttense();
      gen("TENSE");
            
      match(PERIOD);
      break;
            
    default:
      syntax_error2("<EndSentence_2>" );
    }
}

//PURPOSE: show rule 5.<noun>::= WORD1 | PRONOUN
//ALGORITHM:
//    call functions for specific case
void Tnoun()
{
  if(trace)
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
  if(trace)
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
  if(trace)
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
  if(trace)
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
//End of Paser
//==================================================================================================

//Scanner
//==================================================================================================
//-------------------------------------------------------------------------------------------------
int scanner(token_type& the_type, string& w)
{
  int l =0;
  if(trace)
    cout << "--Scanner was called--" << endl;
  fin >> w;

  if(w == "eof" || w == "EOF")
    exit(1);

  print_word = w;
  //cout << "Word is : " << w << endl << endl;
  if(!reservedWord(w, the_type))                     // send the word from file to reservedWord function
    {
      if(word(w, l))
        {
	  if(l == 0)
            {the_type = WORD1;}                      // when all lower case
	  else if(l == 1)
            {the_type = WORD2;}                      // in uppercase
        }
        
      else  if (period(w))                           // if it is period
        {the_type = PERIOD;}
      else
        {the_type = ERROR;}                          // in case if it is error
    }
    
  if(the_type == ERROR)
    {
      cout << "lexicon error: " << w << " not a token" << endl;
      errorf << "lexicon error: " << w << " not a token" << endl;
    }
  return 0;
}

//PURPOSE:to check if the word is already used
//PARAM:takes x and the type of the word
//ALGORITHM:uses a while loop to check each word
bool reservedWord(string x, token_type& the_type)
{
  english_index = 0;
    
  while(english_index < 5)
    {
      if((english_index < 4) && (verb[english_index] == x))
        {
	  if(english_index == 0)
            {the_type = VERB;}
            
	  else if(english_index == 1)
            {the_type = VERBNEG;}
            
	  else if(english_index == 2)
            {the_type = VERBPAST;}
            
	  else if(english_index == 3)
            {the_type = VERBPASTNEG;}
	  return true;
        }
        
      else if(english_index < 4 && connector[english_index] == x)
        {
	  the_type = CONNECTOR;
	  return true;
        }
        
      else if(english_index < 5 && object[english_index] == x)
        {
	  if(english_index == 0)
            {the_type = IS;}
            
	  else if(english_index == 1)
            {the_type = WAS;}
            
	  else if(english_index == 2)
            {the_type = OBJECT;}
            
	  else if(english_index == 3)
            {the_type = SUBJECT;}
            
	  else if(english_index == 4)
            {the_type = DESTINATION;}
            
	  return true;
        }
        
      else if(english_index < 5 && pronoun[english_index] == x)
        {
	  the_type = PRONOUN;
	  return true;
        }
      english_index++;
    }
  return false;
}


//  MYTOKEN DFA to be replaced by the WORD DFA
//  RE: (vowel vowel n | constant vowel | constant vowel n | constant-pair vowel | constant-pair vowel n)^+
//--------------------------------------------------------------
bool word(string s, int& which)
{
  int state = 0;
  int charpos = 0;
  bool vowel, consonant, consonant_pair;
  bool c, h, x, t, y, n;
  bool cap;
    
  while (s[charpos] != '\0')
    {
      vowel = (s[charpos] == 'a' || s[charpos] == 'e' || s[charpos] == 'i' ||s[charpos] == 'o' || s[charpos] == 'u' || s[charpos] == 'E' || s[charpos] == 'I' );
      consonant_pair = (s[charpos] == 'd' || s[charpos] == 'j' || s[charpos] == 'w' || s[charpos] == 'y' || s[charpos] == 'z');
      consonant = (s[charpos] == 'b'|| s[charpos] == 'g' ||  s[charpos] == 'h' || s[charpos] == 'm' || s[charpos] == 'n' || s[charpos] == 'p' || s[charpos] == 'r' || s[charpos] == 'k');
      t = (s[charpos] == 't');
      x = (s[charpos] == 's');
      c = (s[charpos] == 'c');
      h = (s[charpos] == 'h');
      y = (s[charpos] == 'y');
      n = (s[charpos] == 'n');
        
        
      cap = (s[charpos] == 'I' || s[charpos] == 'E');
      if(cap)
        {which = 1;}
        
      if(state == 0)
        {
	  if(t)
            {state = 1;}
	  else if(x)
            {state = 2;}
	  else if(consonant)
            {state = 3;}
	  else if(consonant_pair)
            {state = 4;}
	  else if(c)
            {state = 5;}
	  else if(vowel)
            {state = 7;} //STATE 4
	  else
	    state = -1;
        }
        
      else if(state == 1)
        {
	  if(x)
            {state = 4;}
	  else if(vowel)
            {state = 6;}
	  else
	    state = -1;
        }
        
      else if(state == 2)
        {
	  if(h)
            {state = 4;}
	  else if(vowel)
            {state = 6;}
	  else
	    state = -1;
        }
        
      else if(state == 3)
        {
	  if(y)
            {state = 4;}
	  else if(vowel)
            {state = 7;} // state 6
	  else
	    state = -1;
        }
        
      else if (state == 4)
        {
	  if(vowel)
            {state = 7;}
	  else
	    state = -1;
        }
        
      else if (state == 5)
        {
	  if(h)
            {state = 4;}
	  else
	    state = -1;
        }
        
      else if(state  == 6)
        {
	  if(n)
            {state = 0;}
	  else if(t)
            {state = 1;}
	  else if(x)
            {state = 2;}
	  else if(consonant)
            {state = 3;}
	  else if(consonant_pair)
            {state = 4;}
	  else if(c)
            {state = 5;}
	  else if(vowel)
            {state = 6;}
	  else
	    state = -1;
        }
        
      else if(state == 7)
        {
	  if(n)
            {state = 0;}
	  else if(t)
            {state = 1;}
	  else if(x)
            {state = 2;}
	  else if(consonant)
            {state = 3;}
	  else if(consonant_pair)
            {state = 4;}
	  else if(c)
            {state = 5;}
	  else if(vowel)
            {state = 7;}
	  else 
	    state = -1;
        }
      else
	return(false);
      charpos++;
    }//end of while   
    
  if (state == 6 || state == 7)
    return(true);
  else if(state == 0 && charpos > 1)
    {return true;}
  else
    return(false);
}

//  Add the PERIOD DFA here
//--------------------------------------------------------------
bool period(string a)
{
  if (a == ".")
    return true;
  else 
    return false;
}
//=============================================================
