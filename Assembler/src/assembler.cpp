#include "assembler.h"
#include "error.h"
#include <sstream>
#include <iomanip>
#define WORD_SIZED_OPERANDS_MASK 4
#define IMMED_MASK 0
#define REGDIR_MASK 1
#define REGIND_MASK 2
#define REGIND_DISP_8_MASK 3
#define REGIND_DISP_16_MASK 4
#define MEMDIR_MASK 5
#define PC 7

extern std::unordered_map<std::string, int>  instructions_map;
extern std::unordered_set<std::string>  directives_map;
extern std::unordered_map<std::string, int> instructions_op_numbers;
void SymbolTable::insert_symbol(std::string name,int value,int visibility,std::string section,int import_export)
{
	SymbolTableNode* node = new SymbolTableNode(name,value,visibility,section,import_export);
	symtab.push_back(node);
}
SymbolTableNode* SymbolTable::find_symbol(std::string name)
{
	for (int i=0;i<symtab.size();i++)
	{
		if ((symtab[i])->name == name) return symtab[i];
	}
	return 0;
}

void SectionHeaderTable::insert_sh_node(std::string name,unsigned size,char rwx)
{
	SectionHeaderTableNode* sh_node = new SectionHeaderTableNode(name,size,rwx);
	shtable.push_back(sh_node);
}

SectionHeaderTableNode* SectionHeaderTable::find_sh_node(std::string name)
{
	for (int i=0;i<shtable.size();i++)
	{
		if (shtable[i]->name == name) return shtable[i];
	}
	return 0;
}

void Assembler::fp_label_handler()
{
	if (line_iterator->label != "")
	{
		if (current_section == "")
		{
			Error::symbol_not_in_section(current_section, line_iterator->label_src_line);
		}
		
		SymbolTableNode* temp_node = symtab.find_symbol(line_iterator->label);
		if (temp_node == 0)
		{
			symtab.insert_symbol(line_iterator->label,LC,LOCAL,current_section,NONE);
		}
		else
		{
			if (temp_node->section == "UNDEF")
			{
				if (temp_node->import_export == IMPORTED)
				{
					Error::imported_symbol_defined(temp_node->name, line_iterator->label_src_line);
				}
				else
				{
					temp_node->section = current_section;
					temp_node->value = LC;
				}
			}
			else
				Error::multiple_definition(temp_node->name);
		}
	}
}
void Assembler::fp_global_handler()
{
	/*if (current_section == "")
	{
		Error::global_not_in_section(line_iterator->src_line);
	}*/

	if (line_iterator->operands.size() == 0)
	{
		Error::global_requires_operands(line_iterator->src_line);
	}
	
	for (auto& it:line_iterator->operands)
	{
		if (it.type == ABS)
		{
			SymbolTableNode* temp_node = symtab.find_symbol(it.symbol);
			if (temp_node != 0)
			{
				if (temp_node->section == "UNDEF") Error::multiple_definition(temp_node->name);
				else
				{
					temp_node->visibility = GLOBAL;
				}
			}
			else
				symtab.insert_symbol(it.symbol,0,GLOBAL,"UNDEF",EXPORTED);
		}
		else
			Error::invalid_operand_type(line_iterator->src_line);
	}
}

void Assembler::fp_extern_handler()
{
	/*if (current_section == "")
	{
		Error::extern_not_in_section(line_iterator->src_line);
	}*/
	if (line_iterator->operands.size() == 0)
	{
		Error::global_requires_operands(line_iterator->src_line);
	}
	for (auto& it:line_iterator->operands)
	{
		if (it.type == ABS)
		{
			SymbolTableNode* temp_node = symtab.find_symbol(it.symbol);
			if (temp_node != 0)
			{
				Error::multiple_definition(temp_node->name);
			}
			else
				symtab.insert_symbol(it.symbol,0,GLOBAL,"UNDEF",IMPORTED);
		}
		else
			Error::invalid_operand_type(line_iterator->src_line);
	}
}

void Assembler::fp_skip_handler()
{
	if (current_section == "")
	{
		Error::symbol_not_in_section("",line_iterator->src_line);
	}
	if (line_iterator->operands.size() != 1)
	{
		Error::skip_requires_one_operand(line_iterator->src_line);
	}
	if (line_iterator->operands.front().type != IMMED || line_iterator->operands.front().literal < 0)
	{
		Error::skip_requires_immediate_value(line_iterator->src_line);
	}
	else
		LC = LC + line_iterator->operands.front().literal;
}

