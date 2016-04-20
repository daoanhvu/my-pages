CC=g++
AR=ar
CFLAGS=-I.
src=npdf.cpp

npdf.o: npdf.h npdf.cpp
	$(CC) -c -g npdf.cpp $(CFLAGS)
	
libnpdf.a: npdf.o
	$(AR) rsc libnpdf.a npdf.o
	
pdfgen.exe: libnpdf.a pdfgen.cpp
	$(CC) -g $(CFLAGS) pdfgen.cpp -L./ -lnpdf -o pdfgen.exe