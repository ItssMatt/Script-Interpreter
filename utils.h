#pragma once

#define DEBUG               TRUE
#define MAX_HEAP_SIZE       1024				    // SIZE: 1024 * 4 BYTES
#define MAX_INSTRUCTIONS    1024
#define MAX_VARS            100                     // SIZE: 100 * 4 BYTES

#define RESET_COLOR         "\x1b[0m"   // Reset to default color
#define RED_COLOR           "\x1b[31m"  // Red color
#define CYAN_COLOR          "\x1b[36m"  // Cyan text

namespace script {

	int file_exists(const char* name)
	{
		struct stat   buffer;
		return (stat(name, &buffer) == 0);
	}

    constexpr uint32_t joaat(const char* key, size_t length) {
        uint32_t hash = 0;

        for (size_t i = 0; i < length; ++i) {
            hash += static_cast<uint32_t>(key[i]);
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }

        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);

        return hash;
    }
}

namespace opcodes {

    enum class VAR_TYPES {
        UNK = 0,
        INT = 1
    };
    
	enum class OPCODE_INSTRUCTIONS {
		UNK = 0,
		PUSH = 1001,
		POP = 1002,
		SUM = 1003,
        SUB = 1004,
        PRINT = 1005,
        MOV = 1006
	};

    std::unordered_map<int, uint32_t> opcodeHashes = {
        { (int)opcodes::OPCODE_INSTRUCTIONS::UNK, script::joaat("UNK", 3) },
        { (int)opcodes::OPCODE_INSTRUCTIONS::PUSH, script::joaat("PUSH", 4) },
        { (int)opcodes::OPCODE_INSTRUCTIONS::POP, script::joaat("POP", 3) },
        { (int)opcodes::OPCODE_INSTRUCTIONS::SUM, script::joaat("SUM", 3) },
        { (int)opcodes::OPCODE_INSTRUCTIONS::SUB, script::joaat("SUB", 3) },
        { (int)opcodes::OPCODE_INSTRUCTIONS::PRINT, script::joaat("PRINT", 5) },
        { (int)opcodes::OPCODE_INSTRUCTIONS::MOV, script::joaat("MOV", 3) }
    };

    std::unordered_map<int, uint32_t> varsHashes = {
        { (int)opcodes::VAR_TYPES::UNK, script::joaat("UNK", 3) },
        { (int)opcodes::VAR_TYPES::INT, script::joaat("INT", 3) },
    };
}