void Assembler::fp_byte_handler()
{
	if (current_section == "")
	{
		Error::directive_not_in_section(".byte",line_iterator->src_line);
	}
	if (line_iterator->operands.size() < 1)
	{
		Error::directive_requires_one_operand(".byte",line_iterator->src_line);
	}
	LC = LC + line_iterator->operands.size();
}

void Assembler::fp_word_handler()
{
	if (current_section == "")
	{
		Error::directive_not_in_section(".word", line_iterator->line_number);
	}
	if (line_iterator->operands.size() < 1)
	{
		Error::directive_requires_one_operand(".word", line_iterator->src_line);
	}
	LC = LC + 2*line_iterator->operands.size();
}

void Assembler::fp_align_handler()
{
	if (current_section == "")
	{
		Error::directive_not_in_section(".align", line_iterator->line_number);
	}
	if (line_iterator->operands.size() != 1)
	{
		Error::directive_requires_one_operand(".align", line_iterator->src_line);
	}
	if (line_iterator->operands.front().type != IMMED || line_iterator->operands.front().literal < 0)
	{
		Error::directive_requires_one_operand(".align", line_iterator->src_line);
	}
	unsigned int mask = ~((~0) << line_iterator->operands.front().literal);
	while (mask & LC)
	{
		LC++;
	}
}
void Assembler::fp_section_handler()
{
	if (current_section.size() > 0)
	{
		shtab.insert_sh_node(current_section,LC,current_rwx);
	}
	if (line_iterator->operands.size() > 0)
	{
		Error::section_operands(line_iterator->src_line);
	}
	current_section = line_iterator->directive;	
	LC = 0;
	if (current_section == ".text") current_rwx = 7;
	else
		if (current_section == ".bss") current_rwx = 4;
	else
		if (current_section == ".data") current_rwx = 6;

	SymbolTableNode* temp_node = symtab.find_symbol(current_section);
	if (temp_node != 0)
	{
		Error::multiple_definition(temp_node->name);
	}
	else
		symtab.insert_symbol(current_section,0,LOCAL,current_section,NONE);
}
void Assembler::fp_custom_section_handler()
{
	if (current_section != "")
	{
		shtab.insert_sh_node(current_section,LC,current_rwx);
	}
	if (line_iterator->operands.size() < 1)
	{
		Error::section_requires_one_operand(line_iterator->src_line);
	}
	current_section = line_iterator->operands.front().symbol;
	LC = 0;
	current_rwx = 0;
	if (line_iterator->operands.size() == 2)
	{
		std::string rwx = line_iterator->operands.back().symbol;

		for (int i = 0; i < rwx.size(); i++)
		{
			if (rwx[i] == 'R' || rwx[i] == 'r') current_rwx = current_rwx | R_MASK;
			if (rwx[i] == 'W' || rwx[i] == 'w') current_rwx = current_rwx | W_MASK;
			if (rwx[i] == 'X' || rwx[i] == 'x') current_rwx = current_rwx | X_MASK;
		}
	}
	else
		current_rwx = 7;
	SymbolTableNode* temp_node = symtab.find_symbol(current_section);
	if (temp_node != 0)
	{
		Error::multiple_definition(temp_node->name);
	}
	else
		symtab.insert_symbol(current_section,0,LOCAL,current_section,NONE);
}



