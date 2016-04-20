/**
	http://www.tutorialspoint.com/makefile/makefile_directives.htm
	Build this demo:
		g++ -c npdf.cpp
		ar rcs libnpdf.a npdf.o
		g++ -g pdfgen.cpp -L./ -lnpdf -o pdfgen.exe
*/
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "npdf.h"

using namespace std;

int main(int argc, char **args) {
	PDF pdf1;
	Catalog *catalog;
	Array *array;
	init(pdf1, 1, 5);
	PDFObject *objects;
	
	objects = (PDFObject*) malloc(sizeof(PDFObject) * 8);
	pdf1.object_length = 8;
	pdf1.trailer.size = 8;
	
	objects[0].type = CATALOG;
	objects[0].gen_num = 0;
	objects[0].index = 1;
	objects[0].flags = ISUSED | INDIRECT;
	objects[0].nameElements = NULL;
	objects[0].nameSize = 0;
	
	objects[1].type = OUTLINES;
	objects[1].gen_num = 0;
	objects[1].index = 2;
	objects[1].flags = INDIRECT;
	objects[1].nameElements = NULL;
	objects[1].nameSize = 0;
	
	objects[2].type = PAGES;
	objects[2].gen_num = 0;
	objects[2].index = 3;
	objects[2].flags = INDIRECT;
	objects[2].nameElements = NULL;
	objects[2].nameSize = 0;
	
	objects[3].type = PAGE;
	objects[3].gen_num = 0;
	objects[3].index = 4;
	objects[3].flags = INDIRECT;
	objects[3].nameElements = NULL;
	objects[3].nameSize = 0;
	
	objects[4].type = STREAM;
	//objects[4].objType = XREF;
	objects[4].gen_num = 0;
	objects[4].index = 5;
	objects[4].flags = INDIRECT;
	objects[4].nameElements = NULL;
	objects[4].nameSize = 0;
	
	objects[5].type = TEXT;
	objects[5].gen_num = 0;
	objects[5].index = 6;
	objects[5].flags = INDIRECT;
	objects[5].nameElements = NULL;
	objects[5].nameSize = 0;
	
	//Create a font object
	objects[6].type = FONT;
	objects[6].gen_num = 0;
	objects[6].index = 7;
	objects[6].flags = INDIRECT;
	objects[6].nameElements = NULL;
	objects[6].nameSize = 0;
	Font *font = (Font*)malloc(sizeof(Font));
	memcpy(font->name, "Helvetica", 9);
	font->name[10] = 0;
	objects[6].data = font;
	
	objects[7].type = ARRAY;
	objects[7].gen_num = 0;
	objects[7].index = 8;
	objects[7].flags = INDIRECT;
	objects[7].nameElements = NULL;
	objects[7].nameSize = 0;
	array = (Array*)malloc(sizeof(Array));
	array->size = 2;
	array->elements = (Element*)malloc(sizeof(Element) * array->size);
	array->elements[0].type = NAME;
	array->elements[0].data = (char*)malloc(4);
	memcpy(array->elements[0].data, "PDF", 3);
	
	array->elements[1].type = NAME;
	array->elements[1].data = (char*)malloc(4);
	memcpy(array->elements[1].data, "Text", 4);
	objects[7].data = array;
	
	pdf1.objects = objects;
	
	//point trailer root to Catalog object_length
	pdf1.trailer.root = &pdf1.objects[0];
	pdf1.trailer.size = 7;
	
	//init Catalog
	catalog = (Catalog*)malloc(sizeof(Catalog));
	catalog->refCount = 2;
	catalog->refObjects = (PDFObject**)malloc(sizeof(PDFObject*) * catalog->refCount);
	catalog->refObjects[0] = &objects[1];
	catalog->refObjects[1] = &objects[2];
	objects[0].data = catalog;
	
	addXrefTable(pdf1);
	writeDPF2File("D:\\data\\testpdf.pdf", pdf1);
	
	//Release Catalog
	free(catalog->refObjects);
	free(catalog);
	
	release(pdf1);
	return 0;
}