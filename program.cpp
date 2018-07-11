#include "program.h"
#include<iostream>

Register R[35];
vector<char*> L;
int Lock[35];
const int N = 20;
int PatternHistory[1 << N];
//sp 29 fp 30 ra 31 hi,lo 32,33 pc 34
extern map<string, int> L_index;
char *memory_start, *text_end, *data_end, *heap;
bool isEnded = 0;
void initialize() {
	for (int i = 0; i < 35; i++) R[i] = 0;
	memory_start = (char *)malloc(MEMORY_SIZE);
	memset(memory_start, 0, sizeof(char) * MEMORY_SIZE);
	R[29] = MEMORY_SIZE; // sp;
}
void Delete() {
	free(memory_start);
}
void Program(string filename) {
	initialize();
	char *now = memory_start;
 	Parser parser(filename);
	string word;
	bool isText = false;
	while (parser.hasMoreTokens()) {
		Token token = parser.getNextToken();
		word = token.toString();
		if (token.getTokenType() == KEYWORD) {
			int idx;
			for (int i = 0; i < KEYWORD_SIZE; i++)
				if (keywords[i] == word) idx = i;
			//Statement
			Statement *st = NULL;
			if (idx < 9) {
				if (word == ".data") {
					isText = false;
					parser.getNextToken();// \n
				}
				if (word == ".text") {
					isText = true;
					parser.getNextToken();// \n
				}
				continue;
			}
			if (idx >= 9 && idx < 24){
				string sub = word.substr(0, 3);
				//dest
				Token token1 = parser.getNextToken();
				parser.getNextToken();
				//src1
				Token token2 = parser.getNextToken();
				//type
				CALC_TYPE ct;
				//isUnsigned
				bool isu;
				if (word.back() == 'u') isu = true;
				else isu = false;
				if (sub == "add" || sub == "sub" || sub == "xor" || sub == "rem") {
					parser.getNextToken();
					Token token3 = parser.getNextToken();
					parser.getNextToken();
					if (sub == "add") ct = ADD;
					if (sub == "sub") ct = SUB;
					if (sub == "xor") ct = XOR;
					if (sub == "rem") ct = REM;
					if (token3.getTokenType() == INT) 
						st = new Calc(ct, isu, &R[token1.toInt()], &R[token2.toInt()], NULL, token3.toInt());
					else st = new Calc(ct, isu, &R[token1.toInt()], &R[token2.toInt()], &R[token3.toInt()]);
				}
				if (sub == "mul" || sub == "div") {
					if (sub == "mul") ct = MUL;
					else ct = DIV;
					if (parser.getNextToken().toString() == ",") {
						Token token3 = parser.getNextToken();
						parser.getNextToken();
						if (token3.getTokenType() == INT)
							st = new Calc(ct, isu, &R[token1.toInt()], &R[token2.toInt()], NULL, token3.toInt());
						else st = new Calc(ct, isu, &R[token1.toInt()], &R[token2.toInt()], &R[token3.toInt()]);
					}
					else {
						if (token2.getTokenType() == INT)
							st = new Calc(ct, isu, NULL, &R[token1.toInt()], NULL, token2.toInt());
						else st = new Calc(ct, isu, NULL, &R[token1.toInt()], &R[token2.toInt()]);
					}
				}
				if (sub == "neg") {
					parser.getNextToken();// \n
					st = new Calc(NEG, isu, &R[token1.toInt()], &R[token2.toInt()]);
				}
			}
			//cmp
			if (idx >= 24 && idx < 30) {
				//dest
				Token token1 = parser.getNextToken();
				parser.getNextToken();// ,
				//src1 src2
				Token token2 = parser.getNextToken();
				parser.getNextToken();
				Token token3 = parser.getNextToken();
				parser.getNextToken();// \n
				//type
				CMP_TYPE ct;
				if (word[1] == 'g') {
					if (word[2] == 't') 
						ct = CGT;
					else ct = CGE;
				}
				if (word[1] == 'l') {
					if (word[2] == 't')
						ct = CLT;
					else ct = CLE;
					
				}
				if (word[1] == 'e') ct = CEQ;
				if (word[1] == 'n') ct = CNEQ;
				if(token3.getTokenType() == REGISTER)
				st = new Cmp(ct, &R[token1.toInt()], &R[token2.toInt()], &R[token3.toInt()]);
				else st = new Cmp(ct, &R[token1.toInt()], &R[token2.toInt()], NULL, token3.toInt());
			}
			//branch
			if (idx >= 30 && idx < 43) {
				if (word[1] == '\0') {
					st = new Branch(B, parser.getNextToken().toInt());
					parser.getNextToken();
				}
				else {
					//type
					BRANCH_TYPE ct;
					if (word[1] == 'g') {
						if (word[2] == 't')
							ct = BGT;
						else ct = BGE;
					}
					if (word[1] == 'l') {
						if (word[2] == 't')
							ct = BLT;
						else ct = BLE;

					}
					if (word[1] == 'e') ct = BEQ;
					if (word[1] == 'n') ct = BNE;
					Token token1 = parser.getNextToken();
					parser.getNextToken();// ,
					//src1
					Token token2 = parser.getNextToken();
					parser.getNextToken();
					if (word.back() == 'z') 
						st = new Branch(ct, token2.toInt(), &R[token1.toInt()], NULL, 0);
					else {
						Token token3 = parser.getNextToken();
						parser.getNextToken();// \n
						if (token2.getTokenType() == INT) st = new Branch(ct, token3.toInt(), &R[token1.toInt()], NULL, token2.toInt());
						else st = new Branch(ct, token3.toInt(), &R[token1.toInt()], &R[token2.toInt()]);
					}
				}
				
			}
			//jump
			if (idx >= 43 && idx < 47) {
				Token token1 = parser.getNextToken();
				parser.getNextToken();
				if (word == "j" ) st = new Jump(J, 0, NULL, token1.toInt());
				if (word == "jr") st = new Jump(JR, 0, &R[token1.toInt()]);
				if (word == "jal") st = new Jump(JAL, now + sizeof(Statement*) - memory_start, NULL, token1.toInt());
				if (word == "jalr") st = new Jump(JALR, now + sizeof(Statement*) - memory_start, &R[token1.toInt()]);
			}
			
			if (idx >= 47 && idx < 55) {
				LS_TYPE ct;
				switch (word[1]) {
				case 'i':
					ct = CONST;
					break;
				case 'a':
					ct = ADDRESS;
					break;
				case 'b':
					ct = BYTE;
					break;
				case 'h':
					ct = HALF;
					break;
				case 'w':
					ct = WORD;
					break;
				}
				Token token1 = parser.getNextToken();
				parser.getNextToken();
				Token token2 = parser.getNextToken();
				if (word[0] == 'l') {
					if (ct == CONST) {
						st = new Load(ct, &R[token1.toInt()], Address(), token2.toInt());
					}
					else {
						if (token2.getTokenType() == LABEL) st = new Load(ct, &R[token1.toInt()], Address(token2.toInt()));
						else {
							if (token2.getTokenType() == BRACKET)
								st = new Load(ct, &R[token1.toInt()], Address(-1, &R[token2.toInt()], 0));
							else {
								Token token3 = parser.getNextToken();
								st = new Load(ct, &R[token1.toInt()], Address(-1, &R[token3.toInt()], token2.toInt()));
							}
						}
					}
				}
				else {
					if (token2.getTokenType() == LABEL)
						st = new Store(ct, &R[token1.toInt()], Address(token2.toInt()));
					else {
						if (token2.getTokenType() == BRACKET)
							st = new Store(ct, &R[token1.toInt()], Address(-1, &R[token2.toInt()], 0));
						else {
							Token token3 = parser.getNextToken();
							st = new Store(ct, &R[token1.toInt()], Address(-1, &R[token3.toInt()], token2.toInt()));
						}
					}
				}
				parser.getNextToken();
			}
			
			if (idx >= 55 && idx < 58) {
				Token token1 = parser.getNextToken();
				parser.getNextToken();
				if (word == "mfhi") st = new Move(&R[token1.toInt()], &R[32]);
				if (word == "mflo") st = new Move(&R[token1.toInt()], &R[33]);
				if (word == "move") {
					st = new Move(&R[token1.toInt()], &R[parser.getNextToken().toInt()]);
					parser.getNextToken();
				}
			}
			if (idx == 58) st = new Statement(NOP), parser.getNextToken();
			if (idx == 59) st = new Statement(SYS), parser.getNextToken();
			//save
			if (idx >= 9) {
 				*(Statement**)now = st;
				now += sizeof(Statement*);
			}
		}
		else 
			if (token.getTokenType() == LABEL && isText) {
				parser.getNextToken();
				if (word.back() == ':') {
					word.pop_back();
					L[token.toInt()] = now;
				}
			}
	}
	text_end = now;
}

