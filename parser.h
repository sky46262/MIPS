#pragma once
#ifndef PARSER_H
#define PARSER_H
#include<string>
#include<fstream>
#include<map>
#include<cstring>
#include<cstdlib> 
#include<vector>
#define Register int
using namespace std;
const int BUFFER_SIZE = 256;
const int KEYWORD_SIZE = 60;
const string R_name[35] = { "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1"
, "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9","k0", "k1", "gp", "sp", "fp", "ra", "hi", "lo", "pc" };
const string keywords[60] = {".align", ".ascii", ".asciiz", ".byte", ".half", ".word", ".space", ".data", ".text"
, "add", "addu", "addiu", "sub", "subu", "mul", "mulu", "div", "divu", "xor", "xoru", "neg", "negu", "rem","remu",
"seq", "sge", "sgt", "sle", "slt", "sne",
"b", "beq", "bne", "bge", "ble", "bgt", "blt", "beqz", "bnez", "blez", "bgez", "bgtz", "bltz",
"j", "jr", "jal", "jalr", 
"li", "la", "lb", "lh", "lw", "sb", "sh", "sw"
, "move", "mfhi", "mflo", "nop", "syscall"
};
//seperator: , \n
enum Token_type {
	INT, STRING, KEYWORD, SEPERATOR, REGISTER, LABEL, BRACKET
};

class Token {
private:
	Token_type type;
	string value;
public:
	Token(string, Token_type);
	~Token();
	Token_type getTokenType();

	int toInt();

	string toString();
};

class Parser {
private:
	char *buffer;
	char savedChar;
	FILE* file;
	size_t size, pos_buffer;

	char getNextChar();
	bool hasMoreChars();

	bool isInt(char);
	bool isLetter(char);
public:
	Parser(string name);
	~Parser();

	Token getNextToken();
	bool hasMoreTokens();
};

#endif // !PARSER_H
