#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <npdf.h>
#include "../include/common.h"
#include "../include/lexer.h"

using namespace std;

int main(int argc, char **args) {
	ifstream f;
	char buf[128];
	int size, byteRead, l, remainingSize;
	TokenList list;

	list.list = NULL;
	list.logSize = 0;
	list.size = 0;

	f.open("D:\\data\\test.pdf", ios::binary);
	f.seekg(0, std::ios_base::end);
	size = f.tellg();
	byteRead = size>128?128:size;
	f.seekg(0, std::ios_base::beg);
	f.read(buf, byteRead);
	while(size - byteRead > 0) {
		remainingSize = size - byteRead;
		l = remainingSize>128?128:remainingSize;
		f.read(buf, l);
		byteRead += l;
	}
	f.close();

	lexicalAnalysis(buf, byteRead, &list);

	for(l=0; l<list.size; l++) {
		cout << "Type: "  << list.list[l]->type << endl;
	}

	releaseTokenList(&list);

	return 0;
}
