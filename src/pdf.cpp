//#include <pdf.h>
#include "../include/pdf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

//utility
int scompare(const char *s1, int offs1, int l1, const char *s2) {
	char b[64];
	memcpy(b, s1+offs1, l1);
	b[l1] = 0;
	return strcmp(b, s2);
}


PDF::PDF() {
	this->marginLeft = 0;
	marginTop = 0;
	marginRight = 0;
	marginBottom = 0;

	major = 1;
	minor = 0;

	list_objects = 0;
	listLogSize = 0;
	listSize = 0;

	pageParent = 0;
	root = 0;

	pages = 0;
	pageLogCount = 0;
	pageCount = 0;

	//This array stored index of indirect objects in objects
	rTable = 0;
	rTableLogSize = 0;
	rTableSize = 0;
}

PDF::PDF(char majorV, char minorV) {
	this->marginLeft = 0;
	marginTop = 0;
	marginRight = 0;
	marginBottom = 0;

	major = majorV;
	minor = minorV;

	list_objects = 0;
	listLogSize = 0;
	listSize = 0;
	pageParent = 0;
	root = 0;

	pages = 0;
	pageLogCount = 0;
	pageCount = 0;

	//This array stored index of indirect objects in objects
	rTable = 0;
	rTableLogSize = 0;
	rTableSize = 0;
}

PDF::~PDF() {

}

int PDF::addObject(PDFObject *obj) {
	if(this->listSize >= this->listLogSize) {
		this->listLogSize += 4;
		list_objects = (PDFObject**) realloc(list_objects, sizeof(PDFObject*) * listLogSize);
	}

	list_objects[this->listSize++] = obj;

	return this->listSize;
}

int PDF::addPage(Page *p) {
	if(this->pageCount >= this->pageLogCount) {
		this->pageLogCount += 4;
		pages = (Page**) realloc(pages, sizeof(Page*) * pageLogCount);
	}

	pages[this->pageCount++] = p;
	return this->pageCount;
}

void PDF::initBlankPDF() {
	this->listLogSize = 2;
	list_objects = (PDFObject**) malloc(sizeof(PDFObject*) * listLogSize);
	listSize = 2;

	list_objects[0] = createObject();
	list_objects[0]->type = CATALOG;
	list_objects[0]->gen_num = 0;
	list_objects[0]->index = 1;
	list_objects[0]->flags = ISUSED | INDIRECT;
	list_objects[0]->elements = NULL;
	list_objects[0]->eleCount = 0;
}

void PDF::addPage() {
	Page *page;
	PDFObject *obj = createObject();

	obj->type = PAGE;
	obj->gen_num = 0;
	obj->index = 4;
	obj->flags = INDIRECT;
	obj->elements = NULL;
	obj->eleCount = 0;
	page = (Page*)malloc(sizeof(Page));
	page->parent = pageParent;
	page->mediaBox.llx = 0;
	page->mediaBox.lly = 0;
	page->mediaBox.urx = 288;
	page->mediaBox.ury = 500;
	page->objects = NULL;
	page->refCount = 0;
	obj->data = page;

	addPage(page);
	addObject(obj);
}

int PDF::read(std::istream &in, char *msg) {
	char buff[128];
	char buff2[32];
	int lenth = in.tellg();
	int i, lenRead;
	int curPos;

	in.read(buff, 128);

	if(!in) {
		lenRead = in.gcount();
		if(lenRead < 14) {
			strcpy(msg, "Not a PDF file or file damaged.");
			return 0;
		}

		if( (buff[0] == '%') && (buff[1] =='P') && (buff[2] =='D') && (buff[3] =='F')
				&& (buff[4] =='-')) {
			if( ((buff[5]>=48) || (buff[5]<57)) && buff[6] =='.' &&
					((buff[7]>=48) || (buff[7]<57))	) {

				this->readContent(in, buff, 8, lenRead - 8);

			}
			return 0;
		} else {
			strcpy(msg, "Not a PDF file or file damaged.");
			return 0;
		}
	}

	if( (buff[0] == '%') && (buff[1] =='P') && (buff[2] =='D') && (buff[3] =='F')
					&& (buff[4] =='-')) {
		if( ((buff[5]>=48) || (buff[5]<57)) && buff[6] =='.' &&
						((buff[7]>=48) || (buff[7]<57))	) {

			//Read startxref
			curPos = in.tellg();
			in.seekg(32, std::istream::end);
			in.read(buff2, 32);
			i = 0;
			while(i < 32) {
				if(scompare(buff2, i, 9, "startxref")) {
					break;
				}
				i++;
			}

			//Read content
			this->readContent(in, buff, 8, lenRead - 8);

		}
	} else {
		strcpy(msg, "Not a PDF file or file damaged.");
		return 0;
	}

	return 0;
}

int PDF::readContent(std::istream &in, const char *inbuff, int offs, int len) {
	char buff[128];
	int l;
	while(in.read(buff, 128)) {

	}

	l = in.gcount();

	if(l > 0) {

	}

	return 0;
}

int PDF::write(std::ostream &out) {
	int l, length = 0;
	char buff[128];
	//write file header
	l = sprintf(buff, "%%PDF-%d.%d\r\n", this->major, this->minor);
	out.write(buff, l);
	length = l;

	//startxref
	l = sprintf(buff, "startxref\r\n%ld\r\n%%%%EOF", xref_addr);
	out.write(buff, l);
	length += l;
	out.flush();
	return length;
}
