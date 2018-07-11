#pragma once
#ifndef STATEMENT_H
#include "parser.h"
#include<map>
#include<algorithm>
#include<iostream>
#define uint unsigned int
#define Register int

const int MEMORY_SIZE = 507904;
extern Register R[35];
extern vector<char*> L;
extern char *memory_start, *text_end, *data_end, *heap;
extern bool isEnded;
enum STATEMENT_TYPE{
	CALC, CMP, MOVE, LOAD, STORE, BRANCH, JUMP, NOP, SYS
};
class Statement {
public:
	STATEMENT_TYPE type;
	Statement(STATEMENT_TYPE _type);
	virtual ~Statement(){}
};

// Calculation
enum CALC_TYPE{ADD, SUB, MUL, DIV, XOR, NEG, REM};
class Calc : public Statement {
public:
	CALC_TYPE c_type;
	Register *value1, *dest;
	union Value {
		Register *x;
		int y;
		Value():x(NULL){}
	}value2;
	bool isUnsigned, isWithConst;
	Calc(CALC_TYPE, bool, Register*, Register*, Register* = NULL, int = 0);
	~Calc() {}
};

// Comparison
enum CMP_TYPE{CGT, CGE, CLT, CLE, CEQ, CNEQ};
class Cmp :public Statement{
public:
	CMP_TYPE c_type;
	Register *value1, *dest;
	union Value {
		Register *x;
		int y;
		Value() :x(NULL) {}
	}value2;
	bool isWithConst;
	Cmp(CMP_TYPE, Register*, Register*, Register*, int = 0);
	~Cmp() {}
};

// Branch
enum BRANCH_TYPE{B, BEQ, BNE, BGE, BGT, BLE, BLT };
class Branch :public Statement {
public:
	BRANCH_TYPE b_type;
	uint label;
	Register *value1;
	union Value {
		Register *x;
		int y;
		Value() :x(NULL) {}
	}value2;
	bool isWithConst;
	int History;
	Branch(BRANCH_TYPE, uint, Register* = NULL, Register* = NULL, int = 0);
	~Branch() {}
};
// Jump
// al:$31 ; r: register
enum JUMP_TYPE{J, JR, JAL, JALR};
class Jump :public Statement {
public:
	JUMP_TYPE j_type;
	struct Dest {
		uint label;
		Register *address;
		Dest():address(NULL){}
		~Dest(){}
	}dest;
	int nextPosition;
	Jump(JUMP_TYPE, int, Register*, uint = -1);
	~Jump() {}
};

struct Address {
	int x;
	class toRegis {
	public:
		Register * pos;
		int del;
	}y;
	Address() = default;
	Address(int a, Register* b = NULL, int c = 0) {
		y.pos = b;
		y.del = c;
		x = a;
	}
};
//Load
//I: const
enum LS_TYPE{ADDRESS, BYTE, HALF, WORD, CONST};
class Load :public Statement {
public:
	LS_TYPE l_type;
	Address address;
	Register *dest;
	uint i;
	Load(LS_TYPE, Register*, Address, uint = 0);
	~Load() {}
};

//Load
class Store :public Statement {
public:
	LS_TYPE s_type;
	Address address;
	Register *src;
	Store(LS_TYPE, Register*, Address);
	~Store() {}
};

// Movement
class Move :public Statement {
public:
	Register *src, *dest;
	Move(Register*, Register*);
	~Move() {}
};
#endif // !STATEMENT_H

