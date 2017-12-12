#include "parser.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <stdexcept> //domain_error
#include <cctype> //isspace

using std::endl;
using std::cout;
using std::vector;
using std::cin;



Lexer::Lexer(std::istream& inputStream) : in(inputStream),counter(0){}

Lexer::token Lexer::next() {
	in.seekg(count()); //Go to the current position
	const std::string s = extractString();
	while(count() < in.tellg())
		counter++;

	return identifyToken(s);
}

Lexer::token Lexer::peek(){
	in.seekg(count()); //Go to the current position
	const std::string s = extractString();
	in.seekg(count()); //Go back to where we were before extracting the string
	in.clear(); //Clears the eofbit if lexer.peek() reached eof

	return identifyToken(s);
}

std::streamoff Lexer::count() const {
	return counter;
}

void Lexer::reset(){
	counter = 0;
}


Lexer::token Lexer::identifyToken(const std::string s) {
	if(s == "x")
		return Lexer::X;

	else if(s == "y")
		return Lexer::Y;

	else if(s == "sin")
		return Lexer::SIN;

	else if(s == "cos")
		return Lexer::COS;

	else if(s == "pi")
		return Lexer::PI;

	else if(s == "(")
		return Lexer::OPEN_PAR;

	else if(s == ")")
		return Lexer::CLOSE_PAR;

	else if(s == "*")
		return Lexer::TIMES;

	else if(s == "avg")
		return Lexer::AVG;

	else
		return Lexer::COMMA;
}

std::string Lexer::extractString(){
	const std::string sensitive_chars = "sincospiavg";
	const std::string single_char = "xy()*,";
	char c;
	std::string s = "";


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
	//Add c to string
	s.push_back(c);

	//Check if "xy()*,"
	if(single_char.find(c) != std::string::npos)
		return s;
	

	//Check if "sin", "cos", "pi" or "avg"
	else if(sensitive_chars.find(c) != std::string::npos){
		while(true) {
			c = in.get();
			if(c == std::char_traits<char>::eof()) //Check for end of file
				break;
			else if(sensitive_chars.find(c) != std::string::npos)
                s.push_back(c);
			else {
				in.unget();// We read a character that's not sensitive
				break;
			}
		}
		
		if(s == "sin" || s == "cos" || s == "avg" || s == "pi")
			return s; 
		else if(c == std::char_traits<char>::eof())
			throw std::domain_error("BAD TOKEN: " + s);	
		else
			throw std::domain_error("BAD TOKEN: " + s);
	}
	else{
		throw std::domain_error("BAD TOKEN: " + s);
	}


}


Parser::Parser(std::istream& inputStream): lexer(inputStream){}


bool Parser::parse(Exp& exp){
	if(!checkSyntax())
		return false;
				
	//Phase 2 -- INFIX TO RPN
	return infixToRPN(exp,tokenVector);
}


bool Parser::checkSyntax() {

	if(lexer.peek() == Lexer::SIN || lexer.peek() == Lexer::COS){
		if(!checkSinCos())
			return false;
	}
	//Check AVG
	else if(lexer.peek() == Lexer::AVG){
		if(!checkAverage())
			return false;	
	}
	//Check product
	else if(lexer.peek() == Lexer::OPEN_PAR){
		if(!checkProduct())
			return false;		
	//Check X/Y
	}else if(lexer.peek()  == Lexer::X || lexer.peek() == Lexer::Y){
		if(!checkXY())
			return false;	
	}else{
		return false;
	}		

	return true;
} //end of checkSyntax


bool Parser::checkXY(){
	tokenVector.push_back(lexer.next()); //Take the token

	//After X or Y , there can only be "*)," or EOF
	try{	
		if(lexer.count() == 1){ //We have X or Y alone
			lexer.peek(); //If this doesn't throw, we are not EOI so there is a TOKEN AFTER X/Y
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		}

		if(lexer.peek() != Lexer::CLOSE_PAR && lexer.peek() != Lexer::TIMES && lexer.peek() != Lexer::COMMA)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	}catch(std::domain_error& e){
	std::string error(e.what());
	if(error !="EOI") //ELSE, that means that the expression is correct
		throw;
	else if(lexer.count() != 1) //Throw parse error if X/Y is not by itself
		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	
	}
	return true;
}

