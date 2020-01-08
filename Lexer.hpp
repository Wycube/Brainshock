#ifndef LEXER_HPP
#define LEXER_HPP

#include "language/Language.hpp"

namespace bs {

	class Lexer {
		Language *m_lang;

		public:

			void tokenize(Program& program, char *source);
			bool expr(Program& program);
			void optimize(Program& program);

			Lexer(Language& lang) {m_lang = lang;}
	};

}

#endif