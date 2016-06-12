CC=g++
AR=ar
CFLAGS=-I./include
SRC=src/npdf.cpp src/pdf.cpp src/common.cpp src/lexer.cpp src/main.cpp

all: npdftest.exe

npdf.o: npdf.h npdf.cpp
	$(CC) -c -g $(SRC) $(CFLAGS)
#	$(CC) -c -g ./src/npdf.cpp $(CFLAGS)
	
libnpdf.a: npdf.o
	$(AR) rsc libnpdf.a npdf.o
	
npdftest.exe: libnpdf.a main.cpp
	$(CC) -g src/pdfgen.cpp -L./ -lnpdf -o pdfgen.exe $(CFLAGS)