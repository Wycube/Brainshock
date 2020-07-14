#ifndef BRAINF_HPP
#define BRAINF_HPP

#include "Interpreter.hpp"

namespace bs {

	class BrainfInterpreter : public Interpreter {
	public:

		BrainfInterpreter(std::size_t memSize = 30000);

		bool run(float runSpeed = 0) override;
		bool step() override;

	private:

		bool stepProcessed();
		bool stepUnprocessed();
	public:
		std::deque<std::size_t> m_jumpTable;
	private:
		bool expr() override;
		void preProcess() override;
	};

}

#endif //BRAINF_HPP
