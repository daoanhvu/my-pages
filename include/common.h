#ifndef _COMMON_H_
#define _COMMON_H_

#define NUMBER 			100
#define INT				101
#define LONG_INT		102
#define OBJ_TEXT		103
#define ENDOBJ_TEXT		104
#define OCTET_STREAM	105

#define OPEN_2ARROW		106		// <<
#define CLOSE_2ARROW 	107		// >>
#define OPEN_BRACKET	108
#define CLOSE_BRACKET	109
#define OPN_PARENT		110
#define CLO_PARENT		111
#define STREAM_TEXT			112
#define ENDSTREAM_TEXT		113
#define SLASH			114

#define SPACE 		0x20
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

#endif
