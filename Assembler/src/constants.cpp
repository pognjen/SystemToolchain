#include <unordered_map>
#include <unordered_set>

std::unordered_map<std::string, int>  instructions_map =
{
	{"halt", 1},
	{"xchg", 2},
	{"int", 3},
	{"mov", 4},
	{"add", 5},
	{"sub", 6},
	{"mul", 7},
	{"div", 8},
	{"cmp", 9},
	{"not", 10},
	{"and", 11},
	{"or", 12},
	{"xor", 13},
	{"test", 14},
	{"shl", 15},
	{"shr", 16},
	{"push", 17},
	{"pop", 18},
	{"jmp", 19},
	{"jeq", 20},
	{"jne", 21},
	{"jgt", 22},
	{"call", 23},
	{"ret", 24},
	{"iret", 25},
	{"nop", 26}
};
std::unordered_set<std::string> directives_map =
{
	".text",
	".data",
	".bss",
	".section",
	".byte",
	".word",
	".align",
	".skip",
	".equ",
	".extern",
	".global",
	".end"
};
