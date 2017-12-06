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



int main (){
  std::string input = "(sin( pi*avg(x , y)) * x)";
  std::istringstream is(input);
  std::cout<<"STR ="<<input<<'\n';

//  Lexer lexer(is);
//  Lexer::token tok = lexer.next();


  std::string tokenToText[10] = {"X", "Y", "SIN", "COS", "PI", "OPEN_PAR", "CLOSE_PAR", "TIMES", "AVG", "COMMA"};
/* while(true){

 	tok = lexer.next();
  	std::cout << tokenToText[tok] << " " << lexer.count()<< endl;
  }
*/

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





Lexer::Lexer(std::istream& inputStream) : in(inputStream){}

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


Parser::Parser(std::istream& inputStream): lexer(inputStream){};


bool Parser::parse(Exp& exp){

	//Check SIN/COS
	if(lexer.peek() == Lexer::SIN || lexer.peek() == Lexer::COS)
  		checkSinCos(exp);

	//Check AVG
	else if(lexer.peek()  == Lexer::AVG)
		checkAverage(exp);

	//Check product (starts with OPEN_PAR)
	else if(lexer.peek()  == Lexer::OPEN_PAR)
		checkProduct(exp);

	//Check X/Y
	else if(lexer.peek()  == Lexer::X || lexer.peek() == Lexer::Y){
        lexer.next();
  	  	exp.push_back("X");
	}
    else if(lexer.peek() == Lexer::Y){
        lexer.next();
        exp.push_back("Y");
    }
	return true;

}

void Parser::checkSinCos(Exp& exp){

  	lexer.next();
  	operatorStack.push_back("sin"); //Takes SIN/COS token

	//Check if OPEN_PAR follows SIN/COS
	if(lexer.peek() != Lexer::OPEN_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	lexer.next();
  	operatorStack.push_back("("); //Takes OPEN_PAR token

  	//Check that pi is the left hand operand in SIN/COS
  	if(lexer.peek() != Lexer::PI)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	lexer.next();
  	exp.push_back("pi"); //Takes PI token

	//Check if TIMES follows PI
  	if(lexer.peek() != Lexer::TIMES)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	lexer.next();
  	operatorStack.push_back("*");//Takes TIMES token

  	parse(exp); //Parse expr1

	//Check if SIN/COS ends with CLOSE_PAR
	if(lexer.peek() != Lexer::CLOSE_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

	lexer.next();
  	operatorStack.push_back(")");//Takes CLOSE_PAR token

}

void Parser::checkAverage(Exp& exp){

	lexer.next(); //Skip AVG token

	//Check if OPEN_PAR follows avg
  	if(lexer.peek() != Lexer::OPEN_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	lexer.next();
  	operatorStack.push_back("(");//Takes OPEN_PAR token

  	parse(exp); //Parse expr1

  	//Check COMMA
  	if(lexer.peek() != Lexer::COMMA)
		throw std::domain_error("PARSE ERROR at " + lexer.count());

	lexer.next(); //Skip COMMA token

  	operatorStack.push_back("+");//Takes '+'

	parse(exp); //Parse expr2

	// checks if AVG ends with CLOSE_PAR
  	if(lexer.peek() != Lexer::CLOSE_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	lexer.next();
  	operatorStack.push_back(")");//Takes CLOSE_PAR token
  	operatorStack.push_back("/");//Takes "/"
  	exp.push_back("2");//Takes CLOSE_PAR token



 }
void Parser::checkProduct(Exp& exp){

  	lexer.next();
  	operatorStack.push_back("(");//Takes OPEN_PAR token

  	parse(exp); //Parse expr1

  	//Check TIMES
  	if(lexer.peek() != Lexer::TIMES)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

  	lexer.next();
  	operatorStack.push_back("*");//Takes TIMES token

	parse(exp); //Parse expr2

	// checks if the product ends with CLOSE_PAR
  	if(lexer.peek() != Lexer::CLOSE_PAR)
  		throw std::domain_error("PARSE ERROR at " + lexer.count());

	lexer.next();
  	operatorStack.push_back(")");//Takes CLOSE_PAR token
}
