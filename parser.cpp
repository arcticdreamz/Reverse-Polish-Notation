#include "parser.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <ios> //std::streamoff
#include <stdexcept> //domain_error
#include <cctype> //isspace
#include <fstream>

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

	in.seekg(count());

	c = in.get();

	while(isspace(c)) {
		c = in.get();
	}

	if(single_char.find(c) != string::npos){
		s = s + c;
		return s;
	}

	//Check if "sin", "cos", "pi" or "avg"
	else if(sensitive_chars.find(c) != string::npos){

		s = s + c; //Append to string


		while(true) {
			c = in.get();
			if(sensitive_chars.find(c) != string::npos)
				s = s + c;
			else {
				in.unget();
				break;
			}
		}

		if(s.size() > 3) 
			throw std::domain_error("BAD TOKEN (too long): " + s);
		else
			return s;

		

	}else{
		throw std::domain_error("BAD TOKEN: " + c);
	}

}
