#pragma once

//////////////////////////////////////////////////////////////////////////
//
//							SCRIPT INTERPRETER
//										by Matt
//
//	Interpretor for .script.
//	Variables are parsed first and added on the stack, then the code
//  continues its execution. While executing, it also checks every
//	opcode to be syntactically correct. Execution stops when any error
//	is thrown.
//
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <numeric>
#include <unordered_map>

#include "utils.h"
#include "opcodes.h"

class CScript {
public:
	CScript(const char* name) : m_iStackVarPointer(-1), m_iStackPointer(-1), m_bValidScript(false) {
		m_cScriptName = new char[strlen(name) + 1];
		strcpy((char*)m_cScriptName, name);
		try {
			if (script::file_exists(name)) {
				m_pStack = new int[MAX_STACK_SIZE];
				m_pStackVars = new Token[MAX_VARS];
				for (int i = 0; i < MAX_VARS; i++)
					m_pStackVars[i].m_cToken = nullptr;
				m_bValidScript = true;
			}
			else 
				throw std::exception("SCRIPT_NOT_FOUND");
		}
		catch (std::exception e) {
#if DEBUG == TRUE
				std::stringstream ss;
				ss << "Script '" << m_cScriptName << "' cannot be initialised! (" << e.what() << ")\n";
				std::cout << ss.str();
#endif
		}
	}

	~CScript() {
		if (m_cScriptName != nullptr) {
			delete[] m_cScriptName;
			m_cScriptName = nullptr;
		}
		if (m_pStack != nullptr) {
			delete[] m_pStack;
			m_pStack = nullptr;
		}
		if (m_pInstructions != nullptr) {
			delete[] m_pInstructions;
			m_pInstructions = nullptr;
		}
		if (m_pStackVars != nullptr) {
			delete[] m_pStackVars;
			m_pStackVars = nullptr;
		}
	}

	void Interpret() {
#if DEBUG == TRUE
		std::cout << "Interpreting script '" << m_cScriptName << "'...\n";
#endif
		try {
			if (!m_bValidScript)
				throw std::exception("INVALID_SCRIPT");

			m_pInstructions = new Token[MAX_INSTRUCTIONS];
			for (int i = 0; i < MAX_INSTRUCTIONS; i++)
				m_pInstructions[i].m_cToken = nullptr;
			int index = -1, instrLine = 0;

			std::ifstream file(m_cScriptName);
			if (file.is_open()) {
				std::string line;
				while (std::getline(file, line)) {
					std::istringstream iss(line);
					std::string token;
					instrLine++;

					std::getline(iss, token, ' ');

					if (token.compare(0, 2, "//") != 0) {
						Token* tok = new Token(token.c_str());
						tok->m_iInstructionLine = instrLine;

						while (std::getline(iss, token, ' ')) {
							tok->m_vParameters.push_back(token);
						}
						if (tok->m_eInstruction < 1000) {
							m_pStackVars[++m_iStackVarPointer] = *tok;
						}
						else if (tok->m_eInstruction > 1000) {
							m_pInstructions[++index] = *tok;
						}
					}
				}
				file.close();
				executeInstructions();
			}
		}
		catch (std::exception e) {
#if DEBUG == TRUE
			std::stringstream ss;
			ss << "Script '" << m_cScriptName << "' cannot be interpreted! (" << e.what() << ")\n";
			std::cout << ss.str();
#endif
		}
	}

