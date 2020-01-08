#include "Language.hpp"
#include <unordered_map>

namespace bs {

	/**
	 * Used for adding new instructions to this language.
	 *
	 * @param newInst The new instruction to add
	 *
	 * @return True if added, false if identifier the same as another instruction.
	 */
	bool Language::addInstruction(Instruction_Def newInst) {
		std::unordered_map<char*, Instruction_Def>::iterator itr;


		m_instructions.push_back(newInst);

		return true;
	}

	/**
	 * Returns an instruction definition based on the index, no boundary
	 * checking is done.
	 *
	 * @param identifier The string identifier used to retrieve the instruction_def
	 *
	 * @return An instruction definition from this languages instruction list.
	 */
	Instruction_Def Language::getInstruction(char *identifier) {
		return m_instructions[identifier];
	}

}