#pragma once

class Token {
public:
	const char* m_cToken;
	uint32_t m_uHash;
	int m_eInstruction;
	int m_iStackPosition; 
	int m_iInstructionLine;
	std::vector<std::string> m_vParameters;

	Token() {
		m_cToken = nullptr;
		m_iStackPosition = -1;
		m_iInstructionLine = 0;
	}

	Token(const char* name) : m_iInstructionLine(0), m_iStackPosition(-1), m_eInstruction((int)opcodes::OPCODE_INSTRUCTIONS::UNK) {
		const int length = (int)strlen(name);

		m_cToken = new char[strlen(name) + 1];
		strcpy((char*)m_cToken, name);

		m_uHash = script::joaat(name, length);
		for (const auto& pair : opcodes::opcodeHashes) {
			if (pair.second == m_uHash) {
				m_eInstruction = pair.first;
				break;
			}
		}
		if (m_eInstruction == (int)opcodes::OPCODE_INSTRUCTIONS::UNK) {
			for (const auto& pair : opcodes::varsHashes) {
				if (pair.second == m_uHash) {
					m_eInstruction = pair.first;
					break;
				}
			}
		}
	}

	~Token() {
		if (m_cToken != nullptr) {
			delete[] m_cToken;
			m_cToken = nullptr;
		}
	}
};