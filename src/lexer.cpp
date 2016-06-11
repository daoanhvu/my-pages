#include "../include/lexer.h"
#include "../include/common.h"
#include <malloc.h>
#include <string.h>
#include <ctype.h>

int addToken(int _type, const void *_data, char len, TokenList *tokens) {
	PDFToken *token = (PDFToken*)malloc(sizeof(PDFToken));
	token->type = _type;
	switch(token->type) {
		case INT:
			token->data = malloc(sizeof(int));
			memcpy(token->data, _data, sizeof(int));
			token->dataLength = sizeof(int);
			break;

		case OCTET_STREAM:
			token->data = malloc(len);
			memcpy(token->data, _data, len);
			token->dataLength = len;
			break;

		default:
			token->data = NULL;
			token->dataLength = 0;
			break;
	}

	if(tokens->size <= tokens->logSize) {
		tokens->logSize += 5;
		tokens->list = realloc(tokens->list, tokens->logSize * sizeof(PDFToken*));
	}
	tokens->list[tokens->size] = token;
	tokens->size++;
	return 0;
}

int lexicalAnalysis(const char *bf, int len, TokenList *list) {
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
						addToken(NUMBER, bf + idx, k-idx, list);
						idx = k;
						break;
					}
				}
			}
			if(idx < k) {
				addToken(NUMBER, bf + idx, k-idx, list);
				idx = k;
			}
		} else if(bf[idx]==' ') {
			addToken(SPACE, bf + idx, k-idx, list);
			idx++;
		} else if(bf[idx]=='\n') {
			addToken(SPACE, bf + idx, k-idx, list);
			idx++;
		}
	}

	return 0;
}
