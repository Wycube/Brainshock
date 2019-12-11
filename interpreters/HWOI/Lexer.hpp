#ifndef LEXER_HPP
#define LEXER_HPP

#include "Program.hpp"

namespace bs {

	/** Abstract Lexer class, for tokenization */
	class Lexer {
		public:

		Lexer() {}

		virtual void tokenize(Program& program, const char *string) = 0; //This method will construct a token[], and remove invalid Brainf symbols
		virtual bool expr(Program& program) = 0; //Checks if expressions in the token[] are valid
		virtual void optimize(Program& program) = 0; //Optimizes program to have same behavior, but less tokens
	};

	namespace frick {

		/** All the different token symbols as an enum */
		enum StdOperations {
			RSHIFT='>', LSHIFT='<', INCREMENT='+', DECREMENT='-', OPENLOOP='[', CLOSELOOP=']', OUTPUT='.', INPUT=',', DEBUG='#'
		};

		/** Implementation of the Lexer class for standard Brainf */
		class StdLexer : public Lexer {
			public:

			StdLexer() {}

			void tokenize(Program& program, const char *string) override;
			bool expr(Program& program) override;
			void optimize(Program& program) override;
		};

	}
}

#endif