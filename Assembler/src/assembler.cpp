#include "assembler.h"
#include "error.h"
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
	if (current_section == "")
	{
		Error::global_not_in_section(line_iterator->src_line);
	}

	if (line_iterator->operands.size() == 0)
	{
		Error::global_requires_operands(line_iterator->src_line);
	}
	
	for (auto& it:line_iterator->operands)
	{
		if (it.type == ABS)
		{
			SymbolTableNode* temp_node = symtab.find_symbol(line_iterator->label);
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
	if (current_section == "")
	{
		Error::extern_not_in_section(line_iterator->src_line);
	}
	if (line_iterator->operands.size() == 0)
	{
		Error::global_requires_operands(line_iterator->src_line);
	}
	for (auto& it:line_iterator->operands)
	{
		if (it.type == ABS)
		{
			SymbolTableNode* temp_node = symtab.find_symbol(line_iterator->label);
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
	std::string rwx = line_iterator->operands.back().symbol;
	LC = 0;
	current_rwx = 0;
	for (int i=0;i<rwx.size();i++)
	{
		if (rwx[i] == 'R' || rwx[i] == 'r') current_rwx = current_rwx & R_MASK;
		if (rwx[i] == 'W' || rwx[i] == 'w') current_rwx = current_rwx & W_MASK;
		if (rwx[i] == 'X' || rwx[i] == 'x') current_rwx = current_rwx & X_MASK;
	}
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
	LC = LC + 1; // InstrDesc
	
	if (line_iterator->operands.size() > 2 || line_iterator->operands.size() < 0)
	{
		Error::wrong_number_of_operands(line_iterator->instruction, line_iterator->src_line);
	}

	if (line_iterator->operands.size() == 2)
	{
		LC = LC + 6;
	}
	else
		if (line_iterator->operands.size() == 1)
		{
			LC = LC + 3;
		}
}
void Assembler::first_pass()
{
	while (line_iterator != line_list.end())
	{
		fp_label_handler();
		
		if (line_iterator->is_directive)
		{
			(this->*(map[line_iterator->directive]))();
		}
		else
			if (line_iterator->is_instruction)
			{
				fp_instruction_handler();
			}
		line_iterator++;
	}
}
std::string Assembler::assemble_line_list()
{
	first_pass();
	
	std::cout << symtab;
	//std::cout << shtab;
	return "asdasd";
}

Assembler::Assembler(std::list<Line> line_list)
{
	LC = 0;
	current_section = "";
	this->line_list = line_list;
	line_iterator = this->line_list.begin();
	map[".text"] = &Assembler::fp_section_handler;
	map[".bss"] = &Assembler::fp_section_handler;
	map[".data"] = &Assembler::fp_section_handler;
	map[".section"] = &Assembler::fp_custom_section_handler;
	map[".extern"] = &Assembler::fp_extern_handler;
	map[".global"] = &Assembler::fp_global_handler;
	map[".equ"] = &Assembler::fp_equ_handler;
	map[".byte"] = &Assembler::fp_byte_handler;
	map[".word"] = &Assembler::fp_word_handler;
	map[".skip"] = &Assembler::fp_skip_handler;
	map[".align"] = &Assembler::fp_align_handler;
}