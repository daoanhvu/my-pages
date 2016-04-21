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
//#include <npdf.h>
#include "../include/npdf.h"

using namespace std;

int main(int argc, char **args) {
	PDF pdf1;
	Catalog *catalog;
	Array *array;
	Page *page;
	init(pdf1, 1, 5);
	PDFObject *obj;
	PDFObject **list;
	
	pdf1.listLogSize = 8;
	list = (PDFObject**) malloc(sizeof(PDFObject*) * 8);
	pdf1.listSize = 8;
	
	list[0]->type = CATALOG;
	list[0]->gen_num = 0;
	list[0]->index = 1;
	list[0]->flags = ISUSED | INDIRECT;
	list[0]->elements = NULL;
	list[0]->eleCount = 0;
	
	list[1]->type = OUTLINES;
	list[1]->gen_num = 0;
	list[1]->index = 2;
	list[1]->flags = INDIRECT;
	list[1]->elements = NULL;
	list[1]->eleCount = 0;
	
	list[2]->type = PAGES;
	list[2]->gen_num = 0;
	list[2]->index = 3;
	list[2]->flags = INDIRECT;
	list[2]->elements = NULL;
	list[2]->eleCount = 0;
	
	list[3]->type = PAGE;
	list[3]->gen_num = 0;
	list[3]->index = 4;
	list[3]->flags = INDIRECT;
	list[3]->elements = NULL;
	list[3]->eleCount = 0;
	page = (Page*)malloc(sizeof(Page));
	page->mediaBox.llx = 0;
	page->mediaBox.lly = 0;
	page->mediaBox.urx = 288;
	page->mediaBox.ury = 500;
	page->objects = NULL;
	page->refCount = 0;
	list[3]->data = page;

	
	list[4]->type = STREAM;
	//list[4].objType = XREF;
	list[4]->gen_num = 0;
	list[4]->index = 5;
	list[4]->flags = INDIRECT;
	list[4]->elements = NULL;
	list[4]->eleCount = 0;
	
	list[5]->type = TEXT;
	list[5]->gen_num = 0;
	list[5]->index = 6;
	list[5]->flags = INDIRECT;
	list[5]->elements = NULL;
	list[5]->eleCount = 0;
	
	//Create a font object
	list[6]->type = FONT;
	list[6]->gen_num = 0;
	list[6]->index = 7;
	list[6]->flags = INDIRECT;
	list[6]->elements = NULL;
	list[6]->eleCount = 0;
	Font *font = (Font*)malloc(sizeof(Font));
	memcpy(font->name, "Helvetica", 9);
	font->name[10] = 0;
	list[6]->data = font;
	
	list[7]->type = ARRAY;
	list[7]->gen_num = 0;
	list[7]->index = 8;
	list[7]->flags = INDIRECT;
	list[7]->elements = NULL;
	list[7]->eleCount = 0;
	array = (Array*)malloc(sizeof(Array));
	array->size = 2;
	array->elements = (Element*)malloc(sizeof(Element) * array->size);
	array->elements[0].type = NAME;
	memcpy(array->elements[0].value, "PDF\0", 4);
	
	array->elements[1].type = NAME;
	memcpy(array->elements[1].value, "Text\0", 5);
	list[7]->data = array;
	
	pdf1.list_objects = list;
	
	//point trailer root to Catalog object_length
	pdf1.rootIdx = 0;
	
	//init Catalog
	catalog = (Catalog*)malloc(sizeof(Catalog));
	catalog->refCount = 2;
	catalog->refObjects = (PDFObject**)malloc(sizeof(PDFObject*) * catalog->refCount);
	catalog->refObjects[0] = list[1];
	catalog->refObjects[1] = list[2];
	list[0]->data = catalog;
	
	addXrefTable(pdf1);
	writeDPF2File("D:\\data\\testpdf.pdf", pdf1);
	
	//Release font object
	free(font);
	
	//Release page object
	free(page);

	//Release Array object
	free(array->elements);
	free(array);
	
	//Release Catalog
	free(catalog->refObjects);
	free(catalog);
	
	release(pdf1);
	return 0;
}
