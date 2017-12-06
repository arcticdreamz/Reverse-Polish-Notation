#ifndef _PARSER_H
#define _PARSER_H
#include <string>
#include <vector>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>



class Lexer {	public:
	enum token {X, Y, SIN, COS, PI, OPEN_PAR, CLOSE_PAR, TIMES, AVG, COMMA};
	explicit Lexer(std::istream& in);
	token next();
	token peek();
	std::streamoff count() const;
	void reset();


	private:
	std::istream& in;
	std::streamoff counter = 0;

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

 	void checkAverage(Exp& exp);
 	void checkProduct(Exp& exp);
 	void checkSinCos(Exp& exp);
};





#endif
