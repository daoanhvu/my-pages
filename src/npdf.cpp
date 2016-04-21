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

void int2String(int n, char *str, int offset, int *l) {
	int i, j, l2;
	int tmpn;
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
	for (i = 0; (i<l2) && (n>9); i++) {
		r = str[i + offset];
		j = offset + (*l) - i - 1;
		str[i] = str[j];
		str[j] = r;
	}
}

void addXrefEntry(PDF *pdf, int index) {
	if (pdf->rTableLogSize <= pdf->rTableSize) {
		pdf->rTableLogSize += 4;
		pdf->rTable = (int*)realloc(pdf->rTable, sizeof(int)*pdf->rTableLogSize);
	}
	pdf->rTable[pdf->rTableSize] = index;
	pdf->rTableSize++;
}

void addRefSubobject(const PDFObject *obj, char *buff, int offs, int *len) {
	int p, l;
	switch (obj->type) {
	case OUTLINES:
		buff[offs] = '/';
		memcpy(buff + offs + 1, "Outlines", 8);
		p = offs + 9;
		buff[p++] = SP;

		break;

	case PAGES:
		buff[offs] = '/';
		memcpy(buff + offs + 1, "Pages", 5);
		p = offs + 6;
		buff[p++] = SP;
		break;
	}

	int2String(obj->index, buff, p, &l);
	p += l;
	buff[p] = SP;
	p++;
	int2String(obj->gen_num, buff, p, &l);
	p += l;
	buff[p++] = SP;
	buff[p++] = 'R';
	*len = p - offs;
}

/**
	return length of the page in byte
*/
int writePageContent(Page *page, std::ostream &out) {
	int l1, l = 0;
	int i;
	char buff[64];
	
	for(i=0; i<page->refCount; i++) {
		
	}
	
	//Write mediabox
	memcpy(buff, "/MediaBox [", 11);
	l = 11;
	int2String(page->mediaBox.llx, buff, l, &l1);
	l += l1;
	buff[l++] = SP;
	int2String(page->mediaBox.lly, buff, l, &l1);
	l += l1;
	buff[l++] = SP;
	int2String(page->mediaBox.urx, buff, l, &l1);
	l += l1;
	buff[l++] = SP;
	int2String(page->mediaBox.ury, buff, l, &l1);
	l += l1;
	buff[l++] = ']';
	buff[l++] = CR;
	buff[l++] = LF;
	out.write(buff, l);
	return l;
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
			int2String(obj->elements[i].iValue, buff, keylen + 2, &vlen);
			buff[keylen + vlen + 2] = CR;
			buff[keylen + vlen + 3] = LF;
			length += keylen + vlen + 4;
		} else if(obj->elements[i].type == 3) {
			
		}
	}
	return length;
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
	pdf.start_xref_obj = 0;
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
int addXrefTable(PDF &pdf) {
	PDFObject *obj;
	int i, count = 0;
	for (i = 0; i<pdf.listSize; i++) {
		obj = pdf.list_objects[i];
		//if this is an indirect object then we add it into xref table
		if ((obj->flags & INDIRECT) == INDIRECT) {
			//isUsed = ((obj->flags & ISUSED) == ISUSED)?'f':'n';
			addXrefEntry(&pdf, i);
			count++;
		}
	}
	return count;
}

