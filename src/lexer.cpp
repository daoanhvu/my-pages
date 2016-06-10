#include "../include/lexer.h"
#include "../include/common.h"
#include <string.h>
#include <ctype.h>

int addToken(int _type, const char *_text, char txtlen, int _col) {

}

int lexicalAnalysis(const char *bf, int len) {
	int idx = 0;
	int k;
	int floatingPoint;

	while(idx<len) {
		if( isdigit(bf[idx]) ) {
			floatingPoint = 0; //FALSE;
			for(k = idx+1; k < len; k++) {
				if( !isdigit(bf[k]) ) {
					if(bf[k] == '.') {
						//check if we got a floating point
						if(floatingPoint) { //<- ERROR: the second floating point
							//errorColumn = k;
							return ERROR_TOO_MANY_FLOATING_POINT;
						}
						floatingPoint = 1; //TRUE;
					} else {
						addToken(NUMBER, bf + idx, k-idx, idx);
						idx = k;
						break;
					}
				}
			}
			if(idx < k) {
				addToken(NUMBER, bf + idx, k-idx, idx);
				idx = k;
			}
		} else if(bf[idx]==' ') {
			addToken(SPACE, bf + idx, k-idx, idx);
			idx++;
		} else if(bf[idx]=='\n') {
			addToken(SPACE, bf + idx, k-idx, idx);
			idx++;
		}
	}

	return 0;
}