void Data(string filename) {
	Parser parser(filename);
	string word;
	char *now = text_end;
	bool isText = false;
	while (parser.hasMoreTokens()) {
		Token token = parser.getNextToken();
		word = token.toString();
		//char* a = (char*)L["_static_111"];
		if (token.getTokenType() == KEYWORD) {
			int idx;
			for (int i = 0; i < KEYWORD_SIZE; i++)
 				if (keywords[i] == word) idx = i;
			if (idx < 9) {
				if (word == ".data") {
					isText = false;
					parser.getNextToken();// \n
				}
				if (word == ".text") {
					isText = true;
					parser.getNextToken();// \n
				}
				if (word[1] == 'a') {
					if (word == ".align") {
						int num = parser.getNextToken().toInt();
						if ((now - text_end) % (1 << num) != 0) {
							int i = (now - text_end) / (1 << num);
							now = text_end + (i + 1) * (1 << num);
						}
					}
					else {// ascii asciiz
						string str = parser.getNextToken().toString();
						for (uint i = 0; i < str.length(); i++, now++)
							*now = str[i];
						if (word.back() == 'z') {
							*now = 0;
							now++;
						}
					}
					parser.getNextToken();// \n
				}
				if (word == ".space") {
					now += parser.getNextToken().toInt();
					parser.getNextToken();// \n
				}
				if (word == ".byte") {
					char num = parser.getNextToken().toInt();
					while (parser.getNextToken().toString() != "\n") {
						*now = num;
						now++;
						num = parser.getNextToken().toInt();
					}
				}
				if (word == ".half") {
					short int *tmp = (short int*)now;
					short int num = parser.getNextToken().toInt();
					while (parser.getNextToken().toString() != "\n") {
						*tmp = num;
						tmp++;
						now += 2;
						num = parser.getNextToken().toInt();
					}
				}
				if (word == ".word") {
					int *tmp = (int*)now;
					int num = parser.getNextToken().toInt();
					while (parser.getNextToken().toString() != "\n") {
						*tmp = num;
						tmp++;
						now += 4;
						num = parser.getNextToken().toInt();
					}
					*tmp = num;
					tmp++;
					now += 4;
				}
			}
		}
		else 
			if (token.getTokenType() == LABEL && !isText) {
				parser.getNextToken();
				if (word.back() == ':') {
					word.pop_back();
					L[token.toInt()] = now;
				}
			}
	}
	data_end = now;
}

