#ifndef _PARSER_H
#define _PARSER_H 




class Lexer {
	public:
	enum token {X, Y, SIN, COS, PI, OPEN_PAR, CLOSE_PAR, TIMES, AVG, COMMA};
	explicit Lexer(std::istream& in);
	token next();
	token peek();
	std::streamoff count() const;
	void reset();
};

explicit Parser(std::istream& in);
bool parse(Exp& exp);

typedef std::vector<std::string> Exp;



#endif