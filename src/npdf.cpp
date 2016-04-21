#ifdef WINDOWS
#include "stdafx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
//#include <npdf.h>
#include "../include/npdf.h"

void long2String(long n, char *str, int offset, int *l) {
	int i, j, l2;
	long tmpn;
	char r;

	if (n == 0) {
		str[offset] = 48;
		*l = 1;
		return;
	}
	tmpn = n;
	*l = 0;
	while (tmpn >0) {
		r = (char)((tmpn % 10) + 48);
		str[offset + (*l)++] = r;
		tmpn = tmpn / 10;
	}
	l2 = (*l) >> 1;
	for (i = 0; (i<l2) && (n>9) ; i++) {
		r = str[offset + i];
		j = offset + (*l) - i - 1;
		str[i] = str[j];
		str[j] = r;
	}
}

void addText(PDF &, char *str, int l, Font *f) {

}

void addXrefEntry(PDF *pdf, int startindex, int count) {
	if (pdf->rTableLogSize <= pdf->rTableSize) {
		pdf->rTableLogSize += 4;
		pdf->rTable = (XRefElement*)realloc(pdf->rTable, sizeof(XRefElement)*pdf->rTableLogSize);
	}
	pdf->rTable[pdf->rTableSize].startIdx = startindex;
	pdf->rTable[pdf->rTableSize].count = count;
	pdf->rTableSize++;
}

void addRefSubobject(const PDFObject *obj, char *buff, int offs, int *len) {
	int p, l;
	switch (obj->type) {
	case OUTLINES:
		buff[offs] = '/';
		memcpy(buff + offs + 1, "Outlines ", 9);
		p = offs + 10;
		break;

	case PAGES:
		buff[offs] = '/';
		memcpy(buff + offs + 1, "Pages ", 6);
		p = offs + 7;
		break;
	}

	l = sprintf(buff + p, "%d %d R", obj->index, obj->gen_num);
	*len = p + l - offs;
}

/**
	return length of the page in byte
*/
int writePageContent(Page *page, std::ostream &out) {
	int total = 0, l;
	int i;
	char buff[64];
	
	for(i=0; i<page->refCount; i++) {
		
	}
	
	//Write mediabox
	l = sprintf(buff, "/MediaBox [%d %d %d %d]\r\n", page->mediaBox.llx, page->mediaBox.lly,
			page->mediaBox.urx, page->mediaBox.ury);
	out.write(buff, l);
	total += l;

	//Write parent
	l = sprintf(buff, "/Parent %d %d R\r\n", page->parent->index, page->parent->gen_num);
	out.write(buff, l);
	total += l;

	return total;
}

/**
	This function is used to write elements of an object
	Ex:
		/NAME /STRING_VALUE
		/Size 299
*/
int writeElement(const PDFObject *obj, std::ostream &f){
	int i, length = 0;
	int keylen, vlen;
	char buff[64];
	for(i=0; i<obj->eleCount; i++) {
		keylen = strlen(obj->elements[i].key);
		vlen = strlen(obj->elements[i].value);
		buff[0] = '/';
		memcpy(buff + 1, obj->elements[i].key, keylen);
		buff[keylen + 1] = SP;
		if(obj->elements[i].type == 1) {
			buff[keylen + 2] = '/';
			memcpy(buff + keylen + 3, obj->elements[i].value, vlen);
			buff[keylen + vlen + 3] = CR;
			buff[keylen + vlen + 4] = LF;
			f.write(buff, keylen + vlen + 5);
			length += keylen + vlen + 5;
		} else if(obj->elements[i].type == 2) {
			vlen = sprintf(buff + (keylen + 2), "%d\r\n", obj->elements[i].iValue);
			length += keylen + vlen + 2;
		} else if(obj->elements[i].type == 3) {
			
		}
	}
	return length;
}

PDFObject* createObject() {
	PDFObject *obj = (PDFObject*)malloc(sizeof(PDFObject));
	obj->type = 0;
	obj->gen_num = 0;
	obj->index = 0;
	obj->flags = 0;
	obj->elements = NULL;
	obj->eleCount = 0;
	obj->data = NULL;

	return obj;
}

void initObject(PDFObject *obj, int type) {
	obj->type = type;
	obj->gen_num = 0;
	obj->index = 0;
	obj->flags = ISUSED | INDIRECT;
	obj->elements = NULL;
	obj->eleCount = 0;
	obj->data = NULL;
}

void releaseObj(PDFObject *){

}


void init(PDF &pdf, int major, int minor) {
	pdf.major = major;
	pdf.minor = minor;
	pdf.list_objects = NULL;
	pdf.listLogSize = 0;
	pdf.listSize = 0;
	pdf.rTable = NULL;
	pdf.rTableLogSize = 0;
	pdf.rTableSize = 0;

	pdf.rootIdx = -1;
}

void release(PDF &pdf) {
	int i;
	if (pdf.list_objects != NULL) {
		for(i = 0; i<pdf.listSize; i++) {
			releaseObj(pdf.list_objects[i]);
		}
		free(pdf.list_objects);
		pdf.list_objects = NULL;
	}

	if (pdf.rTable != NULL) {
		free(pdf.rTable);
		pdf.rTable = NULL;
		pdf.rTableLogSize = 0;
		pdf.rTableSize = 0;
	}
}

