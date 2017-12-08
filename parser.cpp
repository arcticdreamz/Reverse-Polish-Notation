#include "parser.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <ios> //std::streamoff
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

 	tok = lexer.next();
  	std::cout << tokenToText[tok] << " " << lexer.count()<< endl;
  }*/

 
int main (){
  std::string input = "sin(pi * x))";
  std::istringstream is(input);
  std::cout<<"STR ="<<input<<'\n';




    Parser parser(is);
  Exp expe;
  if(parser.parse(expe) == true)
    std::cout<<"Parse OK"<<'\n';
  else std::cout<<"Parse NOT OK"<<'\n';
  std::cout<<"expe =";
  for(auto it= expe.begin(); it != expe.end();++it)
    std::cout<<" "<<*it;

  return 0;
}




Lexer::Lexer(std::istream& inputStream) : in(inputStream),counter(0){}

Lexer::token Lexer::next() {
	if(in) {
		in.seekg(count()); //Go to the current position
		string s = extractString();
		while(count() < in.tellg())
			counter++;

		return identifyToken(s);
	}
}

Lexer::token Lexer::peek(){
	if(in) {

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

	if(s == "Y")
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


	//Ignore whitespace
	while(isspace(c = in.get())) {
		if(in.eof())
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
			if(in.eof()) //Check for end of file
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


Parser::Parser(std::istream& inputStream): lexer(inputStream){};


bool Parser::parse(Exp& exp){
	/*//Phase 1
	std::vector<Lexer::token> tokenVector;
	while(!lexer.in.eof()){
		tokenVector.push_back(lexer.next());
	}
	auto it = tokenVector.begin();
	while(it != tokenVector.end()){
  		std::cout << tokenToText[*it] << " " << lexer.count()<< endl;
  		++it;
	}
	*/
	//Phase 2 - SYNTAX
	checkSyntax();
}

void Parser::checkSyntax() {

	if(lexer.peek() == Lexer::SIN || lexer.peek() == Lexer::COS){

		lexer.next(); //Ignore token

		//Check if OPEN_PAR follows SIN/COS
		if(lexer.peek() != Lexer::OPEN_PAR)
	  		throw std::domain_error("PARSE ERROR at " + lexer.count());

	  	lexer.next(); //Ignore token

	  	openParLocations.push_back(lexer.count());

	  	//Check that pi is the left hand operand in SIN/COS
	  	if(lexer.peek() != Lexer::PI)
	  		throw std::domain_error("PARSE ERROR at " + lexer.count());

	  	lexer.next(); //Ignore token

		//Check if TIMES follows PI
	  	if(lexer.peek() != Lexer::TIMES)
	  		throw std::domain_error("PARSE ERROR at " + lexer.count());

	  	lexer.next(); //Ignore token

	  	//Searching for CLOSE_PAR
		while(lexer.peek() != Lexer::CLOSE_PAR){
			checkSyntax();
		}

		//If not found
		if(lexer.peek() != Lexer::CLOSE_PAR)
	  		throw std::domain_error("PARSE ERROR at " + lexer.count());
	  	
	  	lexer.next(); //Ignore token

	  	//If the container is empty (not enough CLOSE_PAR) OR
	  	// the size of the container is 1 and there is still one CLOSE_PAR left
	  	// 
	  	if(openParLocations.empty() || (openParLocations.size() == 1 && lexer.peek() == Lexer::CLOSE_PAR)){
  			throw std::domain_error("PARSE ERROR at " + lexer.count());
  		}else{
	  		openParLocations.pop_back();
  		}

	}
	

	//Check AVG
	else if(lexer.peek() == Lexer::AVG){

		lexer.next(); //Ignore token AVG

		//Check if OPEN_PAR follows avg
	  	if(lexer.peek() != Lexer::OPEN_PAR)
			throw std::domain_error("PARSE ERROR at " + lexer.count());
		
	  	lexer.next(); //Ignore token OPEN_PAR

		while(lexer.peek() != Lexer::COMMA){
			checkSyntax();
		}

		//Check COMMA
	  	if(lexer.peek() != Lexer::COMMA)
			throw std::domain_error("PARSE ERROR at " + lexer.count());

		lexer.next(); //Ignore the COMMA

		while(lexer.peek() != Lexer::CLOSE_PAR){
			checkSyntax();
		}

		// checks if AVG ends with CLOSE_PAR
		if(lexer.peek() != Lexer::CLOSE_PAR)
	  		throw std::domain_error("PARSE ERROR at " + lexer.count());

	  	lexer.next(); //Ignore the CLOSE_PAR

	  	//If the container is empty (not enough CLOSE_PAR) OR
	  	// the size of the container is 1 and there is still one CLOSE_PAR left
	  	// 
	  	if(openParLocations.empty() || (openParLocations.size() == 1 && lexer.peek() == Lexer::CLOSE_PAR)){
  			throw std::domain_error("PARSE ERROR at " + lexer.count());
  		}else{
	  		openParLocations.pop_back();
  		}

	}

	//Check product
	else if(lexer.peek() == Lexer::OPEN_PAR){

		lexer.next(); //Ignore the OPEN_PAR

		while(lexer.peek() != Lexer::TIMES){
			checkSyntax();
		}
	  	//Check TIMES
	  	if(lexer.peek() != Lexer::TIMES)
	  		throw std::domain_error("PARSE ERROR at " + lexer.count());
		
		lexer.next(); //Ignore the TIMES

		// checks if product ends with CLOSE_PAR
		while(lexer.peek() != Lexer::CLOSE_PAR){
			checkSyntax();
		}
		//if not found
		if(lexer.peek() != Lexer::CLOSE_PAR)
	  		throw std::domain_error("PARSE ERROR at " + lexer.count());

	  	lexer.next();//Ignore the CLOSE_PAR

	  	//If the container is empty (not enough CLOSE_PAR) OR
	  	// the size of the container is 1 and there is still one CLOSE_PAR left
	  	// 
	  	if(openParLocations.empty() || (openParLocations.size() == 1 && lexer.peek() == Lexer::CLOSE_PAR)){
  			throw std::domain_error("PARSE ERROR at " + lexer.count());
  		}else{
	  		openParLocations.pop_back();
  		}
  		

	//Check X/Y
	}else if(lexer.peek()  == Lexer::X || lexer.peek() == Lexer::Y){
		lexer.next();
	}
} //end of checkSyntax



void Parser::infixToRPN(Exp& exp,Lexer::token tok,Exp& operatorStackFunc){

	if(tok == Lexer::PI 
		|| tok == Lexer::X
	 	|| tok == Lexer::Y){
		//Push the token onto the queue
		exp.push_back(tokenToText[tok]);
	}
	else if(tok == Lexer::COMMA){
		//operatorStackFunc.push_back(tokenToText[lexer.next()]); //Pushing "+"
	}
//	else if(tok == Lexer::AVG)
//		lexer.next(); //Skip the token
	else if(tok == Lexer::OPEN_PAR){
		operatorStackFunc.push_back(tokenToText[tok]);
	}
	else if(tok == Lexer::CLOSE_PAR){

		//lexer.next(); //Discard the token

		//Pop the operators onto the queue as long as
		//we don't encounter a OPEN_PAR
		while(!operatorStackFunc.empty() && operatorStackFunc.back() != tokenToText[Lexer::OPEN_PAR]){
			exp.push_back(operatorStackFunc.back());
			operatorStackFunc.pop_back();
		}

		//If the stack is empty and we haven't found the CLOSE_PAR
		if(operatorStackFunc.empty()){
			//MISMATCHED PARENTHESES
		}
		//Pop the OPEN_PAR from the stack
		else{operatorStackFunc.pop_back();}
	}

	//For any other token
	else{
		//We pop the operators until empty or until
		//we encounter OPEN_PAR
		while(!operatorStackFunc.empty() && operatorStackFunc.back() != tokenToText[Lexer::OPEN_PAR]){
			exp.push_back(operatorStackFunc.back());
			operatorStackFunc.pop_back();
		}

		//Push the read operator onto the operator stack
		operatorStackFunc.push_back(tokenToText[tok]);
	}
}
/*
	//Check SIN/COS
	if(lexer.peek() == Lexer::SIN || lexer.peek() == Lexer::COS)
  		Parser::checkSinCos(exp);

	//Check AVG
	else if(lexer.peek()  == Lexer::AVG)
		checkAverage(exp);

	//Check product (starts with OPEN_PAR)
	else if(lexer.peek()  == Lexer::OPEN_PAR)
		checkProduct(exp);

	//Check X/Y
	else if(lexer.peek()  == Lexer::X || lexer.peek() == Lexer::Y){
        infixToRPN(exp); //Takes X/Y
	}

	return true;

	*/

	//-------------------------------P------------------------------ARSER
/*	bool parsingOK = false;
	if(!lexer.in.eof()){
		checkSinCos(exp,tempOperatorStack);
		while(!tempOperatorStack.empty()){
			operatorStack.push_back(tempOperatorStack.back());
			tempOperatorStack.pop_back();
		}


		checkProduct(exp,tempOperatorStack);
		while(!tempOperatorStack.empty()){
			operatorStack.push_back(tempOperatorStack.back());
			tempOperatorStack.pop_back();
		}

 		checkAverage(exp,tempOperatorStack); 
		while(!tempOperatorStack.empty()){
			operatorStack.push_back(tempOperatorStack.back());
			tempOperatorStack.pop_back();
		}

 		checkXY(exp,tempOperatorStack);
		while(!tempOperatorStack.empty()){
			operatorStack.push_back(tempOperatorStack.back());
			tempOperatorStack.pop_back();
		}
	}
	while(!operatorStack.empty()){
		exp.push_back(operatorStack.back());
		operatorStack.pop_back();
		//ISSUE WITH BRACKET
	}

	if(lexer.peek() == Lexer::CLOSE_PAR){ //Stop the recursion
		return true;
	}
	parsingOK = true;
	return parsingOK;


*/