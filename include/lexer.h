#ifndef _LEXER_H_
#define _LEXER_H_

#include "common.h"

void releaseTokenList(TokenList *tokens);
int lexicalAnalysis(const char *, int len, TokenList *tokens);

#endif
