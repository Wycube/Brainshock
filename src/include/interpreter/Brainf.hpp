#ifndef BRAINF_HPP
#define BRAINF_HPP

#include "Interpreter.hpp"

namespace bs {

	class BrainfInterpreter : public Interpreter {
	public:

		BrainfInterpreter(std::ostream &stream = std::cout, std::size_t memSize = 30000);

		bool run(float runSpeed = 0) override;
		bool step() override;

	private:

		std::deque<std::size_t> m_jumpTable;
		std::ostream &m_stream;

		bool stepProcessed();
		bool stepUnprocessed();
		bool expr() override;
		void preProcess(unsigned int optimization = 2) override;
	};

}

#endif //BRAINF_HPP
