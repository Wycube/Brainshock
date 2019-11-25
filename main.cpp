#include <iostream>
#include "interpreter/Interpreter.h"

int main() {
	std::cout << "Copyright Spencer Burton" << std::endl;
	std::cout << "Brainshock Alpha 1" << std::endl;

	std::string program;

	std::cout << "Input Program:" << std::endl;

	std::cin >> program;

	bs::Interpreter interp = bs::Interpreter(program.c_str(), 30000);

	interp.run();
}