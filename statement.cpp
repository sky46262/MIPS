#include "statement.h"
Statement::Statement(STATEMENT_TYPE _type) :type(_type) {}

Calc::Calc(CALC_TYPE ct, bool isu, Register* de, Register* src1, Register* src2, int x):Statement(CALC), value1(src1), dest(de), c_type(ct), isUnsigned(isu){
	if (ct == NEG) return;
	if (src2 == NULL) value2.y = x, isWithConst = 1;
	else value2.x = src2, isWithConst = 0;
}
Cmp::Cmp(CMP_TYPE ct, Register* de, Register *src1, Register *src2, int x) : Statement(CMP), value1(src1), dest(de), c_type(ct) {
	if(src2 == NULL) value2.y = x, isWithConst = 1;
	else value2.x = src2, isWithConst = 0;
}
Branch::Branch(BRANCH_TYPE ct, uint la, Register *src1, Register *src2, int x) : Statement(BRANCH), value1(src1), label(la), b_type(ct) {
	if (src1 == NULL) return;
	if (src2 == NULL) value2.y = x, isWithConst = 1;
	else value2.x = src2, isWithConst = 0;
}
Jump::Jump(JUMP_TYPE ct, int pos, Register *src, uint la) : Statement(JUMP), j_type(ct), nextPosition(pos){
	if (src == NULL) dest.label = la;
	else dest.address = src;
}

Load::Load(LS_TYPE ct, Register* de, Address add, uint x):Statement(LOAD), l_type(ct), dest(de){
	if (ct == CONST) i = x;
	else address = add;
}
Store::Store(LS_TYPE ct, Register* sr, Address add) : Statement(STORE), s_type(ct), src(sr), address(add){}

Move::Move(Register* de, Register* sr): Statement(MOVE), dest(de), src(sr){}