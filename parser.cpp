#include "parser.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <ios> //std::streamoff
#include <stdexcept> //domain_error
#include <cctype> //isspace
#include <fstream>

using std::string;

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
	const string single_char = "xy()*";

	in.seekg(count());
	char c;
	string s;

	c = in.get(); 

	while(isspace(c)) {
		c = in.get(); 
	}

	if(single_char.find(c) != string::npos){
		s = s + c;
		return s;
	}

	//Going to check if "sin", "cos", "pi" or "avg"
	else if(sensitive_chars.find(c) != string::npos){
		s = s + c; //Append to string


		while(sensitive_chars.find(s) != string::npos) {
			c = in.get(); 
			s = s + c;
		}

		if(s.size() > 3) {
			throw std::domain_error("Error at string : " + s);
			
		}else if(s == "sin" || s == "cos" && in.get() != '(') {
			in.unget();	
			throw std::domain_error("Error at string : " + s);
		}
		

		return s;


	}else{
		throw std::domain_error("Not a valid character : " + c);
	}

}
