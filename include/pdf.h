/**
http://www.adobe.com/content/dam/Adobe/en/devnet/acrobat/pdfs/PDF32000_2008.pdf
*/
#ifndef __PDF_H_
#define __PDF_H_

#include "npdf.h"

class PDF {
	private:
		float marginLeft;
		float marginTop;
		float marginRight;
		float marginBottom;

		char major;
		char minor;

		PDFObject **list_objects;
		int listLogSize;
		int listSize;

		PDFObject *root;
		Page **pages;
		int pageLogCount;
		int pageCount;

		//This array stored index of indirect objects in objects
		XRefElement *rTable;
		int rTableLogSize;
		int rTableSize;

	public:
		PDF();
		~PDF();
		PDF(char majorV, char minorV);
		void initBlankPDF();
		void addPage();
		void readFromFile(const char *filename);
};

#endif
