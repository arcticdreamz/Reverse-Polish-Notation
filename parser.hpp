#ifndef _PARSER_H
#define _PARSER_H
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>



class Lexer {	
	friend class Parser;
	public:
	enum token {X, Y, SIN, COS, PI, OPEN_PAR, CLOSE_PAR, TIMES, AVG, COMMA};
	explicit Lexer(std::istream& in);
	token next();
	token peek();
	std::streamoff count() const;
	void reset();


	private:
	std::istream& in;
	std::streamoff counter;
	token identifyToken(const std::string s);
	std::string extractString();
};



typedef std::vector<std::string> Exp;


class Parser {
    public:
	explicit Parser(std::istream& in);
	bool parse(Exp& exp);

	private:
    Lexer lexer;
	std::vector<std::streamoff> openParLocations;
	std::vector<Lexer::token> tokenVector;

 	const std::string tokenToText[10] = {"x", "y", "sin", "cos", "pi", "(", ")", "*", "AVG", ","};
 	bool checkSyntax();
 	bool checkAverage();
 	bool checkProduct();
 	bool checkSinCos();
 	bool checkXY();
 	bool infixToRPN(Exp& exp,std::vector<Lexer::token>& tokenVector); 

};





#endif
