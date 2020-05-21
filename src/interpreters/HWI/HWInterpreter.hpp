#ifndef HWOINTERPRETER_HPP
#define HWOINTERPRETER_HPP

#include "Lexer.hpp"
#include "../Memory.hpp"
#include <queue>

//Used for run method, means just run as fast as possible
#define IPS_FAST 0

/** Standard Brainshock namespace */
namespace bs {

	/** Abstract Interpreter class */
	class HWInterpreter {
		private:

		virtual bool process(Program &program, const char *string) = 0; //Tokenize and other processing steps(expression, optimizing etc.)
		
		public:

		Tape m_tape;
		int m_cellPtr = 0;
		
		Program m_program;
		int m_instPtr = 0;

		bool loadProgram(const char *string) {return process(m_program, string);} //Loads program
		virtual bool step(int numSteps = 1) = 0; //Steps a specified number of steps
		virtual void run(float ips = IPS_FAST) = 0; //Runs the program, allows for certain speeds ips = instructions per second

		HWInterpreter() : m_tape(), m_program() {}
	};


	/** Standard Version of Brainf, and I don't swear */
	namespace frick {

		/** Flag struct holding bools */
		struct StdFlags {
			bool numInput = false; //If input should be taken as numbers for 0-9
			bool optimize = true; //Runs the program through the optimize function when processing
		};

		/** Heavy-Weight Interpreter for standard Brainf */
		class StdHWInterpreter : public HWInterpreter {
			private:

			bool m_debug = false;
			Lexer *m_lexer;
			std::queue<unsigned char> m_inBuffer;

			bool process(Program &program, const char *string) override;
			unsigned char getInput();

			public:

			StdFlags m_flags;

			StdHWInterpreter(int lexer = 0) : HWInterpreter() {
				if(lexer == 0) {
					m_lexer = new StdLexer();
				} else if(lexer == 1) {
					m_lexer = new EffLexer();
				}
			}

			~StdHWInterpreter() {delete m_lexer;}

			bool step(int numSteps = 1) override;
			void run(float ips = IPS_FAST) override;

		};
	}
}

#endif