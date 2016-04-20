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
	objects[0].elements = NULL;
	objects[0].eleCount = 0;
	
	objects[1].type = OUTLINES;
	objects[1].gen_num = 0;
	objects[1].index = 2;
	objects[1].flags = INDIRECT;
	objects[1].elements = NULL;
	objects[1].eleCount = 0;
	
	objects[2].type = PAGES;
	objects[2].gen_num = 0;
	objects[2].index = 3;
	objects[2].flags = INDIRECT;
	objects[2].elements = NULL;
	objects[2].eleCount = 0;
	
	objects[3].type = PAGE;
	objects[3].gen_num = 0;
	objects[3].index = 4;
	objects[3].flags = INDIRECT;
	objects[3].elements = NULL;
	objects[3].eleCount = 0;
	
	objects[4].type = STREAM;
	//objects[4].objType = XREF;
	objects[4].gen_num = 0;
	objects[4].index = 5;
	objects[4].flags = INDIRECT;
	objects[4].elements = NULL;
	objects[4].eleCount = 0;
	
	objects[5].type = TEXT;
	objects[5].gen_num = 0;
	objects[5].index = 6;
	objects[5].flags = INDIRECT;
	objects[5].elements = NULL;
	objects[5].eleCount = 0;
	
	//Create a font object
	objects[6].type = FONT;
	objects[6].gen_num = 0;
	objects[6].index = 7;
	objects[6].flags = INDIRECT;
	objects[6].elements = NULL;
	objects[6].eleCount = 0;
	Font *font = (Font*)malloc(sizeof(Font));
	memcpy(font->name, "Helvetica", 9);
	font->name[10] = 0;
	objects[6].data = font;
	
	objects[7].type = ARRAY;
	objects[7].gen_num = 0;
	objects[7].index = 8;
	objects[7].flags = INDIRECT;
	objects[7].elements = NULL;
	objects[7].eleCount = 0;
	array = (Array*)malloc(sizeof(Array));
	array->size = 2;
	array->elements = (Element*)malloc(sizeof(Element) * array->size);
	array->elements[0].type = NAME;
	memcpy(array->elements[0].value, "PDF\0", 4);
	
	array->elements[1].type = NAME;
	memcpy(array->elements[1].value, "Text\0", 5);
	objects[7].data = array;
	
	pdf1.objects = objects;
	
	//point trailer root to Catalog object_length
	pdf1.trailer.root = &pdf1.objects[0];
	
	//init Catalog
	catalog = (Catalog*)malloc(sizeof(Catalog));
	catalog->refCount = 2;
	catalog->refObjects = (PDFObject**)malloc(sizeof(PDFObject*) * catalog->refCount);
	catalog->refObjects[0] = &objects[1];
	catalog->refObjects[1] = &objects[2];
	objects[0].data = catalog;
	
	addXrefTable(pdf1);
	writeDPF2File("D:\\data\\testpdf.pdf", pdf1);
	
	//Release font object
	free(font);
	
	//Release Array object
	free(array->elements);
	free(array);
	
	//Release Catalog
	free(catalog->refObjects);
	free(catalog);
	
	release(pdf1);
	return 0;
}