int Pipeline::exec() {
	R[34] = L[L_index["main"]] - memory_start;//address of (Statement*)
	heap = data_end;
	lockID = false;
	instruction = id_ex = NULL;
	r1 = r2 = _r1 = _r2 = NULL;
	for (int i = 0; i < 1 << N; i++) PatternHistory[i] = 2;
	while (true) {
		WB();
		MEM();
		EX();
		ID();
		IF();
		if (isEnded)
			return return_value;
	}
}
bool Pipeline::IDlocked () {
	switch (instruction->type)
	{
	case SYS: 
		if (Lock[2] || Lock[4] || Lock[5]) return 1;
		break;
	case CALC: {
		Calc* c = (Calc*)instruction;
		if (Lock[c->value1 - R]) return 1;
		if (c->c_type != NEG && !c->isWithConst && Lock[c->value2.x - R]) return 1;
		break;
	}
	case CMP: {
		Cmp* c = (Cmp*)instruction;
		if (Lock[c->value1 - R]) return 1;
		if (!c->isWithConst && Lock[c->value2.x - R]) return 1;
		break;
	}
	case BRANCH: {
		Branch* c = (Branch*)instruction;
		if (c->b_type != B) {
			if (Lock[c->value1 - R]) return 1;
			if (!c->isWithConst && Lock[c->value2.x - R]) return 1;
		}
		break;
	}
	case JUMP: {
		Jump* c = (Jump*)instruction;
		if (c->j_type == JR || c->j_type == JALR)
			if (Lock[c->dest.address - R]) return 1;
		break;
	}
	case LOAD: {
		Load* c = (Load*)instruction;
		if (c->address.x == -1 && Lock[c->address.y.pos - R]) return 1;
		break;
	}
	case STORE: {
		Store* c = (Store*)instruction;
		if (Lock[c->src - R]) return 1;
		if (c->address.x == -1 && Lock[c->address.y.pos - R]) return 1;
		break;
	}
	case MOVE: {
		Move *c = (Move*)instruction;
		if (Lock[c->src - R]) return 1;
		break;
	}
	default:
		break;
	}
	return 0;
}
bool Pipeline::Predict(Branch* c) {
	int pattern = PatternHistory[c->History & ((1 << N) - 1)];
	return PatternHistory[pattern] >= 2;
}
void Pipeline::SaveHistory(Branch* c, bool z) {
	int pattern = c->History & ((1 << N) - 1);
	if (z) {
		if (PatternHistory[pattern] < 3) PatternHistory[pattern]++;
	}
	else if (PatternHistory[pattern] > 0) PatternHistory[pattern]--;
	c->History = (c->History << 1) + z;
}
void Pipeline::IF() {
	if (Lock[34] || lockID) return;
	if (R[34] + memory_start >= text_end) return;
	instruction = *(Statement**)(R[34] + memory_start);//Instruction Fetch
	R[34] += sizeof(Statement*);
}
void Pipeline::ID() {
	if (instruction == NULL) return;
	if (lockID = IDlocked()) return;
	id_ex = instruction;
	switch (instruction->type)
	{
	case SYS: {
		//v0 , a0 , a1
		x[0] = R[2], x[1] = R[4], x[2] = R[5];
		count = 3;
		if (R[2] == 5 || R[2] == 9) Lock[2]++;
		break;
	}
	case CALC: {
		Calc* c = (Calc*)instruction;
		x[0] = *(c->value1);
		if (c->c_type == NEG) {
			count = 1;
		}
		else {
			count = 2;
			if (c->isWithConst) x[1] = c->value2.y;
			else x[1] = *(c->value2.x);
		}
		if (c->dest) Lock[c->dest - R]++;
		else Lock[32]++, Lock[33]++;
		break;
	}
	case CMP: {
		Cmp* c = (Cmp*)instruction;
		x[0] = *(c->value1);
		if (c->isWithConst) x[1] = c->value2.y;
		else x[1] = *(c->value2.x);
		count = 2;
		Lock[c->dest - R]++;
		break;
	}
	case BRANCH: {
		Branch* c = (Branch*)instruction;
		if (c->b_type != B) {
			x[0] = *(c->value1);
			if (c->isWithConst) x[1] = c->value2.y;
			else x[1] = *(c->value2.x);
		}
		if (isBranch = Predict(c)) {
			other = R[34];
			R[34] = L[c->label] - memory_start;
		}
		else other = L[c->label] - memory_start;
		//Lock[34]++;
		count = 2;
		break;
	}
	case JUMP: {
		Jump* c = (Jump*)instruction;
		count = 1;
		if (c->j_type == J || c->j_type == JAL) 
			x[0] = L[c->dest.label] - memory_start;
		else x[0] = *(c->dest.address);
		if (c->j_type == JALR || c->j_type == JAL) Lock[31]++;
		
		R[34] = x[0];//Lock[34]++;
		break;
	}
	case LOAD: {
		Load* c = (Load*)instruction;
		if (c->address.x == -1) {
			x[0] = *(c->address.y.pos);
			x[1] = c->address.y.del;
			count = 2;
		}
		else {
			count = 1;
			if (c->l_type == CONST) x[0] = c->i;
			else x[0] = L[c->address.x] - memory_start;
		}
		Lock[c->dest - R]++;
		break;
	}
	case STORE: {
		Store* c = (Store*)instruction;
		x[0] = *(c->src);
		if (c->address.x == -1) {
			x[1] = *(c->address.y.pos);
			x[2] = c->address.y.del;
			count = 3;
		}
		else {
			x[1] = L[c->address.x] - memory_start;
			count = 2;
		}
		break;
	}
	case MOVE: {
		Move *c = (Move*)instruction;
		x[0] = *(c->src);
		count = 1;
		Lock[c->dest - R]++;
		break;
	}
	default:
		count = 0;
		break;
	}
	instruction = NULL;
}
void Pipeline::EX() {
	if (id_ex == NULL) return;
	type = id_ex->type;
	switch (id_ex->type) {
	case SYS: {
		m_type = x[0];
		v1 = x[1];
		v2 = x[2];
		break;
	}
	case CALC: {
		Calc *c = (Calc*)id_ex;
		if (c->isUnsigned) {
			uint _x = x[0], _y = x[1];
			if (c->dest != NULL) {
				uint z = 0;
				switch (c->c_type) {
				case ADD:
					z = _x + _y;
					break;
				case SUB:
					z = _x - _y;
					break;
				case MUL:
					z = _x * _y;
					break;
				case DIV:
					z = _x / _y;
					break;
				case XOR:
					z = _x ^ _y;
					break;
				case NEG:
					z = ~_x;
					break;
				case REM:
					z = _x % _y;
					break;
				}
				v1 = z;
				r1 = c->dest;
			}
			else {
				uint z1, z2;
				if (c->c_type == MUL) {
					z1 = (size_t)_x * _y >> 32;
					z2 = (size_t)_x * _y ^ (z1 << 32ll);
				}
				else {
					z1 = _x % _y;
					z2 = _x / _y;
				}
				r1 = &R[32]; r2 = &R[33];
				v1 = z1; v2 = z2;
			}
		}
		else {
			int _x = x[0], _y = x[1];
			if (c->dest != NULL) {
				int z = 0;
				switch (c->c_type) {
				case ADD:
					z = _x + _y;
					break;
				case SUB:
					z = _x - _y;
					break;
				case MUL:
					z = _x * _y;
					break;
				case DIV:
					z = _x / _y;
					break;
				case XOR:
					z = _x ^ _y;
					break;
				case NEG:
					z = -_x;
					break;
				case REM:
					z = _x % _y;
					break;
				}
				r1 = c->dest;
				v1 = z;
			}
			else {
				int z1, z2;
				if (c->c_type == MUL) {
					z1 = (long long)_x * _y >> 32;
					z2 = (long long)_x * _y ^ (z1 << 32ll);
				}
				else {
					z1 = _x % _y;
					z2 = _x / _y;
				}
				r1 = &R[32]; r2 = &R[33];
				v1 = z1; v2 = z2;
			}
		}
		break;
	}
	case CMP: {
		Cmp *c = (Cmp*)id_ex;
		bool z;
		switch (c->c_type)
		{
		case CGT:
			z = (x[0] > x[1]);
			break;
		case CGE:
			z = (x[0] >= x[1]);
			break;
		case CLT:
			z = (x[0] < x[1]);
			break;
		case CLE:
			z = (x[0] <= x[1]);
			break;
		case CEQ:
			z = (x[0] == x[1]);
			break;
		case CNEQ:
			z = (x[0] != x[1]);
			break;
		default:
			break;
		}
		r1 = c->dest;
		v1 = z;
		break;
	}
	case BRANCH: {
		Branch* c = (Branch*)id_ex;
		bool z;
		if (c->b_type == B) z = 1;
		else {
			switch (c->b_type)
			{
			case BGT:
				z = (x[0] > x[1]);
				break;
			case BGE:
				z = (x[0] >= x[1]);
				break;
			case BLT:
				z = (x[0] < x[1]);
				break;
			case BLE:
				z = (x[0] <= x[1]);
				break;
			case BEQ:
				z = (x[0] == x[1]);
				break;
			case BNE:
				z = (x[0] != x[1]);
				break;
			default:
				break;
			}
		}
		SaveHistory(c, z);
		if (z != isBranch) {
			instruction = id_ex = NULL;
			count = 0;
			R[34] = other;
		}
		/*
		if (z) {
			v1 = L[c->label] - memory_start;
			r1 = &R[34];
		}
		else Lock[34]--;
		*/
		break;
	}
	case JUMP: {
		Jump * c = (Jump*)id_ex;
		/*
		if (c->j_type == J || c->j_type == JR) {
			r1 = &R[34];
			v1 = x[0];
		}
		else {
				r1 = &R[34]; v1 = x[0];
				r2 = &R[31]; v2 = c->nextPosition;
		}
		*/
		if (c->j_type == JAL || c->j_type == JALR)
			r1 = &R[31], v1 = c->nextPosition;
		break;
	}
	case LOAD: {
		Load* c = (Load*)id_ex;
		m_type = c->l_type;
		if (count == 2)	add = x[0] + x[1];
		else add = x[0];
		r1 = c->dest;
		break;
	}
	case STORE: {
		Store* c = (Store*)id_ex;
		m_type = c->s_type;
		if (count == 3) add = x[1] + x[2];
		else add = x[1];
		v1 = x[0];
		break;
	}
	case MOVE: {
		Move *c = (Move*)id_ex;
		r1 = c->dest;
		v1 = x[0];
	}
	}
	count = 0;
	id_ex = NULL;
}
void Pipeline::MEM() {
	if (type == -1) return;
	switch (type)
	{
	case LOAD:
		_r1 = r1;
		switch (m_type)
		{
		case ADDRESS:
			_v1 = add;
			break;
		case BYTE:
			_v1 = *(char*)(memory_start + add);
			break;
		case HALF:
			_v1 = *(short int*)(memory_start + add);
			break;
		case WORD:
			_v1 = *(int*)(memory_start + add);
			break;
		case CONST:
			_v1 = add;
			break;
		default:
			break;
		}
		break;
	case STORE:
		switch (m_type)
		{
		case BYTE:
			*(memory_start + add) = v1;
			break;
		case HALF:
			*(short int*)(memory_start + add) = v1;
			break;
		case WORD:
			*(int*)(memory_start + add) = v1;
			break;
		default:
			break;
		}
		break;
	case SYS:
		switch (m_type)
		{
		case 1:
			cout << v1;
			break;
		case 4:
			cout << (char*)(v1 + memory_start);
			break;
		case 5:
			_r1 = &R[2];
			cin >> _v1;
			break;
		case 8: {
			string s;
			cin >> s;
			for (int i = 0; i < std::min(v2 - 1, (int)s.length()); i++) {
				*(char*)(v1 + memory_start) = s[i];
				v1++;
			}
			*(char*)(v1 + memory_start) = 0;
			break;
		}

		case 9:
			if ((heap - memory_start) % 4 != 0) {
				heap = memory_start + ((heap - memory_start) / 4 + 1) * 4;
			}
			_r1 = &R[2];
			_v1 = heap - memory_start;
			heap += v1;
			break;
		case 10:
			isEnded = 1;
			return_value = 0;
			break;
		case 17:
			isEnded = 1;
			return_value = v1;
			break;
		default:
			break;
		}
		break;
	default:
		_r1 = r1, _r2 = r2;
		_v1 = v1, _v2 = v2;
		break;
	}
	r1 = r2 = NULL;
	type = -1;
}
void Pipeline::WB() {
	if (_r1) *_r1 = _v1, Lock[_r1 - R]--;
	if (_r2) *_r2 = _v2, Lock[_r2 - R]--;
	_r1 = _r2 = NULL;
}