void Assembler::fp_equ_handler()
{
	if (current_section == "")
	{
		Error::directive_not_in_section(".equ", line_iterator->src_line);
	}
	if (line_iterator->operands.size() != 2)
	{
		Error::directive_requires_two_operands(".equ", line_iterator->src_line);
	}
	
	if (line_iterator->operands.front().type != ABS || line_iterator->operands.back().type != IMMED)
	{
		Error::invalid_operand_type(line_iterator->src_line);
	}
	
	SymbolTableNode* temp_node = symtab.find_symbol(line_iterator->operands.front().symbol);
	if (temp_node == 0)
	{
		symtab.insert_symbol(line_iterator->operands.front().symbol,line_iterator->operands.back().literal,LOCAL,"ABS",NONE);
	}
	else
	{
		if (temp_node->section == "UNDEF")
		{
			if (temp_node->import_export == IMPORTED)
			{
				Error::imported_symbol_defined(temp_node->name, line_iterator->src_line);
			}
			else
			{
				temp_node->section = "ABS";
				temp_node->value = line_iterator->operands.back().literal;
			}
		}
		else
			Error::multiple_definition(temp_node->name);
	}
}
void Assembler::fp_instruction_handler()
{
	if (current_section != ".text")
	{
		Error::symbol_not_in_section("neki_simbol", line_iterator->src_line);
	}
	LC = LC + 1; // InstrDesc

	if (line_iterator->operands.size() > 2) Error::wrong_number_of_operands(line_iterator->instruction, line_iterator->src_line);

	for (auto& op: line_iterator->operands)
	{
		if (op.type == IMMED)
		{
			if (op.is_byte) LC = LC + 2;
			else
				LC = LC + 3;
		}
		else
			if (op.type == IMMED_SYM_VALUE || op.type == PCREL || op.type == REGINDDISP_SYM_VALUE || op.type == MEMDIR || op.type == ABS)
			{
				LC = LC + 3;
			}
			else
				if (op.type == REGDIR)
				{
					LC = LC + 1;
				}
				else
					if (op.type == REGINDDISP_IMMED)
					{
						if (op.literal == 0) LC = LC + 1;
						else
							if (op.is_byte) LC = LC + 2;
							else
								LC = LC + 3;
					}			
	}
}
void Assembler::fp_end_handler()
{
	if (current_section != "")
	{
		shtab.insert_sh_node(current_section, LC, current_rwx);
	}
}
std::string Assembler::word_to_hex(int val)
{
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(4) << std::hex << val;
	std::string result(stream.str());
	return result.substr(result.size() - 4);
}
std::string Assembler::byte_to_hex(int val)
{
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(2) << std::hex << val;
	std::string result(stream.str());
	return result.substr(result.size()-2);
}
void Assembler::first_pass()
{
	while (line_iterator != line_list.end())
	{
		fp_label_handler();
		
		if (line_iterator->is_directive)
		{
			(this->*(fp_map[line_iterator->directive]))();
		}
		else
			if (line_iterator->is_instruction)
			{
				fp_instruction_handler();
			}
		std::list<Line>::iterator prev_iterator = line_iterator;
		line_iterator++;
		if (line_iterator == line_list.end() && prev_iterator->directive != ".end")
		{
			fp_end_handler();
		}
	}

	for (int i = 0; i < symtab.symtab.size(); i++)
	{
		if (symtab.symtab[i]->import_export == EXPORTED && symtab.symtab[i]->section == "UNDEF")
		{
			Error::symbol_not_defined(symtab.symtab[i]->name);
		}
	}
}
void Assembler::sp_section_handler()
{
	current_section = line_iterator->directive;
	SectionHeaderTableNode* temp = shtab.find_sh_node(current_section);
	current_rwx = temp->rwx;
	LC = 0;
}
void Assembler::sp_custom_section_handler()
{
	current_section = line_iterator->operands.front().symbol;
	SectionHeaderTableNode* temp = shtab.find_sh_node(current_section);
	current_rwx = temp->rwx;
	LC = 0;
}
void Assembler::sp_skip_handler()
{
	int i = line_iterator->operands.front().literal;
		while (i > 0)
		{
			if (current_section == ".text")
			{
				sections[current_section] += byte_to_hex(instructions_map["nop"] << 3)+'\n';
			}
			else
			{
				sections[current_section] += byte_to_hex(0);
			}
			LC = LC + 1;
			i--;
		}
}

