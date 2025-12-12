#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

std::map<std::string, std::string> comp_table = {
	// a=0 (comp)
	{"0",   "0101010"},
	{"1",   "0111111"},
	{"-1",  "0111010"},
	{"D",   "0001100"},
	{"A",   "0110000"},
	{"!D",  "0001101"},
	{"!A",  "0110001"},
	{"-D",  "0001111"},
	{"-A",  "0110011"},
	{"D+1", "0011111"},
	{"A+1", "0110111"},
	{"D-1", "0001110"},
	{"A-1", "0110010"},
	{"D+A", "0000010"},
	{"D-A", "0010011"},
	{"A-D", "0000111"},
	{"D&A", "0000000"},
	{"D|A", "0010101"},
	// a=1 (comp)
	{"M",   "1110000"},
	{"!M",  "1110001"},
	{"-M",  "1110011"},
	{"M+1", "1110111"},
	{"M-1", "1110010"},
	{"D+M", "1000010"},
	{"D-M", "1010011"},
	{"M-D", "1000111"},
	{"D&M", "1000000"},
	{"D|M", "1010101"}
};

std::map<std::string, std::string> dest_table = {
	{"null", "000"}, // dest가 생략된 경우 (중요)
	{"M",    "001"},
	{"D",    "010"},
	{"MD",   "011"},
	{"A",    "100"},
	{"AM",   "101"},
	{"AD",   "110"},
	{"AMD",  "111"}
};

std::map<std::string, std::string> jump_table = {
	{"null", "000"}, // jump가 생략된 경우 (중요)
	{"JGT",  "001"},
	{"JEQ",  "010"},
	{"JGE",  "011"},
	{"JLT",  "100"},
	{"JNE",  "101"},
	{"JLE",  "110"},
	{"JMP",  "111"}
};

std::map<std::string, int> symbol_table = {
	{"R0", 0},
	{"R1", 1},
	{"R2", 2},
	{"R3", 3},
	{"R4", 4},
	{"R5", 5},
	{"R6", 6},
	{"R7", 7},
	{"R8", 8},
	{"R9", 9},
	{"R10", 10},
	{"R11", 11},
	{"R12", 12},
	{"R13", 13},
	{"R14", 14},
	{"R15", 15},
	{"SCREEN", 16384},
	{"KBD", 24576},
	{"SP", 0},
	{"LCL", 1},
	{"ARG", 2},
	{"THIS", 3},
	{"THAT", 4}
};

std::string DeciamlToBinary15(int n);
std::string CleanLine(std::string line);

int main() {
	std::string filename = "Pong.asm";

	std::ifstream input(filename);

	std::string line;

	std::vector<std::string> assembly;
	std::vector<std::string> machine_code;

	int rom_address = 0;

	while (std::getline(input, line)) { // 공백, 주석 제거 + Label 저장
		line = CleanLine(line);

		if (!line.length()) //White space
			continue;
		else if (line[0] == '(') {
			int label_end = line.find(")");

			std::string label = line.substr(1, label_end - 1);

			symbol_table.insert({ label, rom_address });

			continue;
		}

		assembly.push_back(line);
		rom_address++;
	}
	
	int ram_address = 16;

	for(std::string line : assembly) { // 정제된 어셈블리어 머신 코드로 전환
		std::string binary_code;

		if (line[0] == '@'){ // A Instruction
			int a_register;

			if (std::isdigit(line[1])) {
				a_register = std::stoi(line.substr(1));
			}
			else{
				std::string var = line.substr(1);

				if (!symbol_table.count(var))
					symbol_table[var] = ram_address++;
				
				a_register = symbol_table[var];
			}

			binary_code = "0" + DeciamlToBinary15(a_register);
		}
		else { // C Instruction

			int pos_equal = line.find("=");
			int pos_semicolon = line.find(";");

			bool is_equal = (pos_equal != std::string::npos);
			bool is_semicolon = (pos_semicolon != std::string::npos);

			std::string dest = "null";
			std::string jump = "null";

			if (is_equal)
				dest = line.substr(0, pos_equal);

			if (is_semicolon)
				jump = line.substr(pos_semicolon + 1);

			int comp_start = is_equal ? pos_equal + 1 : 0;
			int comp_len = is_semicolon ? pos_semicolon - comp_start : line.length() - comp_start;

			std::string comp = line.substr(comp_start, comp_len);

			binary_code = "111" + comp_table[comp] + dest_table[dest] + jump_table[jump];
		}

		machine_code.push_back(binary_code);
		rom_address++;
	}

	input.close();

	std::string result = filename.substr(0, filename.size() - 3) + "hack";

	std::ofstream output(result);

	for (int i = 0; i < machine_code.size(); i++) {
		output << machine_code[i];

		if (i != machine_code.size() - 1)
			output << "\n";
	}

	return 0;
}

std::string DeciamlToBinary15(int n) {
	std::string binary;

	for (int i = 16384; i >= 1; i /= 2) { // i = 2^14
		binary += '0' + n / i;
		n %= i;
	}

	return binary;
}

std::string CleanLine(std::string line) {
	int pos_comment = line.find("//");

	if (pos_comment != std::string::npos)
		line = line.substr(0, pos_comment);

	const std::string whitespace = " \t\n\r\f\v";

	int start = line.find_first_not_of(whitespace);

	if (start == std::string::npos)
		return "";

	int end = line.find_last_not_of(whitespace);

	return line.substr(start, (end + 1 - start));
}