#include "parser.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <stdexcept> //domain_error
#include <cctype> //isspace

using std::string;
using std::endl;
using std::cout;
using std::vector;
using std::cin;
/* 
  std::string tokenToText[10] = {"X", "Y", "SIN", "COS", "PI", "OPEN_PAR", "CLOSE_PAR", "TIMES", "AVG", "COMMA"};
  Lexer::token tok;
  Lexer lexer(is);
  while(lexer.peek() != std::char_traits<char>::eof()){
  tok = lexer.next();
    std::cout << tokenToText[tok] << " " << lexer.count()<< endl;
  }
  }
  */


int main (){
  std::string input = "(sin(pi*x)qq*cos(pi*y))";

  int i;
  std::cout<< "Do you want to input yourself(1) or take the main expression(2) :";
  std::cin>> i; 
  if(i == 1) {
    std::cout<< "Your input :";
    std::cin>> input;
  }

  std::istringstream is(input);
  std::cout<<"STR ="<<input<<'\n';
  /*
  std::string tokenToText[10] = {"X", "Y", "SIN", "COS", "PI", "OPEN_PAR", "CLOSE_PAR", "TIMES", "AVG", "COMMA"};
  Lexer::token tok;
  Lexer lexer(is);
  lexer.peek();
  lexer.peek();
  lexer.peek();
  while(lexer.peek() != std::char_traits<char>::eof()){
  tok = lexer.next();
    std::cout << tokenToText[tok] << " " << lexer.count()<< endl;
  }*/
  
 
  Parser parser(is);
  Exp expression;
  if(parser.parse(expression) == true)
    std::cout<<"Parse OK"<<'\n';
  else std::cout<<"Parse NOT OK"<<'\n';
  std::cout<<"expression =";
  for(auto it= expression.begin(); it != expression.end();++it)
    std::cout<<" "<<*it;
  






  return 0;
}