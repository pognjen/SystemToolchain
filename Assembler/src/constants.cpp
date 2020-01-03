#include <unordered_map>
#include <unordered_set>

std::unordered_map<std::string, int> instructions_map = { { "halt", 1 }, { "xchg", 2 },
    { "int", 3 }, { "mov", 4 }, { "add", 5 }, { "sub", 6 }, { "mul", 7 }, { "div", 8 },
    { "cmp", 9 }, { "not", 10 }, { "and", 11 }, { "or", 12 }, { "xor", 13 }, { "test", 14 },
    { "shl", 15 }, { "shr", 16 }, { "push", 17 }, { "pop", 18 }, { "jmp", 19 }, { "jeq", 20 },
    { "jne", 21 }, { "jgt", 22 }, { "call", 23 }, { "ret", 24 }, { "iret", 25 }, { "nop", 26 } };
std::unordered_set<std::string> directives_map = { ".text", ".data", ".bss", ".section", ".byte",
    ".word", ".align", ".skip", ".equ", ".extern", ".global", ".end" };

std::unordered_map<std::string, int> instructions_op_numbers = { { "halt", 0 }, { "xchg", 2 },
    { "int", 1 }, { "mov", 2 }, { "add", 2 }, { "sub", 2 }, { "mul", 2 }, { "div", 2 },
    { "cmp", 2 }, { "not", 1 }, { "and", 2 }, { "or", 2 }, { "xor", 2 }, { "test", 2 },
    { "shl", 2 }, { "shr", 2 }, { "push", 1 }, { "pop", 1 }, { "jmp", 1 }, { "jeq", 1 },
    { "jne", 1 }, { "jgt", 1 }, { "call", 1 }, { "ret", 0 }, { "iret", 0 }, { "nop", 0 } };