/**
return number of indirect objects
*/
void addXrefTable(PDF &pdf) {
	PDFObject *obj;
	int i, count = 0;
	int start;
	bool seen = false;
	for (i = 0; i<pdf.listSize; i++) {
		obj = pdf.list_objects[i];
		//if this is an indirect object then we add it into xref table
		if ((obj->flags & INDIRECT) == INDIRECT) {
			if(!seen) {
				seen = true;
				count = 1;
				start = i;
			} else {
				count++;
			}
		} else {
			seen = false;
			if(count > 0) {
				addXrefEntry(&pdf, start, count);
				count = 0;
			}
		}
	}

	if(count > 0) {
		addXrefEntry(&pdf, start, count);
		count = 0;
	}
}

int writeDPF2File(const char *filename, const PDF &pdf) {
	char buff[128];
	char buff2[16];
	char isUsed;
	int i, j, l1, l, tmp, k;
	long xref_addr;
	long length = 0;
	PDFObject *obj;
	Catalog* catalog;
	Page *page;
	XRefElement *xref;

	std::ofstream f(filename, std::ofstream::binary);

	//write file header
	l = sprintf(buff, "%%PDF-%d.%d\r\n", pdf.major, pdf.minor);
	f.write(buff, l);
	length = l;

	//write objects
	for (i = 0; i<pdf.listSize; i++) {
		obj = pdf.list_objects[i];

		obj->offset = length;

		l = sprintf(buff, "%d %d obj\r\n<<", obj->index, obj->gen_num);
		f.write(buff, l);
		length += l;
		
		//Write object's name elements
		l = writeElement(obj, f);
		length += l;

		//object's content
		switch (obj->type) {

		case CATALOG:
			buff[0] = '/';
			buff[1] = 'T';
			buff[2] = 'y';
			buff[3] = 'p';
			buff[4] = 'e';
			buff[5] = SP;
			memcpy(buff + 6, "/Catalog", 8);

			catalog = (Catalog*)obj->data;
			l = 14;
			for (j = 0; j<catalog->refCount; j++) {
				addRefSubobject(catalog->refObjects[j], buff, l, &l1);
				l += l1;
				buff[l++] = SP;
			}

			buff[l++] = CR;
			buff[l++] = LF;
			f.write(buff, l);
			length += l;
			break;

		case OUTLINES:
			buff[0] = '/';
			buff[1] = 'T';
			buff[2] = 'y';
			buff[3] = 'p';
			buff[4] = 'e';
			buff[5] = SP;
			buff[6] = '/';
			memcpy(buff + 7, "Outlines", 8);
			buff[15] = CR;
			buff[16] = LF;
			f.write(buff, 17);
			length += 17;
			break;

		case PAGES:
			memcpy(buff + 7, "/Type /Pages\r\n", 14);
			f.write(buff, 14);
			length += 14;
			break;

		case PAGE:
			memcpy(buff + 7, "/Type /Page\r\n", 13);
			f.write(buff, 13);
			length += 13;
			
			page = (Page*)obj->data;
			l = writePageContent(page, f);
			length += l;
			
			break;

		case TEXT:
			break;

		case FONT:
			break;

		case XOBJECT:
			break;
		}

		buff[0] = CLO;
		buff[1] = CLO;
		buff[2] = CR;
		buff[3] = LF;
		f.write(buff, 4);
		length += 4;

		if(obj->stream != NULL) {
			memcpy(buff, "stream\r\n", 8);
			f.write(buff, 8);
			length += 8;

			//Stream content

			memcpy(buff, "endstream\r\n", 11);
			f.write(buff, 11);
			length += 11;
		}

		//write endobj
		memcpy(buff, "endobj\r\n", 8);
		f.write(buff, 8);
		length += 8;
	}

	//xref
	memcpy(buff, "xref", 4);
	buff[4] = CR;
	buff[5] = LF;
	f.write(buff, 6);
	length += 6;
	xref_addr = length; //Save the offset of XRef table.
	
	for (i = 0; i<pdf.rTableSize; i++) {
		xref = &pdf.rTable[i];
		l = sprintf(buff, "%d %d\r\n", xref->startIdx, xref->count);
		f.write(buff, l);
		length += l;

		for(k=xref->startIdx; k<(xref->count + xref->startIdx); k++) {
			obj = pdf.list_objects[k];
			isUsed = (obj->flags & ISUSED) == ISUSED ? 'f' : 'n';
			sprintf(buff, "0000000000 00000 %c\r\n", isUsed);
			tmp = sprintf(buff2, "%d", obj->offset);
			memcpy(buff + (10-tmp), buff2, tmp);
			tmp = sprintf(buff2, "%d", obj->gen_num);
			memcpy(buff + (16-tmp), buff2, tmp);
			f.write(buff, 20);
			length += 20;
		}
	}

	//write trailer
	memcpy(buff, "trailer", 7);
	buff[7] = CR;
	buff[8] = LF;
	f.write(buff, 9);
	length += 9;

	buff[0] = OPN;
	buff[1] = OPN;
	f.write(buff, 2);
	length += 2;

	//Content of trailer
	//if (pdf.listSize > 0) {
		l = sprintf(buff, "/Size %d ", pdf.listSize);
		f.write(buff, l);
		length += l;
	//}

	//write root object
	l = sprintf(buff, "/Root %d %d R", pdf.list_objects[pdf.rootIdx]->index, pdf.list_objects[pdf.rootIdx]->gen_num);
	f.write(buff, l);
	length += l;

	buff[0] = CR;
	buff[1] = LF;
	buff[2] = CLO;
	buff[3] = CLO;
	buff[4] = CR;
	buff[5] = LF;
	f.write(buff, 6);
	length += 6;

	//startxref
	l = sprintf(buff, "startxref\r\n%ld\r\n%%%%EOF", xref_addr);
	f.write(buff, l);
	length += l;
	f.flush();
	f.close();
	return length;
}
