#include "Brainf.hpp"

BrainfInterpreter::BrainfInterpreter(size_t memSize) {
	m_memory = Tape(memSize);
}

/**
 * This steps the execution a certain number of steps, or to the
 * end whichever is first. 
 *
 * @param numInstructions The number of instructions to move forward by. 
 */
bool BrainfInterpreter::step(int numInstructions) {

}

/**
 * This is the run function, which run speed can be adjusted and is
 * controlled by some chrono utilities. It basically just steps and
 * checks the delta time, waits or steps again. It is not guaranteed
 * to go as fast as set but it will try. Also zero means as fast as possible.
 *
 * @param runSpeed The speed to run at in instructions per second
 */
bool BrainfInterpreter::run(float runSpeed) {

}

/**
 * This method is responsible for putting certain data into tokens
 * and validation check, for [] it can put relative jump information.
 */
bool BrainfInterpreter::expr() {

}

/**
 * This method will do the optimization like folding repetive 
 * instructions into one and other creative things I can find
 * or think of, without modifying the behavior.
 */
void BrainfInterpreter::preProcess() {
	//Optimizations *yay*
}