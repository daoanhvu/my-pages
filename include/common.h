#ifndef _COMMON_H_
#define _COMMON_H_

#define COMMA			0x0000002C
#define SEMI			0x0000003B
#define AND				0x00002227
#define OR				0x00002228
#define GT				0x000000E3 //>
#define LT				0x0000003C //<
#define GTE				0x00002265 //>=
#define LTE				0x00002264 //<=
#define NE				0x00002260 // not equals !=
#define EQ				0x0000003D // equals =
#define LPAREN			0x00000028
#define RPAREN			0x00000029
#define LPRACKET 		0x0000005B
#define RPRACKET 		0x0000005D
#define ELEMENT_OF 		0x00002208
#define PLUS 			0x0000002B
#define MINUS 			0x0000002D
#define MULTIPLY 		0x000000D7
#define DIVIDE 			0x000000F7
#define POWER 			0x0000005E
#define SQRT 			0x0000221A
#define CBRT 			0x0000221B
#define PI_TYPE 		0x000003C0
#define E_TYPE 			0x0000212F
#define TYPE_INFINITY	0x0000221E

#define NUMBER 			100
#define INT				101
#define LONG_INT		102
#define OBJ_TEXT		103
#define ENDOBJ_TEXT		104
#define OCTET_STREAM	105

#define OPEN_D_ARROW	106		// <<
#define CLOSE_D_ARROW 	107		// >>
#define OPN_PARENT		110
#define CLO_PARENT		111
#define STREAM_TEXT		112
#define ENDSTREAM_TEXT	113
#define SLASH			114
#define CODE_NEWLINE	115
#define START_PDF		116
#define END_OF_PDF		117

#define SPACE 		0x20
#define ERROR_PARSE_DOUBLE -1
#define ERROR_TOO_MANY_FLOATING_POINT -5

typedef struct tagToken {
	short type;
	void *data;
	int dataLength;
} PDFToken;

typedef struct tagTokenList {
	PDFToken **list;
	int logSize;
	int size;
} TokenList;


int parseInt(const char *str, int len);
double parseDouble(const char *str, int start, int end, int *error);

#endif
