#ifndef _PARSER_H
#define _PARSER_H
#include <string>
#include <vector>
#include <ios>
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
 	Exp operatorStack;
	std::vector<std::streamoff> openParLocations;

 	std::string tokenToText[10] = {"x", "y", "sin", "cos", "pi", "(", ")", "*", "AVG", ","};
 	void checkAverage(Exp& exp,Exp& operatorStack);
 	void checkProduct(Exp& exp,Exp& operatorStack);
 	void checkSinCos(Exp& exp,Exp& operatorStack);
 	void checkXY(Exp& exp,Exp& operatorStack);
 	void infixToRPN(Exp& exp, Lexer::token,Exp& operatorStack); //maybe reference for token, need to think
};





#endif