void Assembler::sp_align_handler()
{
	unsigned int mask = ~((~0) << line_iterator->operands.front().literal);
		while (mask & LC)
		{
			if (current_section == ".text")
			{
				sections[current_section] += byte_to_hex(instructions_map["nop"] << 3)+'\n';
			}
			else
			{
				sections[current_section] += byte_to_hex(0);
			}
			LC = LC + 1;
		}
}
void Assembler::sp_word_handler()
{
	if (current_rwx & W_MASK)
	{
		for (auto& it : line_iterator->operands)
		{
			if (it.type != IMMED && it.type != ABS)
			{
				Error::invalid_operand_type(line_iterator->src_line);
			}
			else
				if (it.type == IMMED)
				{
					std::string temp = word_to_hex(it.literal);
					sections[current_section] += temp.substr(temp.size() - 2);
					sections[current_section] += temp.substr(0, 2);
						
				}
				else
					{
						SymbolTableNode* temp_ptr = symtab.find_symbol(it.symbol);
						std::string temp = word_to_hex(temp_ptr->value);
						if (current_section == ".data")
						{
							sections[current_section] += temp.substr(temp.size() - 2); //lower byte
							sections[current_section] += temp.substr(0, 2); //higher byte
							if (temp_ptr->section != "ABS")
							{
								reltab.insert(current_section, sections[current_section].size()/2-2, REL_16, it.symbol);
							}
						}
						else
						{
							sections[current_section]+= temp.substr(temp.size() - 2);
							sections[current_section]+= temp.substr(0, 2);
							if (temp_ptr->section != "ABS")
							{
								reltab.insert(current_section, sections[current_section].size() / 2 -2, REL_16, it.symbol);
							}
						}
					
				}
			LC = LC + 2;
		}
	}
	else
		Error::writing_forbiden(current_section);
}
void Assembler::sp_byte_handler()
{
	if (current_rwx & W_MASK)
	{
		for (auto& it : line_iterator->operands)
		{
			SymbolTableNode* temp_ptr = symtab.find_symbol(it.symbol);
			if (it.type != IMMED && (temp_ptr && temp_ptr->section != "ABS"))
			{
				Error::invalid_operand_type(line_iterator->src_line);
			}
			else
				if (it.is_byte || (temp_ptr && temp_ptr->section == "ABS"))
				{
					if (current_section == ".data")
					{
						if (!temp_ptr || temp_ptr->section != "ABS")
						{
							sections[current_section] += byte_to_hex(it.literal);
						}
						else
						{
							if (temp_ptr->value >= INT8_MIN && temp_ptr->value <= INT8_MAX)
							{
								sections[current_section] += byte_to_hex(temp_ptr->value);
							}
							else
								Error::wrong_literal_width(line_iterator->src_line);
						}
					}
					else
					{
						if (temp_ptr->section != "ABS")
						{
							sections[current_section] += byte_to_hex(it.literal);
						}
						else
						{
							if (temp_ptr->value >= INT8_MIN && temp_ptr->value <= INT8_MAX)
							{
								sections[current_section] += byte_to_hex(temp_ptr->value);
							}
							else
								Error::wrong_literal_width(line_iterator->src_line);
						}
					}
				}
				else
				{
					Error::wrong_literal_width(line_iterator->src_line);
				}
			LC = LC + 1;
		}
	}
	else
		Error::writing_forbiden(current_section);
}
bool Assembler::check_operands()
{
	
	if (line_iterator->operands.size() == 2)
	{
		if (line_iterator->operands.front().type == IMMED || line_iterator->operands.front().type == IMMED_SYM_VALUE) return false;
		if (line_iterator->operands.front().is_byte && line_iterator->operands.back().is_word ) return false;
		return true;
	}
	else
	{
		if (line_iterator->instruction != "push" && line_iterator->operands.front().type == IMMED || line_iterator->operands.front().type == IMMED_SYM_VALUE)
		{
			return false;
		}
	}
}
bool Assembler::pcrel_present()
{
	for (auto& it : line_iterator->operands)
	{
		if (it.type == PCREL) return true;
	}
	return false;
}
bool Assembler::immed_sym_present()
{
	for (auto& it : line_iterator->operands)
	{
		if (it.type == IMMED_SYM_VALUE) return true;
	}
	return false;
}
bool Assembler::is_jump_instruction()
{
	std::string s = line_iterator->instruction;
	if (s == "jmp" || s == "jgt" || s == "jne" || s == "jeq") return true;
	else
		return false;
}
void Assembler::one_operand_instruction_handler(bool first_operand,bool first_operand_byte,bool first_operand_word,bool front)
{
	Operand op;
	if (!is_jump_instruction() && line_iterator->instruction != "call")
	{
		if (pcrel_present()) Error::pcrel_only_jump_call(line_iterator->src_line);
	}
	else
	{
		if (!immed_sym_present() && !pcrel_present()) Error::jump_call_immed_pcrel(line_iterator->src_line);
	}
	if (front) op = line_iterator->operands.front();
	else
		op = line_iterator->operands.back();
	if (line_iterator->operands_byte)
	{
		int second_byte = 0;
		int third_byte = 0;
		int fourth_byte = 0;
		std::string offset_bytes;
		switch (op.type)
		{
		case IMMED: 
			if (op.is_word) Error::wrong_literal_width(line_iterator->src_line);
			second_byte = (second_byte | IMMED_MASK) << 5;
			third_byte = op.literal;
			sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(third_byte);
			LC = LC + 2;
			break;
		case IMMED_SYM_VALUE:
			Error::wrong_literal_width(line_iterator->src_line);
			break;
		case REGDIR:
			if (!op.reg.low && !op.reg.high || op.reg.number == 15) Error::wrong_literal_width(line_iterator->src_line);
			second_byte = (second_byte | REGDIR_MASK) << 5;
			second_byte = second_byte | (op.reg.number << 1);
			if (op.reg.high) second_byte |= 1;
			sections[current_section] += byte_to_hex(second_byte);
			LC = LC + 1;
			break;
		case REGINDDISP_IMMED:
			if (op.literal == 0)
			{
				second_byte = (second_byte | REGIND_MASK) << 5;
				second_byte = second_byte | (op.reg.number << 1);
				sections[current_section] += byte_to_hex(second_byte);
				LC = LC + 1;
			}
			else
				if (op.literal >= INT8_MIN && op.literal <= INT8_MAX)
				{
					second_byte = (second_byte | REGIND_DISP_8_MASK) << 5;
					second_byte = second_byte | (op.reg.number << 1);
					third_byte = op.literal;
					sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(third_byte);
					LC = LC + 2;
				}
				else
				{
					second_byte = (second_byte | REGIND_DISP_16_MASK) << 5;
					second_byte = second_byte | (op.reg.number << 1);
					third_byte = op.literal;
					std::string offset_bytes = word_to_hex(third_byte);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size()-2) + offset_bytes.substr(0,2);
					LC = LC + 3;
				}
			break;
		case REGINDDISP_SYM_VALUE:
				second_byte = (second_byte | REGIND_DISP_16_MASK) << 5;
				second_byte = second_byte | (op.reg.number << 1);
				third_byte = symtab.find_symbol(op.symbol)->value;
				offset_bytes = word_to_hex(third_byte);
				sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
				reltab.insert(current_section, LC - 2, REL_16, op.symbol);
				LC = LC + 3;
				break;
		case PCREL:
			second_byte = second_byte |(REGIND_DISP_16_MASK << 5);
			second_byte = second_byte | (PC << 1);
			if (symtab.find_symbol(op.symbol)->section == current_section)
			{
				int offset = symtab.find_symbol(op.symbol)->value - LC - 2;
				third_byte = offset;
				if (offset >= INT8_MIN && offset <= INT8_MAX)
				{
					second_byte = 0;
					second_byte = second_byte | (REGIND_DISP_8_MASK << 5);
					second_byte = second_byte | (PC << 1);
					sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(third_byte);
					LC = LC + 2;
				}
				else
				{
					offset_bytes = word_to_hex(third_byte);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
					LC = LC + 3;
				}
			}
			else
			{
				sections[current_section] += byte_to_hex(second_byte) + word_to_hex(0);
				LC = LC + 3;
				reltab.insert(current_section, LC - 2, REL_PC_16, op.symbol);
			}
			break;
		case ABS:
			second_byte = (second_byte | MEMDIR_MASK) << 5;
			offset_bytes = word_to_hex(symtab.find_symbol(op.symbol)->value);
			sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
			LC = LC + 3;
			reltab.insert(current_section, LC - 2, REL_16, op.symbol);
			break;
		case MEMDIR:
			second_byte = (second_byte | MEMDIR_MASK) << 5;
			offset_bytes = word_to_hex(op.literal);
			LC = LC + 3;
			sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
			break;
		}
	}
	else
		if (line_iterator->operands_word)
		{
			int second_byte = 0;
			int third_byte = 0;
			int fourth_byte = 0;
			std::string offset_bytes;
			std::string temp;
			switch (op.type)
			{
				case IMMED:
					second_byte = (second_byte | IMMED_MASK) << 5;
					third_byte = op.literal;
					temp = word_to_hex(op.literal);
					sections[current_section] += byte_to_hex(second_byte) + temp.substr(temp.size() - 2) + temp.substr(0, 2);
					LC = LC + 3;
					break;
				case IMMED_SYM_VALUE:
					second_byte = (second_byte | IMMED_MASK) << 5;
					third_byte = op.literal;
					temp = word_to_hex(symtab.find_symbol(op.symbol)->value);
					sections[current_section] += byte_to_hex(second_byte) + temp.substr(offset_bytes.size() - 2) + temp.substr(0, 2);
					LC = LC + 3;
					reltab.insert(current_section, LC - 2, REL_16, op.symbol);
					break;
				case REGDIR:
					if (op.reg.low || op.reg.high) Error::wrong_literal_width(line_iterator->src_line);
					second_byte = (second_byte | REGDIR_MASK) << 5;
					second_byte = second_byte | (op.reg.number << 1);
					sections[current_section] += byte_to_hex(second_byte);
					LC = LC + 1;
					break;
				case REGINDDISP_IMMED:
					if (op.literal == 0)
					{
						second_byte = (second_byte | REGIND_MASK) << 5;
						second_byte = second_byte | (op.reg.number << 1);
						sections[current_section] += byte_to_hex(second_byte);
						LC = LC + 1;
					}
					else
						if (op.literal >= INT8_MIN && op.literal <= INT8_MAX)
						{
							second_byte = (second_byte | REGIND_DISP_8_MASK) << 5;
							second_byte = second_byte | (op.reg.number << 1);
							third_byte = op.literal;
							sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(third_byte);
							LC = LC + 2;
						}
						else
						{
							second_byte = (second_byte | REGIND_DISP_16_MASK) << 5;
							second_byte = second_byte | (op.reg.number << 1);
							third_byte = op.literal;
							std::string offset_bytes = word_to_hex(third_byte);
							sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
							LC = LC + 3;
						}
					break;
				case REGINDDISP_SYM_VALUE:
					second_byte = (second_byte | REGIND_DISP_16_MASK) << 5;
					second_byte = second_byte | (op.reg.number << 1);
					third_byte = symtab.find_symbol(op.symbol)->value;
					offset_bytes = word_to_hex(third_byte);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
					LC = LC + 3;
					reltab.insert(current_section, LC - 2, REL_16, op.symbol);
					break;
				case PCREL:
					second_byte = second_byte | (REGIND_DISP_16_MASK << 5);
					second_byte = second_byte | (PC << 1);
					if (symtab.find_symbol(op.symbol)->section == current_section)
					{
						int offset = symtab.find_symbol(op.symbol)->value - LC - 2;
						third_byte = offset;
						if (offset >= INT8_MIN && offset <= INT8_MAX)
						{
							second_byte = 0;
							second_byte = second_byte | (REGIND_DISP_8_MASK << 5);
							second_byte = second_byte | (PC << 1);
							sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(third_byte);
							LC = LC + 2;
						}
						else
						{
							offset_bytes = word_to_hex(third_byte);
							sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
							LC = LC + 3;
						}
					}
					else
					{
						sections[current_section] += byte_to_hex(second_byte) + word_to_hex(0);
						LC = LC + 3;
						reltab.insert(current_section, LC - 2, REL_PC_16, op.symbol);
					}
					break;
				case ABS:
					second_byte = (second_byte | MEMDIR_MASK) << 5;
					offset_bytes = word_to_hex(symtab.find_symbol(op.symbol)->value);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
					LC = LC + 3;
					reltab.insert(current_section, LC - 2, REL_16, op.symbol);
					break;
				case MEMDIR:
					second_byte = (second_byte | MEMDIR_MASK) << 5;
					offset_bytes = word_to_hex(op.literal);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
					LC = LC + 3;
					break;
			}
		}
		else
		{
			if (check_operands())
			{
				int second_byte = 0;
				int third_byte = 0;
				int fourth_byte = 0;
				std::string offset_bytes;
				std::string temp;
				switch (op.type)
				{
				case IMMED:
					second_byte = (second_byte | IMMED_MASK) << 5;
					third_byte = op.literal;
					if (op.is_byte)
					{
						if (first_operand || first_operand_byte) sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(op.literal);
						else
							if (first_operand_word)
							{
								temp = word_to_hex(op.literal);
								sections[current_section] += byte_to_hex(second_byte) + temp.substr(temp.size() - 2) + temp.substr(0, 2);
								LC = LC + 3;
							}
						break;
					}
					temp = word_to_hex(op.literal);
					if (first_operand || first_operand_word) sections[current_section] += byte_to_hex(second_byte) + temp.substr(temp.size() - 2) + temp.substr(0, 2), LC = LC + 3;
					else
						if (first_operand_byte) Error::operands_error(line_iterator->src_line);
					break;
				case IMMED_SYM_VALUE:
					second_byte = (second_byte | IMMED_MASK) << 5;
					third_byte = op.literal;
					temp = word_to_hex(symtab.find_symbol(op.symbol)->value);
					sections[current_section] += byte_to_hex(second_byte) + temp.substr(offset_bytes.size() - 2) + temp.substr(0, 2);
					LC = LC + 3;
					reltab.insert(current_section, LC - 2, REL_16, op.symbol);
					break;
				case REGDIR:
					//if (op.reg.low || op.reg.high) Error::wrong_literal_width(line_iterator->src_line);
					second_byte = (second_byte | REGDIR_MASK) << 5;
					second_byte = second_byte | (op.reg.number << 1);
					if (first_operand_byte && !first_operand && !op.reg.low && !op.reg.high) Error::wrong_operands_size(line_iterator->src_line);
					sections[current_section] += byte_to_hex(second_byte);
					LC = LC + 1;
					break;
				case REGINDDISP_IMMED:
					if (op.literal == 0)
					{
						second_byte = (second_byte | REGIND_MASK) << 5;
						second_byte = second_byte | (op.reg.number << 1);
						sections[current_section] += byte_to_hex(second_byte);
						LC = LC + 1;
					}
					else
						if (op.literal >= INT8_MIN && op.literal <= INT8_MAX)
						{
							second_byte = (second_byte | REGIND_DISP_8_MASK) << 5;
							second_byte = second_byte | (op.reg.number << 1);
							third_byte = op.literal;
							sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(third_byte);
							LC = LC + 2;
						}
						else
						{
							second_byte = (second_byte | REGIND_DISP_16_MASK) << 5;
							second_byte = second_byte | (op.reg.number << 1);
							third_byte = op.literal;
							std::string offset_bytes = word_to_hex(third_byte);
							sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
							LC = LC + 3;
						}
					break;
				case REGINDDISP_SYM_VALUE:
					second_byte = (second_byte | REGIND_DISP_16_MASK) << 5;
					second_byte = second_byte | (op.reg.number << 1);
					third_byte = symtab.find_symbol(op.symbol)->value;
					offset_bytes = word_to_hex(third_byte);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
					LC = LC + 3;
					reltab.insert(current_section, LC - 2, REL_16, op.symbol);
					break;
				case PCREL:
					second_byte = (second_byte | REGIND_DISP_16_MASK) << 5;
					second_byte = second_byte | (PC << 1);
					if (symtab.find_symbol(op.symbol)->section == current_section)
					{
						int offset = symtab.find_symbol(op.symbol)->value - LC -2;
						third_byte = offset;
						if (offset >= INT8_MIN && offset <= INT8_MAX)
						{
							second_byte = 0;
							second_byte = second_byte | (PC << 1);
							second_byte = second_byte | (REGIND_DISP_8_MASK<< 5);
							sections[current_section] += byte_to_hex(second_byte) + byte_to_hex(third_byte);
							LC = LC + 2;
						}
						else
						{
							offset_bytes = word_to_hex(third_byte);
							sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
							LC = LC + 3;
						}
					}
					else
					{
						sections[current_section] += byte_to_hex(second_byte) + word_to_hex(0);
						LC = LC + 3;
						reltab.insert(current_section, LC - 2, REL_PC_16, op.symbol);
					}
					break;
				case ABS:
					second_byte = (second_byte | MEMDIR_MASK) << 5;
					offset_bytes = word_to_hex(symtab.find_symbol(op.symbol)->value);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
					LC = LC + 3;
					reltab.insert(current_section, LC - 2, REL_16, op.symbol);
					break;
				case MEMDIR:
					second_byte = (second_byte | MEMDIR_MASK) << 5;
					offset_bytes = word_to_hex(op.literal);
					sections[current_section] += byte_to_hex(second_byte) + offset_bytes.substr(offset_bytes.size() - 2) + offset_bytes.substr(0, 2);
					LC = LC + 3;
					break;
				}
			}
			else
				Error::operands_error(line_iterator->src_line);
		}
}
void Assembler::two_operands_instruction_handler()
{
	bool is_byte = line_iterator->operands.front().is_byte;
	one_operand_instruction_handler(true, false, false, true);
	one_operand_instruction_handler(false, is_byte, !is_byte, false);
}
void Assembler::sp_instruction_handler()
{
	if (current_rwx & X_MASK)
	{
		if (line_iterator->operands.size() == instructions_op_numbers[line_iterator->instruction])
		{
			int first_byte = instructions_map[line_iterator->instruction] << 3; //Set opcode
			if (line_iterator->operands_word) first_byte = first_byte | WORD_SIZED_OPERANDS_MASK;
			if (!line_iterator->operands_word && !line_iterator->operands_byte)
			{
				if (line_iterator->operands.size() > 0 && line_iterator->operands.front().is_word) first_byte = first_byte | WORD_SIZED_OPERANDS_MASK;
			}


			sections[current_section] += byte_to_hex(first_byte);
			LC = LC + 1;

			switch (line_iterator->operands.size())
			{
				case 1:one_operand_instruction_handler(true,false,false,true); break;
				case 2:two_operands_instruction_handler(); break;
			}
			sections[current_section] += '\n';
		}
		else
			Error::wrong_operands_numbers(line_iterator->instruction, instructions_op_numbers[line_iterator->instruction], line_iterator->src_line);
	}
	else
		Error::execution_forbidden(current_section);
}
void Assembler::second_pass()
{
	LC = 0;
	current_section = "";
	current_rwx = 0;
	line_iterator = line_list.begin();
	
	while (line_iterator != line_list.end())
	{
		if (line_iterator->is_directive)
		{
			(this->*(sp_map[line_iterator->directive]))();
		}
		else
			if (line_iterator->is_instruction)
			{
				sp_instruction_handler();
			}
		if (line_iterator->directive == ".end") break;
		line_iterator++;
	}
}
std::string Assembler::assemble_line_list()
{
	first_pass();

	/*std::cout << symtab;
	std::cout << "------\n";
	std::cout << shtab;*/

	second_pass();
	std::cout << reltab;
	std::string ret = "";
	for (auto& it : sections)
	{
		ret += it.second;
	}
	//return ret;
	return sections[".text"];
}

