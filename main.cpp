#include <vector>
#include <string>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "parser.hpp"
using std::vector;
using std::cout;
using std::endl;
using std::cin;



int main (){
  std::string input = "(sin ( pi*avg(x , y)) * x)";
  std::istringstream is(input);
  std::cout<<"STR ="<<input<<'\n';

  Lexer lexer(is);
 //Parser parser(is);
 /* Exp exp;
  if(parser.parse(exp)==1)
    std::cout<<"Parse OK"<<'\n';
  else std::cout<<"Parse NOT OK"<<'\n';
  std::cout<<"exp =";
  for(Exp::iterator it= exp.begin(); it != exp.end();++it)
    std::cout<<" "<<*it;
*/
  return 0;
}



 
































