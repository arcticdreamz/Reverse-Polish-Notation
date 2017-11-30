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
	const string sensitive_chars = "sincospi"
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

		}elseif(sensitive_chars.find(c) != string::npos){
			s = s + c; //Append to string

			//Going to check if "sin", "cos" or "pi"
			while(sensitive_chars.find(s) != string::npos) {
				c = in.get(); count++;
				s = s + c;
			}

			if(s.size > 3) // not "sin","cos" or "pi"
				//THROW ERROR
			else() 
				identifyToken(s);


		}elseif{
			//THROW ERROR
		}

		return identifyToken(s)

	}
}

token Lexer::peek(){


}

std::streamoff Lexer::count() const {

}

void Lexer::reset(){


}




