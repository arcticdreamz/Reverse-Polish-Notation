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

Lexer::Lexer(std::istream& inputStream) : in(inputStream){
}

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
	string s;


	//Ignore whitespace
	while(isspace(c = in.get())) {
		if(in.eof())
			throw std::domain_error("EOI");
	}
	//Check if "xy()*,"
	if(single_char.find(c) != string::npos){
		s = s + c;
		return s;
	}

	//Check if "sin", "cos", "pi" or "avg"
	else if(sensitive_chars.find(c) != string::npos){

		s = s + c; 

		while(true) {
			c = in.get();
			if(in.eof()) //Check for end of file
				throw std::domain_error("EOI");
			else if(sensitive_chars.find(c) != string::npos)
				s = s + c;
			else {
				in.unget();// We read a character that's not sensitive
				break;
			}
		}

		if(s.size() > 3) 
			throw std::domain_error("BAD TOKEN: " + s);
		else
			return s;
	}else{
		throw std::domain_error("BAD TOKEN: " + c);
	}

}


Parser::Parser(std::istream& inputStream) : in(inputStream){
 	Lexer::Lexer lexer(is);
}

bool parse(Exp& exp){
	int parentheses = 0;
	std::vector<Lexer::token> tokVec;


	tokVec.push_back(lexer.next());
	
	//Check SIN/COS
	if(tokVec.back() == Lexer::token[2] || tokVec.back() == Lexer::token[3])
  		checkSinCos(tokVec);
	
	//Check AVG 
	else if(tokVec.back() == Lexer::token[8])
		checkAverage(tokVec);

	//Check product (starts with OPEN_PAR)
	else if(tokVec.back() == Lexer::token[5])
		checkProduct(tokVec);
	
	//Check X/Y
	else if(tokVec.back() == Lexer::token[0] || tokVec.back() == Lexer::token[1])
  	  	tokVec.push_back(lexer.next());		  	
		
}

void checkSinCos(std::vector<Lexer::token>& tokVec){  
	Exp& tempExp;

	//Check if OPEN_PAR follows SIN/COS
	if(lexer.peek() != token[5])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next()); //Takes OPEN_PAR token

  	//Check that pi is the left hand operand in SIN/COS
  	if(lexer.peek() != token[4])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());
  	
  	tokVec.push_back(lexer.next()); //Takes PI token

	//Check if TIMES follows PI
  	if(lexer.peek() != token[7])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next());//Takes TIMES token

  	parse(tempExp); //Parse expr1

	//Check if SIN/COS ends with CLOSE_PAR
	if(lexer.peek() != token[6])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next());//Takes CLOSE_PAR token
	
}

void checkAverage(std::vector<Lexer::token>& tokVec){
	Exp& tempExp;

	//Check if OPEN_PAR follows avg
  	if(lexer.peek() != token[5])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next());//Takes OPEN_PAR token

  	parse(tempExp); //Parse expr1

  	//Check COMMA
  	if(lexer.peek() != Lexer::token[9])
		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next());//Takes COMMA token

	parse(tempExp); //Parse expr2

	// checks if AVG ends with CLOSE_PAR 
  	if(lexer.peek() != token[6])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next());//Takes CLOSE_PAR token


 }
void checkProduct(std::vector<Lexer::token>& tokVec){
	Exp& tempExp;
  	parse(tempExp); //Parse expr1

  	//Check TIMES
  	if(lexer.peek() != token[7])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next());//Takes TIMES token

	parse(tempExp); //Parse expr2
	
	// checks if the product ends with CLOSE_PAR 
  	if(lexer.peek() != token[6])
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	tokVec.push_back(lexer.next());//Takes CLOSE_PAR token
}