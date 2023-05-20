CC = gcc

CFLAGS = 

OBJS = main.o util.o symtab.o lex.yy.o parse.o


TARGET = hw2_binary

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lfl


tiny.exe: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lfl

main.o: main.c globals.h util.h scan.h parse.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

parse.o: parse.c parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.c

lex.yy.o: lex.yy.c util.h globals.h scan.h 
	$(CC) $(CFLAGS) -c lex.yy.c -lfl

lex.yy.c: lex/tiny.l
	flex lex/tiny.l

clean:
	-rm tiny.exe
	-rm tm.exe
	-rm main.o
	-rm util.o
	-rm parse.o

tm.exe: tm.c
	$(CC) $(CFLAGS) -etm tm.c

tiny: tiny.exe

tm: tm.exe

all: tiny tm

