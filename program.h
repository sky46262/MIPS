#pragma once
#ifndef PROGRAM_H

#include<string>
#include"statement.h"
#define uint unsigned int
#define Register int

void initialize();
void Program(string filename);
void Data(string filename);

class Pipeline {
private:
	//IF->ID
	Statement* instruction;
	//ID->EX
	int x[3], count;
	Statement* id_ex;
	//EX->MEM
	int type, m_type;
	Register *r1, *r2;
	int v1, v2;
	int add;
	//MEM->WB
	Register *_r1, *_r2;
	int _v1, _v2;
	//outside
	int return_value;
	bool lockID;
	void IF();
	void ID();
	void EX();
	void MEM();
	void WB();
	bool IDlocked();
public:
	int exec();
};
#endif // !PROGRAM_H