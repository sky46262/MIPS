#include "program.h"
#include<iostream>
using namespace std;

int main(int argc, char** argv) {
	Program(argv[1]);
	Data(argv[1]);
	Pipeline p;
	return p.exec();
}
