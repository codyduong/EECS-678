STUDENT_ID=3050266

CC=gcc
TARGET=out1
OBJ=libpriqueue.o

all: $(TARGET)

$(TARGET): queuetest.c $(OBJ)
	$(CC) queuetest.c $(OBJ) -o $(TARGET)

$(OBJ): libpriqueue.c
	$(CC) -c libpriqueue.c -o $(OBJ)

test: out1_test

out1_test: out1
	./out1

clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf *-c.txt $(STUDENT_ID)-libpriqueue

SRCDIR = ./
CFILELIST = libpriqueue.c queuetest.c

RAWC = $(patsubst %.c,%,$(addprefix $(SRCDIR), $(CFILELIST)))

zip: 
	make clean
	mkdir $(STUDENT_ID)-libpriqueue
#	get all the c files to be .txt for archiving	
	$(foreach file, $(RAWC), cp $(file).c $(file)-c.txt;)
#	copy files into temp folder
	cp Makefile libpriqueue.c libpriqueue.h queuetest.c $(STUDENT_ID)-libpriqueue/
	mv *-c.txt $(STUDENT_ID)-libpriqueue/
	zip -r $(STUDENT_ID)-libpriqueue.zip $(STUDENT_ID)-libpriqueue
	rm -rf $(STUDENT_ID)-libpriqueue