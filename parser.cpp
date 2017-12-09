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

  /*Lexer lexer(is);
    Lexer::token tok;


  std::string tokenToText[10] = {"X", "Y", "SIN", "COS", "PI", "OPEN_PAR", "CLOSE_PAR", "TIMES", "AVG", "COMMA"};
 while(true){

 	lexer.next();
  	std::cout << tokenToText[tok] << " " << lexer.count()<< endl;
  }*/

/* 
int main (){
  std::string input = "(sin(pi * avg(x,y)) * x)";
  std::istringstream is(input);
  std::cout<<"STR ="<<input<<'\n';
 
 /* std::string tokenToText[10] = {"X", "Y", "SIN", "COS", "PI", "OPEN_PAR", "CLOSE_PAR", "TIMES", "AVG", "COMMA"};
  Lexer::token tok;
  Lexer lexer(is);
  while(lexer.peek() != std::char_traits<char>::eof()){
 	tok = lexer.next();
  	std::cout << tokenToText[tok] << " " << lexer.count()<< endl;
  }



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
*/



Lexer::Lexer(std::istream& inputStream) : in(inputStream),counter(0){}

Lexer::token Lexer::next() {
	if(in.peek() != EOF) {
		in.seekg(count()); //Go to the current position
		string s = extractString();
		while(count() < in.tellg())
			counter++;

		return identifyToken(s);
	}
	else{
		throw std::domain_error("EOI");
	}
}

Lexer::token Lexer::peek(){
	if(in.peek() != EOF) {
		in.seekg(count()); //Go to the current position
		string s = extractString();
		in.seekg(count()); //Go back to where we were before extracting the string
		return identifyToken(s);
	}
}

std::streamoff Lexer::count() const {
	return counter;
}

void Lexer::reset(){
	counter = 0;
}


Lexer::token Lexer::identifyToken(const string s) {
	if(s == "x")
		return Lexer::token::X;

	if(s == "y")
		return Lexer::token::Y;

	if(s == "sin")
		return Lexer::token::SIN;

	if(s == "cos")
		return Lexer::token::COS;

	if(s == "pi")
		return Lexer::token::PI;

	if(s == "(")
		return Lexer::token::OPEN_PAR;

	if(s == ")")
		return Lexer::token::CLOSE_PAR;

	if(s == "*")
		return Lexer::token::TIMES;

	if(s == "avg")
		return Lexer::token::AVG;

	if(s == ",")
		return Lexer::token::COMMA;
}

std::string Lexer::extractString(){
	const string sensitive_chars = "sincospiavg";
	const string single_char = "xy()*,";
	char c;
	string s = "";


	c = in.get();
	//check for EOF
	if(c == std::char_traits<char>::eof())
		throw std::domain_error("EOI");
	
	//Ignore whitespace 
	while(isspace(c)) {
		c = in.get();
		if(c == std::char_traits<char>::eof())
			throw std::domain_error("EOI");
	}

	//Check if "xy()*,"
	if(single_char.find(c) != string::npos){
		s.push_back(c);
		return s;
	}

	//Check if "sin", "cos", "pi" or "avg"
	else if(sensitive_chars.find(c) != string::npos){

		s.push_back(c);

		while(true) {
			c = in.get();
			if(c == std::char_traits<char>::eof()) //Check for end of file
				throw std::domain_error("EOI");
			else if(sensitive_chars.find(c) != string::npos)
                s.push_back(c);
			else {
				in.unget();// We read a character that's not sensitive
				break;
			}
		}

		if(s.size() > 3 || (s.size() <= 2 && s != "pi"))
			throw std::domain_error("BAD TOKEN: " + s);
		else
			return s;
	}else{
		throw std::domain_error("BAD TOKEN: " + c);
	}


}


Parser::Parser(std::istream& inputStream): lexer(inputStream){}


bool Parser::parse(Exp& exp){
	//Phase 1 - SYNTAX
	if(!checkSyntax())
		return false;
	//Phase 2 -- INFIX TO RPN
	return infixToRPN(exp,tokenVector);
}


