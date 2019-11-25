#ifndef LEXER_H
#define LEXER_H

namespace bs {
	class BasicToken {
		public:
		const char m_symbol;
		int m_data;

		BasicToken(char symbol, int data = 0) : m_symbol(symbol), m_data(data) {}
	};

	class Sequence {
		public:
		BasicToken *m_seq;
		int m_len;

		Sequence(BasicToken *sequence, int length) : m_seq(sequence), m_len(length) {};
		~Sequence() {delete[] m_seq;}
	};

	class BasicLexer {
		public:
		virtual Sequence tokenize(const char *program) = 0;
		virtual Sequence expr(Sequence seq) = 0;
		virtual Sequence optimize(Sequence seq) = 0;
	};

	namespace frick { //This is standard brainf*** but I don's swear
		enum Token {
			RSHIFT='>', LSHIFT='<', INCREMENT='+', DECREMENT='-', OPENLOOP='[', CLOSELOOP=']', OUTPUT='.', INPUT=',', DEBUG='#'
		};
		
		class StdToken : public BasicToken {
			public:
			const char *m_desc;

			StdToken(const char symbol = ' ', const char *desc = "", int data = 0) : BasicToken(symbol, data), m_desc(desc) {}
		
			StdToken operator =(StdToken other) {
				return StdToken(other.m_symbol, other.m_desc, other.m_data);
			}
		};

		class StdLexer : public BasicLexer {
			public:
			Sequence tokenize(const char *program);
			Sequence expr(Sequence seq);
			Sequence optimize(Sequence seq);
		};
	}
}
#endif