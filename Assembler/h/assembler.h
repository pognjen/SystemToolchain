#ifndef _ASM_H
#define _ASM_H

#include "parser.h"
#include <vector>

#define LOCAL 0
#define GLOBAL 1

#define IMPORTED 1
#define EXPORTED 2
#define NONE 3

class SymbolTableNode
{
friend class SymbolTable;
friend class Assembler;
private:
	std::string name;
	int value; //LC value
	int visibility; // Local or global
	std::string section; //Section header table index,ABS(.equ) or UNDEF(.extern)
	int import_export; // IMPORTED(.global) , EXPORTED(.extern), NONE
public:
	SymbolTableNode(std::string name,int value,int visibility,std::string section,int import_export)
	{
		this->name = name;
		this->value = value;
		this->visibility = visibility;
		this->section = section;
		this->import_export = import_export;
	}
};

class SymbolTable
{
friend class Assembler;
private:
	std::vector<SymbolTableNode*> symtab;
public:
	void insert_symbol(std::string name,int value,int visibility,std::string section,int import_export);
	SymbolTableNode* find_symbol(std::string name);
};

class SectionHeaderTableNode
{
friend class Assembler;
friend class SectionHeaderTable;
private:
	std::string name;
	unsigned size;
	char rwx;
public:
	SectionHeaderTableNode(std::string name,unsigned size,char rwx)
	{
		this->name = name;
		this->size = size;
		this->rwx = rwx;
	}
};

class SectionHeaderTable
{
friend class Assembler;
private:
	std::vector<SectionHeaderTableNode*> shtable;
	void insert_sh_node(std::string name,unsigned size,char rwx);
	SectionHeaderTableNode* find_sh_node(std::string name);
};

class Assembler
{
private:
	std::list<Line> line_list;
	std::list<Line>::iterator line_iterator;
	void first_pass();
	void fp_label_handler();
	void fp_global_handler();
	void fp_extern_handler();
	void fp_skip_handler();
	void fp_byte_handler();
	void fp_word_handler();
	void fp_align_handler();
	unsigned LC;
	std::string current_section;
	SymbolTable symtab;
	SectionHeaderTable shtab;
public:
	Assembler(std::list<Line> line_list)
	{
		LC = 0;
		current_section = "";
		this->line_list = line_list;
		line_iterator = line_list.begin();
	}
	std::string assemble_line_list();
};
#endif