int writeDPF2File(const char *filename, const PDF &pdf) {
	char buff[128];
	char isUsed;
	int i, j, l1, l, tmp, r;
	long xref_addr;
	long length = 0;
	PDFObject *obj;
	Catalog* catalog;
	Page *page;
	std::ofstream f(filename, std::ofstream::binary);

	//write file header
	memcpy(buff, "%PDF-", 5);
	buff[5] = pdf.major + 48;
	buff[6] = '.';
	buff[7] = pdf.minor + 48;
	buff[8] = CR;
	buff[9] = LF;
	f.write(buff, 10);

	length = 10;

	//write objects
	for (i = 0; i<pdf.listSize; i++) {
		obj = pdf.list_objects[i];

		obj->offset = length;

		l = 0;
		int2String(obj->index, buff, 0, &l1);
		l += l1;
		buff[l++] = SP;
		//write generation number
		int2String(obj->gen_num, buff, l, &l1);
		l += l1;
		buff[l++] = SP;
		buff[l++] = 'o';
		buff[l++] = 'b';
		buff[l++] = 'j';
		buff[l++] = CR;
		buff[l++] = LF;
		f.write(buff, l);

		length += l;

		buff[0] = OPN;
		buff[1] = OPN;
		f.write(buff, 2);
		length += 2;
		
		//Write object's name elements
		l = writeElement(obj, f);
		length += l;

		//object's content
		switch (obj->type) {
		case STREAM: //CROSS REFERENCE STREAM Object
			//write stream
			memcpy(buff, "stream", 6);
			buff[6] = CR;
			buff[7] = LF;
			f.write(buff, 8);
			length += 8;

			memcpy(buff, "endstream", 9);
			buff[9] = CR;
			buff[10] = LF;
			f.write(buff, 11);
			length += 11;
			break;

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
			buff[0] = '/';
			buff[1] = 'T';
			buff[2] = 'y';
			buff[3] = 'p';
			buff[4] = 'e';
			buff[5] = SP;
			buff[6] = '/';
			memcpy(buff + 7, "Pages", 5);
			buff[12] = CR;
			buff[13] = LF;
			f.write(buff, 14);
			length += 14;
			break;

		case PAGE:
			buff[0] = '/';
			buff[1] = 'T';
			buff[2] = 'y';
			buff[3] = 'p';
			buff[4] = 'e';
			buff[5] = SP;
			buff[6] = '/';
			memcpy(buff + 7, "Page", 4);
			buff[11] = CR;
			buff[12] = LF;
			f.write(buff, 13);
			length += 13;
			
			page = (Page*)obj->data;
			l = writePageContent(page, f);
			
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
		//write endobj
		memcpy(buff, "endobj", 6);
		buff[6] = CR;
		buff[7] = LF;
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
	
	int2String(pdf.start_xref_obj, buff, 0, &l);
	buff[l++] = SP;
	int2String(pdf.rTableSize, buff, l, &i);
	l = l + i;
	buff[l++] = CR;
	buff[l++] = LF;
	f.write(buff, l);
	length += l;

	for (i = 0; i<pdf.rTableSize; i++) {
		obj = pdf.list_objects[pdf.rTable[i]];
		isUsed = (obj->flags & ISUSED) == ISUSED ? 'f' : 'n';
		memset(buff, '0', 18);
		buff[10] = SP;
		buff[16] = SP;
		buff[17] = isUsed;
		buff[18] = CR;
		buff[19] = LF;
		tmp = obj->offset;
		j = 9;
		while (tmp > 0) {
			r = (tmp % 10) + 48;
			tmp = tmp / 10;
			buff[j] = r;
			j--;
		}

		j = 15;
		tmp = obj->gen_num;
		while (tmp > 0) {
			r = (tmp % 10) + 48;
			tmp = tmp / 10;
			buff[j] = r;
			j--;
		}

		f.write(buff, 20);
	}
	length += pdf.rTableSize * 20;

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
	if (pdf.listSize > 0) {
		buff[0] = '/';
		buff[1] = 'S';
		buff[2] = 'i';
		buff[3] = 'z';
		buff[4] = 'e';
		buff[5] = SP;
		int2String(pdf.listSize, buff, 6, &l);
		l += 6;
		buff[l++] = SP;
		f.write(buff, l);
		length += l;
	}

	//write root object
	buff[0] = '/';
	buff[1] = 'R';
	buff[2] = 'o';
	buff[3] = 'o';
	buff[4] = 't';
	buff[5] = SP;
	int2String(pdf.list_objects[pdf.rootIdx]->index, buff, 6, &l);
	l += 6;
	buff[l++] = SP;
	int2String(pdf.list_objects[pdf.rootIdx]->gen_num, buff, l, &l1);
	l += l1;
	buff[l++] = SP;
	buff[l++] = 'R';
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
	memcpy(buff, "startxref", 9);
	buff[9] = CR;
	buff[10] = LF;
	f.write(buff, 11);
	length += 11;

	//Write offset of cross reference stream
	long2String(xref_addr, buff, 0, &l);
	f.write(buff, l);
	length += l;

	//write end-of-file marker
	buff[0] = CR;
	buff[1] = LF;
	buff[2] = '%';
	buff[3] = '%';
	buff[4] = 'E';
	buff[5] = 'O';
	buff[6] = 'F';
	f.write(buff, 7);
	f.flush();
	length += 7;
	f.close();
	return length;
}
