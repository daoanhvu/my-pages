#include "../include/lexer.h"
#include "../include/common.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

void releaseTokenList(TokenList *tokens) {
	int i;
	PDFToken *p;

	if(tokens == NULL)
		return;

	for(i=0; i<tokens->size; i++) {
		p = tokens->list[i];
		if(p->data != NULL) {
			free(p->data);
		}
		free(p);
	}

	free(tokens->list);
}

int addToken(int _type, const void *_data, int len, TokenList *tokens) {
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
		tokens->list = (PDFToken**)realloc(tokens->list, tokens->logSize * sizeof(PDFToken*));
	}
	tokens->list[tokens->size] = token;
	tokens->size++;
	return 0;
}

/**
 * @param: end - excluded end
 */
int readStream(const char * bf, int *start, int *end) {
	int l = 0;
	int i;
	if(bf[l] == '\n' || bf[l] == 0x0A ) {
		(*start)++;
	} else if(bf[l] == 0x0D || bf[l] == 0x0A ) {
		(*start) += 2;
	}

	i = (*start);

	while( !(bf[i]=='e' && bf[i+1]=='n' && bf[i+2]=='d'
			&& bf[i+3]=='s' && bf[i+4]=='t' && bf[i+5]=='r'
					&& bf[i+6]=='e' && bf[i+7]=='a' && bf[i+8]=='m') ) {
		i++;
	}

	(*end) = i;

	return ( (*end) - (*start) );
}

int lexicalAnalysis(const char *bf, int len, TokenList *list) {
	int idx = 0;
	int k, intValue;
	int floatingPoint;
	int start, end;

	while(idx<len) {
		if(bf[idx]=='%') {
			if(bf[idx+1]=='P' && bf[idx+2]=='D' && bf[idx+3]=='F' ) {
				// Not implemented
				addToken(START_PDF, NULL, 0, list);
				idx += 8;
			} else if(bf[idx+1] =='%' && bf[idx+2]=='E' && bf[idx+3]=='O' && bf[idx+4]=='F' ) {
				addToken(END_OF_PDF, NULL, 0, list);
				idx += 5;
			}
		} else if( isdigit(bf[idx]) ) {
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
						if(floatingPoint) {
							//Not implemented yet
						} else {
							intValue = parseInt(bf+idx, k-idx);
							addToken(INT, &intValue, sizeof(int), list);
							idx = k;
						}
						break;
					}
				}
			}
			if(idx < k) {
				if(floatingPoint) {
					//Not implemented yet
				} else {
					intValue = parseInt(bf+idx, k-idx);
					addToken(INT, &intValue, sizeof(int), list);
					idx = k;
				}
			}
		} else if(bf[idx]==' ') {
			addToken(SPACE, bf + idx, k-idx, list);
			idx++;
		} else if(bf[idx]=='<') {
			if(bf[idx+1]=='<') {
				addToken(OPEN_D_ARROW, NULL, 0, list);
				idx += 2;
			} else {
				//Not implemented yet
			}
		} else if(bf[idx]=='>') {
			if(bf[idx+1]=='>') {
				addToken(CLOSE_D_ARROW, NULL, 0, list);
				idx += 2;
			} else {
				//Not implemented yet
			}
		} else if(bf[idx]=='[') {
			addToken(LPRACKET, NULL, 0, list);
			idx++;
		} else if(bf[idx]==']') {
			addToken(RPRACKET, NULL, 0, list);
			idx++;
		} else if(bf[idx]=='o' && bf[idx+1]=='b' && bf[idx+2]=='j') {
			addToken(OBJ_TEXT, NULL, 0, list);
			idx += 3;
		} else if(bf[idx]=='s' && bf[idx+1]=='t' && bf[idx+2]=='r'
				&& bf[idx+3]=='e' && bf[idx+4]=='a' && bf[idx+5]=='m' ) {
			addToken(STREAM_TEXT, NULL, 0, list);
			idx += 6;
			k = readStream(bf + idx, &start, &end);
			if( k > 0 ) {
				addToken(STREAM_DATA, bf + start, k, list);
				idx += k;
			}
		} else if(bf[idx]=='e' && bf[idx+1]=='n' && bf[idx+2]=='d') {
			if(bf[idx+3]=='o' && bf[idx+4]=='b' && bf[idx+5]=='j') {
				addToken(ENDOBJ_TEXT, NULL, 0, list);
				idx += 6;
			} else if(bf[idx+3]=='s' && bf[idx+4]=='t' && bf[idx+5]=='r'
					&& bf[idx+6]=='e' && bf[idx+7]=='a' && bf[idx+8]=='m') {
				addToken(ENDSTREAM_TEXT, NULL, 0, list);
				idx += 9;
			}
		} else if(bf[idx]=='/') {
			addToken(SLASH, NULL, 0, list);
			idx++;
			if(bf[idx]=='R' && bf[idx+1]=='o' && bf[idx+2]=='o' && bf[idx+3]=='t') {
				addToken(NAME_ROOT, NULL, 0, list);
			} else if(bf[idx]=='S' && bf[idx+1]=='i' && bf[idx+2]=='s' && bf[idx+3]=='e') {
				addToken(NAME_SIZE, NULL, 0, list);
			}
		} else if(bf[idx]=='R' && isTrimmedChar(bf[idx+1]) ) {
			addToken(R_TEXT, NULL, 0, list);
			idx++;
		} else if(bf[idx]==0x0A) {
			addToken(CODE_NEWLINE, NULL, 0, list);
			idx++;
		} else if( (bf[idx]==0x0D) && (bf[idx+1]==0x0A) ) {
			addToken(CODE_NEWLINE, NULL, 0, list);
			idx += 2;
		} else {
			idx++;
		}
	}

	return 0;
}
