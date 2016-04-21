CC=g++
AR=ar
CFLAGS=-I./include
src=src/npdf.cpp

all: pdfgen.exe

npdf.o: npdf.h npdf.cpp
	$(CC) -c -g ./src/npdf.cpp $(CFLAGS)
	
libnpdf.a: npdf.o
	$(AR) rsc libnpdf.a npdf.o
	
pdfgen.exe: libnpdf.a pdfgen.cpp
	$(CC) -g src/pdfgen.cpp -L./ -lnpdf -o pdfgen.exe $(CFLAGS)