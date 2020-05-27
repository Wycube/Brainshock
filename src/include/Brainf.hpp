#ifndef BRAINF_HPP
#define BRAINF_HPP

#include "Interpreter.hpp"

namespace bs {

	class BrainfInterpreter : public Interpreter {
	public:

		BrainfInterpreter(size_t memSize = 30000);

		bool run(float runSpeed = 0);
		bool step(int numInstructions = 1);

	private:

		bool expr();
		void preProcess();
	};

}

#endif //BRAINF_HPP