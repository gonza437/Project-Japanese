//=================================================================================
// File scanner.cpp written by: Group Number: 11
//==============================================================================
#include<iostream>
#include<fstream>
#include<string>
#include<ctype.h>

using namespace std;

//---------------------Global Variables----------------------------------------------
enum tokentype {ERROR, WORD1, WORD2, PERIOD, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR}; 

string verb[4] = {"masu", "masen", "mashita", "masendeshita"};
string connector[4] = {"mata", "soshite", "shikashi", "dakara"};
string object[5] = {"desu", "deshita", "o", "wa", "ni"};
string pronoun[5] = {"watashi", "anata", "kare", "kanojo", "sore"};

ifstream fin;
//-------------------------------------------------------------------------


//--------------------------------------------------------------------------
int scanner(tokentype& the_type, string& w);
bool reservedWord(string x, tokentype& the_type);
bool word(string s, int& which);
bool period(string a);
//-------------------------------------------------------------------------

int main()
{
  string tokens[15] = {"ERROR", "WORD1", "WORD2", "PERIOD", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG", "IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR"};
  tokentype thetype;
  string theword, file_name;                             // strings for the word, and the filename

  cout << "Enter the file name:   ";                     // ask the user for the file name 
  getline(cin,file_name);                                // get the file name from the user 
  fin.open(file_name.c_str());                           // open the file
  
  if (fin)    // to check if the file exists or not
    {
      while(true)
	{
	  cout << "=========================================================" << endl;
	  scanner(thetype, theword);  // call the scanner                                                                                     
	    
	  if(theword == "eofm" || theword == "EOFM" || !fin)
	    {break;}

	  if(thetype != ERROR)
	    {cout << "Type is: " << tokens[thetype] << endl;}

	  cout << "==========================================================" << endl << endl;
	  // display the actual type instead of a number       
	}
      fin.close();                                        // close the file 
    }
  else // if the file doesn't exist 
    {cout << "ERROR: File doesn't exist\n";}
  
  return 0;
}// end
//================================================================


//  Need the lexicon to be set up here (to be used in Part C)
//================================================================
//----------------------------------------------------------------
int scanner(tokentype& the_type, string& w)
{
  int l =0;
  cout << "--Scanner was called--" << endl;
  fin >> w;
   
  cout << "Word is : " << w << endl << endl;
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
    {cout << "lexicon error: >> " << w << " << not a token" << endl;}
}
//====================================================================

//--------------------------------------------------------------------
//PURPOSE:to check if the word is already used 
//PARAM:takes x and the type of the word 
//ALGORITHM:uses a while loop to check each word
bool reservedWord(string x, tokentype& the_type)
{
  int i = 0;

  while(i < 5)
    {
      if((i < 4) && (verb[i] == x))
        {  
          if(i == 0)
            {the_type = VERB;}
	      
	  else if(i == 1)
	    {the_type = VERBNEG;}

	  else if(i == 2)
	    {the_type = VERBPAST;}

	  else if(i == 3)
	    {the_type = VERBPASTNEG;}
	  return true;
        }

      else if(i < 4 && connector[i] == x)
        {
          the_type = CONNECTOR;
          return true;
        }

      else if(i < 5 && object[i] == x)
        { 
          if(i == 0)
            {the_type = IS;}
	      
	  else if(i == 1)
	    {the_type = WAS;}

	  else if(i == 2)
	    {the_type = OBJECT;}

	  else if(i == 3)
	    {the_type = SUBJECT;}
	      
	  else if(i == 4)
	    {the_type = DESTINATION;}

	  return true;
        }

      else if(i < 5 && pronoun[i] == x)
        {
          the_type = PRONOUN;
          return true;
        }
      i++;
    }
  return false;
}
//=========================================================================================


//=========================================================================================
//  MYTOKEN DFA to be replaced by the WORD DFA
//  RE: (vowel vowel n | constant vowel | constant vowel n | constant-pair vowel | constant-pair vowel n)^+
//----------------------------------------------------------
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
            state -1;
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
//============================================================


//===========================================================
bool period(string a)
{
  if (a == ".")
    return true;
  else 
    return false;
}
//========================================================
