//#include <pdf.h>
#include "../include/pdf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>


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

void addPage() {

}

void readFromFile(const char *filename) {

}
