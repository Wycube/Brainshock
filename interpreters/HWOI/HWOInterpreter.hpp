#ifndef HWOINTERPRETER_HPP
#define HWOINTERPRETER_HPP

#include "Lexer.hpp"
#include "../Memory.hpp"
#include <queue>

/** Standard Brainshock namespace */
namespace bs {

	/** Abstract Interpreter class */
	class HWInterpreter {
		protected:

		Tape m_tape;
		int m_cellPtr = 0;
		
		Program m_program;
		int m_instPtr = 0;

		virtual bool process(Program& program, const char *string) = 0; //Tokenize and other processing steps(expression, optimizing etc.)
		
		public:

		HWInterpreter() : m_tape(), m_program() {}

		bool loadProgram(const char *string) {return process(m_program, string);} //Loads program
		virtual bool step(int numSteps = 1) = 0; //Steps a specified number of steps
		virtual bool run() = 0; //Runs the program TODO: allow for certain speeds
	};


	/** Standard Version of Brainf, and I don't swear */
	namespace frick {

		/** Flag struct holding bools */
		struct StdFlags {
			bool numInput = false; //If input should be taken as numbers for 0-255
		};

		/** Heavy-Weight Interpreter for standard Brainf */
		class StdHWInterpreter : public HWInterpreter {
			private:

			bool m_debug = false;
			StdLexer m_lexer;
			std::queue<unsigned char> m_inBuffer;

			bool process(Program& program, const char *string) override;
			unsigned char getInput();

			public:

			StdFlags m_flags;

			StdHWInterpreter() : HWInterpreter() {}

			bool step(int numSteps = 1) override;
			bool run() override;

		};
	}
}

#endif