Assembler::Assembler(std::list<Line> line_list)
{
	LC = 0;
	current_section = "";
	this->line_list = line_list;
	sections[".text"] = "";
	sections[".data"] = "";
	sections[".bss"] = "";

	line_iterator = this->line_list.begin();
	fp_map[".text"] = &Assembler::fp_section_handler;
	fp_map[".bss"] = &Assembler::fp_section_handler;
	fp_map[".data"] = &Assembler::fp_section_handler;
	fp_map[".section"] = &Assembler::fp_custom_section_handler;
	fp_map[".extern"] = &Assembler::fp_extern_handler;
	fp_map[".global"] = &Assembler::fp_global_handler;
	fp_map[".equ"] = &Assembler::fp_equ_handler;
	fp_map[".byte"] = &Assembler::fp_byte_handler;
	fp_map[".word"] = &Assembler::fp_word_handler;
	fp_map[".skip"] = &Assembler::fp_skip_handler;
	fp_map[".align"] = &Assembler::fp_align_handler;
	fp_map[".end"] = &Assembler::fp_end_handler;

	sp_map[".text"] = &Assembler::sp_section_handler;
	sp_map[".bss"] = &Assembler::sp_section_handler;
	sp_map[".data"] = &Assembler::sp_section_handler;
	sp_map[".section"] = &Assembler::sp_custom_section_handler;
	sp_map[".skip"] = &Assembler::sp_skip_handler;
	sp_map[".align"] = &Assembler::sp_align_handler;
	sp_map[".word"] = &Assembler::sp_word_handler;
	sp_map[".byte"] = &Assembler::sp_byte_handler;
	sp_map[".extern"] = &Assembler::do_nothing;
	sp_map[".global"] = &Assembler::do_nothing;
	sp_map[".equ"] = &Assembler::do_nothing;
	sp_map[".end"] = &Assembler::do_nothing;
}

void RelocationTable::insert(std::string section, unsigned offset, RelocationType type, std::string symbol)
{
	RelocationTableNode* node = new RelocationTableNode(section, offset, type, symbol);
	reltab.push_back(node);
}

RelocationTableNode* RelocationTable::find(std::string section, unsigned offset)
{
	for (auto& ptr : reltab) 
	{
		if (ptr->section == section && ptr->offset == offset) return ptr;
	}
	return 0;
}
