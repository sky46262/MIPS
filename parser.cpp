#include "parser.h"
#define uint unsigned int
std::map<string, Register> R_index;//register
std::map<string, int> L_index;
uint L_cnt = 0;
extern vector<char*> L;
Token::Token(string _value, Token_type _type) :value(_value), type(_type) {}
Token::~Token() {}

Token_type Token::getTokenType() {
	return type;
}

int Token::toInt() {
	string tmp = value;
	if (tmp.back() == ':') tmp.pop_back();
	return atoi(tmp.c_str());
}

string Token::toString() {
	return value;
}
Parser::Parser(string name) :pos_buffer(0), savedChar(0){
	file = fopen(name.c_str(), "r");
	buffer = new char[BUFFER_SIZE + 1];
	memset(buffer, 0, sizeof(buffer));
	size = fread(buffer, sizeof(char), BUFFER_SIZE, file);

	for (int i = 0; i < 35; i++) R_index[R_name[i]] = i;
}
Parser::~Parser() {
	delete buffer;
}
bool Parser::isInt(char c) {
	return (c >= '0' && c <= '9');
}
bool Parser::isLetter(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
char Parser::getNextChar() {
	char tmp;
	if (savedChar) {
		tmp = savedChar;
		savedChar = 0;
	}
	else {
		if (pos_buffer == size) {
			size = fread(buffer, sizeof(char), BUFFER_SIZE, file);
			if (size == 0) throw exception();
			pos_buffer = 0;
		}
		tmp = buffer[pos_buffer++];
	}
	if (tmp == '\\') {
		char tmp2 = getNextChar();
		switch (tmp2) {
		case 'n':
			return '\n';
			break;
		case 't':
			return '\t';
			break;
		case '0':
			return 0;
			break;
		default:
			return tmp2;
		}
	}
	else return tmp;
}

bool Parser::hasMoreChars() {
	if (savedChar || pos_buffer < size) return 1;
	size = fread(buffer, sizeof(char), BUFFER_SIZE, file);
	pos_buffer = 0;
	if (size == 0) return 0;
	else return 1;
}

bool Parser::hasMoreTokens() {
	if (savedChar) return 1;
	char tmp;
	while (hasMoreChars()) {
		tmp = getNextChar();
		if (tmp != ' ' && tmp != '\t') {
			savedChar = tmp;
			return 1;
		}
	}
	savedChar = 0;
	return 0;
}


Token Parser::getNextToken() {
	string tmp;
	char ch;
	if (!hasMoreTokens()) throw exception();
	//skip whitespace
	ch = getNextChar();
	if (ch == '(') {
		ch = getNextChar();// $
		ch = getNextChar();
		bool isI = true;
		while (ch != ')') {
			if (!isInt(ch)) isI = false;
			tmp.push_back(ch);
			ch = getNextChar();
		}
		if (isI == false) {
			char t[20];
			sprintf(t, "%d", R_index[tmp.c_str()]);
			tmp = t;
			return Token(tmp, BRACKET);
		}
		else return Token(tmp, BRACKET);
	}
	if (ch == '"') {
		ch = getNextChar();
		while (ch != '"') {
			tmp.push_back(ch);
			ch = getNextChar();
		}
		return Token(tmp, STRING);
	}
	bool flag_negint = false, flag_register = false;
	if (ch == '$') {
		flag_register = true;
		ch = getNextChar();
	}
	if (ch == '#') {
		while (ch != '\n') ch = getNextChar();
		//skip until \n
	}
	tmp.push_back(ch);
	if (ch == ',' || ch == '\n') return Token(tmp, SEPERATOR);
	int isneg = 1;
	if (!isInt(ch) && ch != '-') flag_negint = true;
	while (hasMoreChars()) {
		ch = getNextChar();
		if (ch == ' ' || ch == '\t') break;
		if (ch == ',' || ch == '(' || ch == '\n') {
			savedChar = ch;
			break;
		}
		if (ch == '#') {
			while (ch != '\n') ch = getNextChar();
			savedChar = ch;
			break;
		}
		tmp.push_back(ch);
		if (!isInt(ch)) flag_negint = true;
	}
	//Register : name->number
	if (flag_register) {
		if (flag_negint == true) {
			char t[20];
			sprintf(t, "%d", R_index[tmp.c_str()]);
			tmp = t;
			return Token(tmp, REGISTER);
		}
		else return Token(tmp, REGISTER);
	}
	if (!flag_negint) return Token(tmp, INT);
	for (int i = 0; i < KEYWORD_SIZE; i++)
		if (tmp == keywords[i]) return Token(tmp, KEYWORD);
	bool flag = (tmp.back() == ':');
	if (flag) tmp.pop_back();
	if (!L_index.count(tmp)) L_index[tmp] = L_cnt++, L.push_back(NULL);
	char t[20];
	sprintf(t, "%d", L_index[tmp.c_str()]);
	tmp = t;
	if (flag)
	return Token(tmp + ":", LABEL);//with ':' at last
	else 
	return Token(tmp, LABEL);
}