bool Parser::checkSinCos(){

	tokenVector.push_back(lexer.next());

	try{
		//Check if OPEN_PAR follows SIN/COS
		if(lexer.peek() != Lexer::OPEN_PAR)
	  		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

	  	tokenVector.push_back(lexer.next()); //Take token

	  	//Remember the position of the OPEN_PAR
	  	openParLocations.push_back(lexer.count());

	  	//Check that pi is the left hand operand in SIN/COS
	  	if(lexer.peek() != Lexer::PI)
	  		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

	  	tokenVector.push_back(lexer.next()); //Take token

		//Check if TIMES follows PI
	  	if(lexer.peek() != Lexer::TIMES)
	  		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

	  	tokenVector.push_back(lexer.next()); //Take token

	  	//Checking expr1
		if(!checkSyntax())
			return false;
			
		
		//If not found
		if(lexer.peek() != Lexer::CLOSE_PAR)
	  		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	  	
	  	tokenVector.push_back(lexer.next()); //Take token

	  	//If the container is empty (not enough CLOSE_PAR)
	  	if(openParLocations.empty())
				throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		else
			openParLocations.pop_back();

		// IF "EOI", there syntax was wrong(things were missing)
		// If PARSE_ERROR, the same --> We transform an EOI into PARSE_ERROR
	}catch(std::domain_error& e){
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	}
  	// the container is empty (no OPEN_PAR) 
  	// and there is still one CLOSE_PAR left
	try{
		if(openParLocations.empty() && lexer.peek() == Lexer::CLOSE_PAR)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

  		//After the closing parenthesis of SIN/COS, there can only be "*,)"
		if(lexer.peek() != Lexer::CLOSE_PAR && lexer.peek() != Lexer::TIMES && lexer.peek() != Lexer::COMMA)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	
		//If there is a TIMES but there is no open parentheses in that block
		if(lexer.peek() == Lexer::TIMES && openParLocations.empty())
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		

	}catch(std::domain_error& e){
		std::string error(e.what());
		if(error !="EOI") //ELSE, that means that the expression is correct
			throw;
	}

	return true;
}
bool Parser::checkAverage(){
	
	tokenVector.push_back(lexer.next()); //Take token AVG

	try{

		//Check if OPEN_PAR follows avg
	  	if(lexer.peek() != Lexer::OPEN_PAR)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		
	  	tokenVector.push_back(lexer.next()); //Take token OPEN_PAR
	  	
	  	//Remember the position of the OPEN_PAR
	  	openParLocations.push_back(lexer.count());

		//checking expr1

		if(!checkSyntax())
			return false;
		

		//Check COMMA
	  	if(lexer.peek() != Lexer::COMMA)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

		tokenVector.push_back(lexer.next()); //Take the token

		//Check expr2
		if(!checkSyntax())
				return false;

		// checks if AVG ends with CLOSE_PAR
		if(lexer.peek() != Lexer::CLOSE_PAR)
	  		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

	  	tokenVector.push_back(lexer.next()); //Take the CLOSE_PAR


	  	//If the container is empty (not enough OPEN_PAR) OR
	  	if(openParLocations.empty())
				throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		else
			openParLocations.pop_back();


		// IF "EOI", the syntax was wrong(things were missing)
		// If PARSE_ERROR, the same --> We transform an EOI into PARSE_ERROR
	}catch(std::domain_error& e){
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	}
	
  	// the container is emty (no OPEN_PAR) 
  	// and there is still one CLOSE_PAR left
	try{
		if(openParLocations.empty() && lexer.peek() == Lexer::CLOSE_PAR)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

  		//After the closing parenthesis there can only be "*,)"
		if(lexer.peek() != Lexer::CLOSE_PAR && lexer.peek() != Lexer::TIMES && lexer.peek() != Lexer::COMMA)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));	

		//If there is a TIMES but there is no open parentheses in that block
		if(lexer.peek() == Lexer::TIMES && openParLocations.empty())
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		
	}catch(std::domain_error& e){		
		std::string error(e.what());
		if(error !="EOI") //ELSE, that means that the expression is correct
			throw;
		}


	return true;
}

