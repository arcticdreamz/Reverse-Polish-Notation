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


	private:	
	std::istream in;
	std::streamoff count = 0;

	token identifyToken(std::string s) {
		if(s == 'x')
			return token::X;
		if(s == 'Y')
			return token::Y;

	}
};

explicit Parser(std::istream& in);
bool parse(Exp& exp);

typedef std::vector<std::string> Exp;



#endif