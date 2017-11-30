#ifndef _PARSER_H
#define _PARSER_H 
#include <string>
#include <vector>
#include <ios>




class Lexer {
	public:
	enum token {X, Y, SIN, COS, PI, OPEN_PAR, CLOSE_PAR, TIMES, AVG, COMMA};
	explicit Lexer(std::istream& in);
	token next();
	token peek();
	std::streamoff count() const;
	void reset();


	private:	
	std::istream in;
	std::streamoff counter = 0;

	token identifyToken(char* s);
};



typedef std::vector<std::string> Exp;


class Parser {
explicit Parser(std::istream& in);
bool parse(Exp& exp);
};





#endif