bool Parser::checkProduct(){

	tokenVector.push_back(lexer.next()); //Take the OPEN_PAR

  	//Remember the position of the OPEN_PAR
  	openParLocations.push_back(lexer.count());

  	try{
		//checking exp1
		if(!checkSyntax())
			return false;
		
	  	//Check TIMES
	  	if(lexer.peek() != Lexer::TIMES)
	  		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		
		tokenVector.push_back(lexer.next()); //Take the TIMES

		// checks exp2
		if(!checkSyntax())
			return false;
		
		//if not found
		if(lexer.peek() != Lexer::CLOSE_PAR)
	  		throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

	  	tokenVector.push_back(lexer.next());//Take the CLOSE_PAR

	  	//If the container is empty (not enough CLOSE_PAR) OR
	  	if(openParLocations.empty())
				throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		else
			openParLocations.pop_back();

	// IF "EOI", the syntax was wrong(things were missing)
	// If PARSE_ERROR, the same --> We transform an EOI into PARSE_ERROR
	}catch(std::domain_error& e){
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	}

  	// the container is empty (no OPEN_PAR) 
  	// and there is still one CLOSE_PAR left
	try{
		if(openParLocations.empty() && lexer.peek() == Lexer::CLOSE_PAR)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
	
  		//After the closing parenthesis there can only be "*,)"
		if(lexer.peek() != Lexer::CLOSE_PAR && lexer.peek() != Lexer::TIMES && lexer.peek() != Lexer::COMMA)
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));

		//If there is a TIMES but there is no open parentheses in that block
		if(lexer.peek() == Lexer::TIMES && openParLocations.empty())
			throw std::domain_error("PARSE ERROR at " + std::to_string(lexer.count()));
		

	}catch(std::domain_error& e){
		std::string error(e.what());
		if(error !="EOI")
			throw;
	}

	return true;
}



bool Parser::infixToRPN(Exp& exp,std::vector<Lexer::token>& tokenVector){

 	Exp operatorStack;
 	
 	//Variables to keep track of the parentheses for AVG
 	int parentheses = 0;
 	std::vector<int> avgPar;

 	auto tok = tokenVector.begin();
 	while(tok != tokenVector.end()){

		if(*tok == Lexer::PI || *tok == Lexer::X || *tok == Lexer::Y){
			//Pushback enter
			if(!exp.empty()){
				if(exp.back() == "x" || exp.back() =="y"|| exp.back() == "pi")
					exp.push_back("enter");
			}
			//Push the token onto the expression
			exp.push_back(tokenToText[*tok]);

		}
		else if(*tok == Lexer::AVG){
			//Remember the number of parentheses
			avgPar.push_back(parentheses);
		}
		else if(*tok == Lexer::COMMA){
			//Do nothing
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

			//Pop the (eventual) OPEN_PAR
			if(!operatorStack.empty())
				operatorStack.pop_back();

			//If parentheses is the same as one of avgPar, we must add "+2/"
			//(Same method as if the one for the other operators)
			if(std::find(avgPar.begin(), avgPar.end(), parentheses) != avgPar.end()){
				exp.push_back("+");
				exp.push_back("2");
				exp.push_back("/");
			}
			

		//For operators
		}else{
			//We pop the other operators from the stack until empty or until
			//we encounter OPEN_PAR or "+" (because smaller precedence)
			while(!operatorStack.empty() && operatorStack.back() != tokenToText[Lexer::OPEN_PAR]){
				exp.push_back(operatorStack.back());
				operatorStack.pop_back();
			}
			//Pop the (eventual) OPEN_PAR
			if(!operatorStack.empty())
				operatorStack.pop_back();

			//Push the read operator onto the operator stack
			operatorStack.push_back(tokenToText[*tok]);
		}

		++tok;
	}

	// while there are still operator tokens on the stack, pop them onto the expression
	while(!operatorStack.empty()){
		exp.push_back(operatorStack.back());
		operatorStack.pop_back();
	}

	return true;
}
