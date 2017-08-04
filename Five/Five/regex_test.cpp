//Uses a regex to check if the input is a floating point number
//#include <iostream>
#include <regex.h>
#include <string>

using namespace std;

int main()
{
  string input;
  regex rr("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?");
  //As long as the input is correct ask for another number
  while(true)
  {
    printf("Give me a real number!\n");

    input = "235.64";

    //Exit when the user inputs q
    if(input=="q")
      break;
    if(regex_match(input,rr))
      cout<<"float"<<endl;
    else
    {
      printf("Invalid input" );
    }
  }
}
