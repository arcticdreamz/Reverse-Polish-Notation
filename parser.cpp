#include "parser.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <ios> //std::streamoff
#include <stdexcept> //domain_error
#include <cctype> //isspace
#include <fstream>

using std::string;

Lexer::Lexer(std::istream& inputStream){
	in = inputStream;
}

Lexer::token Lexer::next() {
	const string sensitive_chars = "sincospiavg";
	const string single_char = "xy()*";
	if(in) {
		char* c;
		string s;

		c = in.get(); counter++;

		while(isspace(*c)) {
			c = in.get(); counter++; 
		}

		if(single_char.find(c) != string::npos)
			identifyToken(c);

		//Going to check if "sin", "cos", "pi" or "avg"
		else if(sensitive_chars.find(c) != string::npos){
			s = s + c; //Append to string


			while(sensitive_chars.find(s) != string::npos) {
				c = in.get(); counter++;
				s = s + c;
			}

			if(s.size() > 3 || in.get() != '(' ) 
				//THROW ERROR
			identifyToken(s.c_str());


		}else{
			//THROW ERROR
		}
	}
}

Lexer::token Lexer::peek(){


}

std::streamoff Lexer::count() const {
	return counter;
}

void Lexer::reset(){
	counter = 0;
}


Lexer::token identifyToken(const char* s) {
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


