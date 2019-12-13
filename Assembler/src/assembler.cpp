#include "assembler.h"
#include "error.h"
void SymbolTable::insert_symbol(std::string name,int value,int visibility,std::string section,int import_export)
{
	SymbolTableNode* node = new SymbolTableNode(name,value,visibility,section,int import_export);
	symtab.push_back(node);
}

SymbolTableNode* SymbolTable::find_symbol(std::string name)
{
	for (int i=0;i<symtab.size();i++)
	{
		if (symtab[i]->name == name) return symtab[i];
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
	if (line_iterator->label.size() > 0)
	{
		if (current_section == "")
		{
			Error::symbol_not_in_section(line_iterator->label,line_iterator->src_line);
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
				temp_node->section = current_section;
				temp_node->value = LC;
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
		Error::symbol_not_in_section(line_iterator->src_line);
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
				symtab.insert(it.symbol,0,GLOBAL,"UNDEF",EXPORTED);
		}
		else
			Error::invalid_operand_type(line_iterator->src_line);
	}
}

void Assembler::fp_extern_handler()
{
	if (current_section == "")
	{
		Error::symbol_not_in_section(line_iterator->src_line);
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
				symtab.insert(it.symbol,0,GLOBAL,"UNDEF",IMPORTED);
		}
		else
			Error::invalid_operand_type(line_iterator->src_line);
	}
}

void Assembler::fp_skip_handler()
{
	if (current_section == "")
	{
		Error::symbol_not_in_section(line_iterator->src_line);
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
		Error::symbol_not_in_section(line_iterator->src_line);
	}
	if (line_iterator->operands.size() < 1)
	{
		Error::skip_requires_one_operand(line_iterator->src_line);
	}
	LC = LC + line_iterator->operands.size();
}

void Assembler::fp_word_handler()
{
	if (current_section == "")
	{
		Error::symbol_not_in_section(line_iterator->src_line);
	}
	if (line_iterator->operands.size() < 1)
	{
		Error::skip_requires_one_operand(line_iterator->src_line);
	}
	LC = LC + 2*line_iterator->operands.size();
}

void Assembler::fp_align_handler()
{
	if (current_section == "")
	{
		Error::symbol_not_in_section(line_iterator->src_line);
	}
	if (line_iterator->operands.size() != 1)
	{
		Error::skip_requires_one_operand(line_iterator->src_line);
	}
	if (line_iterator->operands.front().type != IMMED || line_iterator->operands.front().literal < 0)
	{
		Error::skip_requires_one_operand(line_iterator->src_line);
	}
	unsigned int mask = ~((~0) << line_iterator->operands.front().literal);
	
	while (mask & LC)
	{
		LC++;
	}
}

void Assembler::first_pass()
{
	while (line_iterator != line_list.end())
	{
		fp_label_handler();
		
		line_iterator++;
	}
}
std::string Assembler::assemble_line_list()
{
	first_pass();
}