#ifndef BRAINF_HPP
#define BRAINF_HPP

#include "Interpreter.hpp"

namespace bs {

	class BrainfInterpreter : public Interpreter {
	public:

		BrainfInterpreter(std::size_t memSize = 30000);

		bool run(float runSpeed = 0) override;
		bool step(std::size_t numInstructions = 1) override;

	private:

		bool stepProcessed(size_t numInstructions);
		bool stepUnprocessed(size_t numInstructions);

		std::deque<std::size_t> m_jumpTable;

		bool expr() override;
		void preProcess() override;
	};

}

#endif //BRAINF_HPP