bool Parser::checkSyntax() {

	bool syntaxOK = true;

	if(lexer.peek() == Lexer::SIN || lexer.peek() == Lexer::COS){
		syntaxOK = checkSinCos();
	}
	//Check AVG
	else if(lexer.peek() == Lexer::AVG){
		syntaxOK = checkAverage();
	}
	//Check product
	else if(lexer.peek() == Lexer::OPEN_PAR){
		 syntaxOK = checkProduct();	
	//Check X/Y
	}else if(lexer.peek()  == Lexer::X || lexer.peek() == Lexer::Y){
		tokenVector.push_back(lexer.next());
	}

	return syntaxOK;
} //end of checkSyntax


bool Parser::checkSinCos(){

	tokenVector.push_back(lexer.next());

	//Check if OPEN_PAR follows SIN/COS
	if(lexer.peek() != Lexer::OPEN_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokenVector.push_back(lexer.next()); //Take token

  	//Remember the position of the OPEN_PAR
  	openParLocations.push_back(lexer.count());

  	//Check that pi is the left hand operand in SIN/COS
  	if(lexer.peek() != Lexer::PI)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokenVector.push_back(lexer.next()); //Take token

	//Check if TIMES follows PI
  	if(lexer.peek() != Lexer::TIMES)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokenVector.push_back(lexer.next()); //Take token

	//If there's no expr1
	if(lexer.peek() == Lexer::CLOSE_PAR){
  		return false;
	}

  	//Searching for CLOSE_PAR
	while(lexer.peek() != Lexer::CLOSE_PAR){
		if(!checkSyntax())
			return false;
	}

	//If not found
	if(lexer.peek() != Lexer::CLOSE_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());
  	
  	tokenVector.push_back(lexer.next()); //Take token

  	//If the container is empty (not enough CLOSE_PAR) OR
  	// the size of the container is 1 (no OPEN_PAR) 
  	// and there is still one CLOSE_PAR left
  	if(openParLocations.empty() || (openParLocations.size() == 1 && lexer.peek() == Lexer::CLOSE_PAR)){
			throw std::domain_error("PARSE ERROR at " + lexer.count());
	}else{
		openParLocations.pop_back();
	}

	return true;
}
bool Parser::checkAverage(){
	
	tokenVector.push_back(lexer.next()); //Take token AVG

	//Check if OPEN_PAR follows avg
  	if(lexer.peek() != Lexer::OPEN_PAR)
		throw std::domain_error("PARSE ERROR at " + lexer.count());
	
  	tokenVector.push_back(lexer.next()); //Take token OPEN_PAR
  	
  	//Remember the position of the OPEN_PAR
  	openParLocations.push_back(lexer.count());

	//If there's no expr1
	if(lexer.peek() == Lexer::COMMA){
  		return false;
	}

	//Searching the COMMA
	while(lexer.peek() != Lexer::COMMA){
		if(!checkSyntax())
			return false;
	}

	//Check COMMA
  	if(lexer.peek() != Lexer::COMMA)
		throw std::domain_error("PARSE ERROR at " + lexer.count());

	tokenVector.push_back(lexer.next()); //Take the token

	//If there's no expr2
	if(lexer.peek() == Lexer::CLOSE_PAR){
  		return false;
	}

	while(lexer.peek() != Lexer::CLOSE_PAR){
		if(!checkSyntax())
			return false;
	}

	// checks if AVG ends with CLOSE_PAR
	if(lexer.peek() != Lexer::CLOSE_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokenVector.push_back(lexer.next()); //Take the CLOSE_PAR

  	//If the container is empty (not enough CLOSE_PAR) OR
  	// the size of the container is 1 (no OPEN_PAR) 
  	// and there is still one CLOSE_PAR left
  	if(openParLocations.empty() || (openParLocations.size() == 1 && lexer.peek() == Lexer::CLOSE_PAR)){
			throw std::domain_error("PARSE ERROR at " + lexer.count());
	}else{
		openParLocations.pop_back();
	}

	return true;
}
bool Parser::checkProduct(){

	tokenVector.push_back(lexer.next()); //Take the OPEN_PAR

  	//Remember the position of the OPEN_PAR
  	openParLocations.push_back(lexer.count());

	//If there's no expr1
	if(lexer.peek() == Lexer::TIMES){
  		return false;
	}
	//Searching the TIMES
	while(lexer.peek() != Lexer::TIMES){
		if(!checkSyntax())
			return false;
	}
  	//Check TIMES
  	if(lexer.peek() != Lexer::TIMES)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());
	
	tokenVector.push_back(lexer.next()); //Take the TIMES

	//If there's no expr2
	if(lexer.peek() == Lexer::CLOSE_PAR){
  		return false;
	}

	// checks if product ends with CLOSE_PAR
	while(lexer.peek() != Lexer::CLOSE_PAR){
		if(!checkSyntax())
			return false;
	}
	//if not found
	if(lexer.peek() != Lexer::CLOSE_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokenVector.push_back(lexer.next());//Take the CLOSE_PAR

  	//If the container is empty (not enough CLOSE_PAR) OR
  	// the size of the container is 1 (no OPEN_PAR) 
  	// and there is still one CLOSE_PAR left
  	if(openParLocations.empty() || (openParLocations.size() == 1 && lexer.peek() == Lexer::CLOSE_PAR)){
		throw std::domain_error("PARSE ERROR at " + lexer.count());
	}else{
		openParLocations.pop_back();
	}

	return true;
}



bool Parser::infixToRPN(Exp& exp,std::vector<Lexer::token> tokenVector){

 	Exp operatorStack;
 	
 	//Variables to keep track of the parentheses for AVG
 	int parentheses = 0;
 	std::vector<int> avgPar;

 	auto tok = tokenVector.begin();
 	while(tok != tokenVector.end()){

		if(*tok == Lexer::PI || *tok == Lexer::X || *tok == Lexer::Y){
			//Push the token onto the expression
			exp.push_back(tokenToText[*tok]);
		}
		else if(*tok == Lexer::AVG){
			//Remember the number of parentheses
			avgPar.push_back(parentheses);
		}
		else if(*tok == Lexer::OPEN_PAR){
			parentheses++;
			operatorStack.push_back(tokenToText[*tok]);
		}
		else if(*tok == Lexer::CLOSE_PAR){
			parentheses--;

			//Pop the operators onto the queue as long as
			//we don't encounter a OPEN_PAR
			while(!operatorStack.empty() && operatorStack.back() != tokenToText[Lexer::OPEN_PAR]){
				exp.push_back(operatorStack.back());
				operatorStack.pop_back();
			}

			//If we have found the CLOSE_PAR
			if(!operatorStack.empty())
				operatorStack.pop_back();

			//If parentheses is the same as one of avgPar, we must add "/2"
			if(std::find(avgPar.begin(), avgPar.end(), parentheses) != avgPar.end()){

				//We pop the operators until empty or until
				//we encounter OPEN_PAR or "+" (because smaller precedence)
				while(!operatorStack.empty() && operatorStack.back() != tokenToText[Lexer::OPEN_PAR] && operatorStack.back() != "+"){
					exp.push_back(operatorStack.back());
					operatorStack.pop_back();
				}
				//Push the read operator onto the operator stack
				operatorStack.push_back("/");
				exp.push_back("2");
			}
			

		//For any other token
		}else{
			//We pop the operators until empty or until
			//we encounter OPEN_PAR or "+" (because smaller precedence)
			while(!operatorStack.empty() && operatorStack.back() != tokenToText[Lexer::OPEN_PAR] && operatorStack.back() != "+"){
				exp.push_back(operatorStack.back());
				operatorStack.pop_back();
			}
			//Push the read operator onto the operator stack
			if(*tok == Lexer::COMMA){
				operatorStack.push_back("+");
			}else{
			operatorStack.push_back(tokenToText[*tok]);
			}
		}

		++tok;
	}

	// while there are still operator tokens on the stack, pop them onto the expression
	while(!operatorStack.empty()){
		exp.push_back(operatorStack.back());
		operatorStack.pop_back();
	}

	//Managing the "enter"
	auto it = exp.begin() + 1;
	while(it != exp.end()){
		if(*it == "2" || *it =="x" || *it =="y")
			if(*(it-1) == "2" || *(it-1) =="x" || *(it-1) =="y")
				exp.insert(it,"enter");
		++it;
	}
	return true;
}
