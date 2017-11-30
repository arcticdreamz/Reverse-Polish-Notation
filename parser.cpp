#include "parser.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using std::string

Lexer::Lexer(std::istream& inputStream){
	in = inputStream;
}

token Lexer::next() {
	const string sensitive_chars = "sincospiavg"
	const string other_chars = "+/2,"
	const string single_char = "xy()*"
	if(in) {
		char c;
		string s;

		c = in.get();
		count++;
		while(isspace(c)) {
			c = in.get(); count++; 

		}

		if(single_char.find(c) != string::npos){
			identifyToken(c);

		//Going to check if "sin", "cos", "pi" or "avg"
		}elseif(sensitive_chars.find(c) != string::npos){
			s = s + c; //Append to string


			while(sensitive_chars.find(s) != string::npos) {
				c = in.get(); count++;
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

token Lexer::peek(){


}

std::streamoff Lexer::count() const {
	return this.count;
}

void Lexer::reset(){
	this.count = 0;


}


	token identifyToken(std::string s) {
		if(s == 'x')
			return token::X;

		if(s == 'Y')
			return token::Y;
		
		if(s == "sin")
			return token::SIN;
		
		if(s == "cos")
			return token::COS;
		
		if(s == "pi")
			return token::PI;

		if(s == '(')
			return token::OPEN_PAR;		

		if(s == ')')
			return token::CLOSE_PAR;

		if(s == '*')
			return token::TIMES;

		if(s == "avg")
			return token::AVG;

		if(s == ',')
			return token::COMMA;
}







	}