	void executeInstructions() {
		if (m_pInstructions == nullptr)
			return;

		try {

			for (int i = 0; i < MAX_VARS; i++) {
				switch (m_pStackVars[i].m_eInstruction)
				{
					// VAR_TYPES
				case (int)opcodes::VAR_TYPES::INT:
				{
					if (m_pStackVars[i].m_vParameters.size() != 1) {
						std::stringstream ss;
						ss << "INT - Expected only one parameter (variable name) (line " << m_pStackVars[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (!(atoi(m_pStackVars[i].m_vParameters[0].c_str()) == 0 && m_pStackVars[i].m_vParameters[0][0] != '0')) {
						std::stringstream ss;
						ss << "INT - Invalid passed parameter (expected variable name != constant integer) (line " << m_pStackVars[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					m_pStack[++m_iStackPointer] = 0; // var declaration + initialization
					m_pStackVars[i].m_iStackPosition = m_iStackPointer;
					break;
				}
				}
			}

			for (int i = 0; i < MAX_INSTRUCTIONS; i++) {
				if (m_pInstructions[i].m_cToken == nullptr)
					break;

				switch (m_pInstructions[i].m_eInstruction)
				{
					// OPCODE_INSTRUCTIONS
				case (int)opcodes::OPCODE_INSTRUCTIONS::PUSH:
				{
					if (m_pInstructions[i].m_vParameters.size() != 1) {
						std::stringstream ss;
						ss << "PUSH - Expected only one parameter (constant integer) (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (atoi(m_pInstructions[i].m_vParameters[0].c_str()) == 0 && m_pInstructions[i].m_vParameters[0][0] != '0') {
						std::stringstream ss;
						ss << "PUSH - Invalid passed parameter (expected constant integer) (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					m_pStack[++m_iStackPointer] = atoi(m_pInstructions[i].m_vParameters[0].c_str());
					break;
				}
				case (int)opcodes::OPCODE_INSTRUCTIONS::POP:
				{
					if (m_pInstructions[i].m_vParameters.size() != 0) {
						std::stringstream ss;
						ss << "POP - Expected no parameters (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (m_iStackPointer == 0) {
						std::stringstream ss;
						ss << "POP - Expected at least 1 variable on the stack (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					m_iStackPointer--;
					break;
				}
				case (int)opcodes::OPCODE_INSTRUCTIONS::SUM:
				{
					if (m_pInstructions[i].m_vParameters.size() != 1) {
						std::stringstream ss;
						ss << "SUM - Expected only one parameter (destination variable) (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (!(atoi(m_pInstructions[i].m_vParameters[0].c_str()) == 0 && m_pInstructions[i].m_vParameters[0][0] != '0')) {
						std::stringstream ss;
						ss << "SUM - Invalid passed parameter (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (m_iStackPointer < 2) {
						std::stringstream ss;
						ss << "SUM - Expected at least 2 variables on the stack (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					const int _p1 = m_pStack[m_iStackPointer--];
					const int _p2 = m_pStack[m_iStackPointer--];

					bool found = false;
					for (int ii = 0; ii < MAX_VARS; ii++) {
						if (m_pStackVars[ii].m_cToken == nullptr)
							break;
						if (strcmp(m_pStackVars[ii].m_vParameters[0].c_str(), m_pInstructions[i].m_vParameters[0].c_str()) != 0)
							continue;
						m_pStack[m_pStackVars[ii].m_iStackPosition] = (_p1 + _p2);
						found = true;
						break;
					}
					if (!found) {
						std::stringstream ss;
						ss << "SUM - Destination variable not found! (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}
					break;
				}
				case (int)opcodes::OPCODE_INSTRUCTIONS::SUB:
				{
					if (m_pInstructions[i].m_vParameters.size() != 2) {
						std::stringstream ss;
						ss << "SUB - Expected only two parameters (destination variable, constant integer) (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (!(atoi(m_pInstructions[i].m_vParameters[0].c_str()) == 0 && m_pInstructions[i].m_vParameters[0][0] != '0')) {
						std::stringstream ss;
						ss << "SUB - Invalid passed parameter (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (atoi(m_pInstructions[i].m_vParameters[1].c_str()) == 0 && m_pInstructions[i].m_vParameters[1][0] != '0') {
						std::stringstream ss;
						ss << "PUSH - Invalid passed parameter (expected constant integer) (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					bool found = false;
					for (int ii = 0; ii < MAX_VARS; ii++) {
						if (m_pStackVars[ii].m_cToken == nullptr)
							break;
						if (strcmp(m_pStackVars[ii].m_vParameters[0].c_str(), m_pInstructions[i].m_vParameters[0].c_str()) != 0)
							continue;
						m_pStack[m_pStackVars[ii].m_iStackPosition] -= atoi(m_pInstructions[i].m_vParameters[1].c_str());
						found = true;
						break;
					}
					if (!found) {
						std::stringstream ss;
						ss << "SUB - Destination variable not found! (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}
					break;
				}
				case (int)opcodes::OPCODE_INSTRUCTIONS::MOV:
				{
					if (m_pInstructions[i].m_vParameters.size() != 2) {
						std::stringstream ss;
						ss << "MOV - Expected only two parameters (destination variable, constant integer) (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					bool found = false;
					for (int ii = 0; ii < MAX_VARS; ii++) {
						if (m_pStackVars[ii].m_cToken == nullptr)
							break;
						if (strcmp(m_pStackVars[ii].m_vParameters[0].c_str(), m_pInstructions[i].m_vParameters[0].c_str()) != 0)
							continue;
						m_pStack[m_pStackVars[ii].m_iStackPosition] = atoi(m_pInstructions[i].m_vParameters[1].c_str());
						found = true;
						break;
					}
					if (!found) {
						std::stringstream ss;
						ss << "MOV - Destination variable not found! (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}
					break;
				}
				case (int)opcodes::OPCODE_INSTRUCTIONS::PRINT:
				{
					if (m_pInstructions[i].m_vParameters.size() < 1) {
						std::stringstream ss;
						ss << "PRINT - No parameter provided (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					std::string joinedString = std::accumulate(
						m_pInstructions[i].m_vParameters.begin(), m_pInstructions[i].m_vParameters.end(), std::string(),
						[](const std::string& a, const std::string& b) {
							return a + (a.empty() ? "" : " ") + b;
						}
					);

					if (joinedString.front() == '`' && joinedString.back() != '`') {
						std::stringstream ss;
						ss << "PRINT - Invalid string passed as parameter (line " << m_pInstructions[i].m_iInstructionLine << ")";
						throw std::exception(ss.str().c_str());
					}

					if (joinedString._Equal("system::newline"))
					{
						std::cout << "\n";
						break;
					}

					if (joinedString.front() == '`' && joinedString.back() == '`') {
						std::cout << CYAN_COLOR << joinedString.substr(1, joinedString.length() - 2) << RESET_COLOR;
					}
					else {
						const int val = atoi(m_pInstructions[i].m_vParameters[0].c_str());
						if (!(val == 0 && m_pInstructions[i].m_vParameters[0][0] != '0'))
							std::cout << CYAN_COLOR << val << RESET_COLOR;
						else {
							bool found = false;
							for (int ii = 0; ii < MAX_VARS; ii++) {
								if (m_pStackVars[ii].m_cToken == nullptr)
									break;
								if (strcmp(m_pStackVars[ii].m_vParameters[0].c_str(), m_pInstructions[i].m_vParameters[0].c_str()) != 0)
									continue;
								
								std::cout << CYAN_COLOR << m_pStack[m_pStackVars[ii].m_iStackPosition] << RESET_COLOR;
								found = true;
								break;
							}
							if (!found) {
								std::stringstream ss;
								ss << "PRINT - Variable '" << m_pInstructions[i].m_vParameters[0] << "' not found (line " << m_pInstructions[i].m_iInstructionLine << ")";
								throw std::exception(ss.str().c_str());
							}
						}
					}
					break;
				}
				}
			}
		}
		catch (std::exception e) {
			m_bValidScript = false;
			std::stringstream ss;
			ss << RED_COLOR << "\nError while executing instructions: " << e.what() << RESET_COLOR << "\n";
			std::cout << ss.str().c_str();
		}
	}

	void printVars() {
		if (m_pStackVars == nullptr || !m_bValidScript)
			return;
		for (int i = 0; i < MAX_VARS; i++) {
			if (m_pStackVars[i].m_cToken == nullptr)
				break;
			std::cout << "(" << i << ") " << "Variable: " << m_pStackVars[i].m_cToken << " - Type: " << m_pStackVars[i].m_eInstruction << " - Hash: " << m_pStackVars[i].m_uHash << " - VarName: " << m_pStackVars[i].m_vParameters.at(0) << "\n";
		}
	}

	void printInstructions() {
		if (m_pInstructions == nullptr || !m_bValidScript)
			return;
		for (int i = 0; i < MAX_INSTRUCTIONS; i++) {
			if (m_pInstructions[i].m_cToken == nullptr)
				break;
			if (m_pInstructions[i].m_eInstruction == 0 || m_pInstructions[i].m_eInstruction < 1000)
				continue;
			std::cout << "(" << i << ") " << "Instruction: " << m_pInstructions[i].m_cToken << " - Type: " << m_pInstructions[i].m_eInstruction << " - Hash: " << m_pInstructions[i].m_uHash << " - Params: " << m_pInstructions[i].m_vParameters.size();
			if (m_pInstructions[i].m_vParameters.size() > 0) {
				std::cout << " {";
				for (auto param : m_pInstructions[i].m_vParameters) {
					std::cout << " " << param;
				}
				std::cout << " } ";
			}
			std::cout << "\n";
		}
	}

	void printStack() {
		if (m_pStack == nullptr || !m_bValidScript)
			return;
		for (uint32_t i = 0; i <= m_iStackPointer; i++) {
			std::cout << "Index (" << i << ") - " << m_pStack[i] << "\n";
		}
	}

	// ACCESSORS
	const char* getScriptName() { return m_cScriptName; }
	int* getStackPointer() { return m_pStack; }
	bool isValidScript() { return m_bValidScript; }

private:
	bool m_bValidScript;
	char* m_cScriptName;

	int* m_pStack;
	uint32_t m_iStackPointer;
	Token* m_pInstructions;
	Token* m_pStackVars;
	uint32_t m_iStackVarPointer;
};