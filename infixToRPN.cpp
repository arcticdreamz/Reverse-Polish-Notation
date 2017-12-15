bool Parser::infixToRPN(Exp& exp,std::vector<Lexer::token>& tokenVector){

 	Exp operatorStack;
 	
 	exp.clear();

 	//Variables to keep track of the parentheses for AVG
 	int parentheses = 0;
 	std::vector<int> avgPar;

 	auto tok = tokenVector.begin();
 	while(tok != tokenVector.end()){

		if(*tok == Lexer::PI || *tok == Lexer::X || *tok == Lexer::Y){
			//Pushback enter
			if(!exp.empty()){
				if(exp.back() == "x" || exp.back() =="y"|| exp.back() == "pi")
					exp.push_back("enter");
			}
			//Push the token onto the expression
			exp.push_back(tokenToText[*tok]);

		}
		else if(*tok == Lexer::AVG){
			//Remember the number of parentheses
			avgPar.push_back(parentheses);
		}
		else if(*tok == Lexer::COMMA){
			//Do nothing
		}
		else if(*tok == Lexer::OPEN_PAR){
			parentheses++;
			operatorStack.push_back(tokenToText[*tok]);
		}
		else if(*tok == Lexer::CLOSE_PAR){
			parentheses--;

			//Pop the operators onto the queue as long as
			//we don't encounter an OPEN_PAR
			while(!operatorStack.empty() && operatorStack.back() != tokenToText[Lexer::OPEN_PAR]){
				exp.push_back(operatorStack.back());
				operatorStack.pop_back();
			}

			//Pop the (eventual) OPEN_PAR
			if(!operatorStack.empty())
				operatorStack.pop_back();

			//If parentheses is the same as one of avgPar, we must add "+2/"
			//(Same method as if the one for the other operators)
			if(std::find(avgPar.begin(), avgPar.end(), parentheses) != avgPar.end()){
				exp.push_back("+");
				exp.push_back("2");
				exp.push_back("/");
			}
			

		//For operators
		}else{
			//We pop the other operators from the stack until empty or until
			//we encounter OPEN_PAR or "+" (because smaller precedence)
			while(!operatorStack.empty() && operatorStack.back() != tokenToText[Lexer::OPEN_PAR]){
				exp.push_back(operatorStack.back());
				operatorStack.pop_back();
			}
			//Pop the (eventual) OPEN_PAR
			if(!operatorStack.empty())
				operatorStack.pop_back();

			//Push the read operator onto the operator stack
			operatorStack.push_back(tokenToText[*tok]);
		}

		++tok;
	}

	// while there are still operator tokens on the stack, pop them onto the expression
	while(!operatorStack.empty()){
		exp.push_back(operatorStack.back());
		operatorStack.pop_back();
	}

	return true;
}