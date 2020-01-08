#ifndef LANGUAGE_HPP
#define LANGUAGE_HPP

#include <unordered_map>
#include "Instruction.hpp"
#include "../interpreters/HWI/Program.hpp"

namespace bs {

	/**
	 * A class representing a brainf "language", that is a set of instructions 
	 * in that language (there can't be duplicate instruction identifiers or they won't
	 * be added) and some functions.
	 */
	class Language {
		std::unordered_map<char*, Instruction_Def> m_instructions;
		const void (*m_optimize)(Program&);

		void default_optimize(Program& program) {}

		public:

		bool addInstruction(Instruction_Def newInst);
		Instruction_Def getInstruction(char *identifier);

		Language(const void (*optimize)(Program&) = {}) : m_optimize(optimize) {}
	};

}

#endif