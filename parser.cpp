#include "parser.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ios> //std::streamoff
#include <stdexcept> //domain_error
#include <cctype> //isspace

using std::string;

Lexer::Lexer(std::istream& inputStream){
	in = inputStream;
}

Lexer::token Lexer::next() {
	const string sensitive_chars = "sincospiavg";
	const string single_char = "xy()*";
	if(in) {
		char c;
		string s;

		c = in.get();
		counter++;
		while(isspace(c)) {
			c = in.get(); counter++; 
		}

		if(single_char.find(c) != string::npos){
			identifyToken(c);

		//Going to check if "sin", "cos", "pi" or "avg"
		}elseif(sensitive_chars.find(c) != string::npos){
			s = s + c; //Append to string


			while(sensitive_chars.find(s) != string::npos) {
				c = in.get(); counter++;
				s = s + c;
			}

			if(s.size > 3 || in.get() != '(' ) 
				//THROW ERROR
			identifyToken(s);


		}elseif{
			//THROW ERROR
		}

		return identifyToken(s);

	}
}

Lexer::token Lexer::peek(){


}

std::streamoff Lexer::count() const {
	return this.counter;
}

void Lexer::reset(){
	this.counter = 0;
}


Lexer::token identifyToken(std::string s) {
	if(s == "x")
		return token::X;

	if(s == "Y")
		return token::Y;
	
	if(s == "sin")
		return token::SIN;
	
	if(s == "cos")
		return token::COS;
	
	if(s == "pi")
		return token::PI;

	if(s == "(")
		return token::OPEN_PAR;		

	if(s == ")")
		return token::CLOSE_PAR;

	if(s == "*")
		return token::TIMES;

	if(s == "avg")
		return token::AVG;

	if(s == ",")
		return token::COMMA;
}


