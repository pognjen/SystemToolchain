#ifndef _ASM_H
#define _ASM_H

#include "parser.h"
#include <vector>

#define LOCAL 0
#define GLOBAL 1

#define IMPORTED 1
#define EXPORTED 2
#define NONE 3

#define R_MASK 1
#define W_MASK 2
#define X_MASK 4

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
	friend std::ostream& operator<<(std::ostream& os, const SymbolTableNode& node) // for testing
	{
		os << node.name << " | "<<node.value << " | ";
		if (node.visibility == LOCAL) os << "LOCAL | "<<node.section<<" | ";
		else
			os << "GLOBAL | " << node.section << " | ";
		if (node.import_export == IMPORTED) os << "IMPORT";
		else
			if (node.import_export == EXPORTED) os << "EXPORT";
			else
				os << "NONE";
		return os;
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
	friend std::ostream& operator<<(std::ostream& os, const SymbolTable& table) // for testing
	{
		for (int i = 0; i < table.symtab.size(); i++)
			os << *(table.symtab[i]) << '\n';
		return os;
	}
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
	friend std::ostream& operator<<(std::ostream& os, const SectionHeaderTableNode& n) // for testing
	{
		return os << n.name << " | " << n.size << " | "<<(int)n.rwx;
	}
};

class SectionHeaderTable
{
friend class Assembler;
private:
	std::vector<SectionHeaderTableNode*> shtable;
	void insert_sh_node(std::string name,unsigned size,char rwx);
	SectionHeaderTableNode* find_sh_node(std::string name);
	friend std::ostream& operator<<(std::ostream& os, const SectionHeaderTable& n) // for testing
	{
		for (int i = 0; i < n.shtable.size(); i++)
		{
			os << *(n.shtable[i]) << '\n';
		}
		return os;
	}
};

class Assembler
{
private:
	typedef void(Assembler::*method_pointer)();	
	std::unordered_map<std::string,method_pointer> map;
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
	void fp_section_handler();
	void fp_custom_section_handler();
	void fp_equ_handler();
	unsigned int LC;
	std::string current_section;
	char current_rwx;
	SymbolTable symtab;
	SectionHeaderTable shtab;
public:
	Assembler(std::list<Line> line_list);
	std::string assemble_line_list();
};

#endif

