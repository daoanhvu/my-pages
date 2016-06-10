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

		PDFObject *pageParent;

		PDFObject *root;
		Page **pages;
		int pageLogCount;
		int pageCount;

		//This array stored index of indirect objects in objects
		XRefElement *rTable;
		int rTableLogSize;
		int rTableSize;

		long xref_addr;

		int addObject(PDFObject *);
		int addPage(Page *);
		int readContent(std::istream &in, const char *inbuff, int offs, int len);
		void lexicalAnalysis(const char *, int len);
		void parseObject();

	public:
		PDF();
		~PDF();
		PDF(char majorV, char minorV);
		void initBlankPDF();
		void addPage();
		int read(std::istream &in, char *msg);
		int write(std::ostream &out);
};

#endif
