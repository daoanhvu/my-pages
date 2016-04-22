#ifndef __NPDF_H_
#define __NPDF_H_

#define CR 0x0D
#define LF 0x0A
#define SP 0x20
#define FF 0x0C //Form Feed
//#define NULL 0x00
#define SL 0x2F // /
#define OPN 0x3C
#define CLO 0x3E

//These are used for flags
#define INDIRECT 0x00000001
#define ISUSED 0x00000002

#define NAME 			0
#define CATALOG 		1
#define OUTLINES 		2
#define PAGES 			3
#define PAGE 			4
#define XREF 			5
#define STREAM			6
#define TEXT 			7
#define FONT 			8
#define ARRAY 			9
#define XOBJECT			10


struct tagElement;
typedef tagElement Element;

typedef struct tagStream {
	char *data;
	int length;
} Stream;

typedef struct tagObject{
	int type;
	
	Element *elements;
	short eleCount;
	
	unsigned int offset;
	int index;
	int gen_num;
	int flags;
	void *data;
	Stream *stream;
} PDFObject;

//======= Common data structures ========================
typedef struct tagRectangle {
	int llx;
	int lly;
	int urx;
	int ury;
} Rectangle;
//=======================================================

typedef struct tagCatalog {
	PDFObject **refObjects;
	int refCount;
} Catalog;

typedef struct tagOutline {
	int count;
} Outline;

/**
	These two following types are used for array objects
	type: Object, Name, text, Number
	type:
		0: just name
		1: name and string value
		2: name and iValue
		3: indirect object
*/
struct tagElement{
	short type;
	char key[16];
	char value[32];
	int iValue;
	PDFObject *obj;
};

typedef struct tagArray{
	Element *elements;
	int size;
} Array;

typedef struct tagPage {
	PDFObject *parent;
	int *objects;
	int refCount;
	Rectangle mediaBox;
} Page;

typedef struct tagXObject{
	char *stream_data;
	unsigned int stream_length;
} XObject;

typedef struct tagFont{
	char subtype[16];
	char baseFont[32];
	char name[16];
	char encoding[32];
	float size;
} Font;

typedef struct tagXRefElement {
	int startIdx;
	int count;
} XRefElement;

typedef struct tagPDF {
	float marginLeft;
	float marginTop;
	float marginRight;
	float marginBottom;
	
	char major;
	char minor;

	PDFObject **list_objects;
	int listLogSize;
	int listSize;

	//This array stored index of indirect objects in objects
	XRefElement *rTable;
	int rTableLogSize;
	int rTableSize;
	
	/**
	 * Trailer information
	 *
	 */
	int rootIdx; //Index of the root object in array list_objects

} PDF;

void init(PDF &pdf, int major, int minor);
PDFObject* createObject();
void initObject(PDFObject *);
void releaseObj(PDFObject *);
void release(PDF &pdf);
void addXrefTable(PDF &pdf);
PDFObject* addText(PDF &pdf, int pageIdx, char *str, int l, Font *f);
int writeDPF2File(const char *filename, const PDF &pdf);
#endif
