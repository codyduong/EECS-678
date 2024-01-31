STUDENT_ID=XXXXXXX

SRCDIR = ./
CFILELIST = signals.c

RAWC = $(patsubst %.c,%,$(addprefix $(SRCDIR), $(CFILELIST)))

all: signals

signals: signals.c
	gcc -g -Wall $^ -o $@

clean:
	rm -f *.o signals
	rm -rf $(STUDENT_ID)-signals-lab

tar: clean
#	create temp dir
	mkdir $(STUDENT_ID)-signals-lab
#	get all the c files to be .txt for archiving
	$(foreach file, $(RAWC), cp $(file).c $(file)-c.txt;)
#	move the necessary files into the temp dir
	cp signals.c Makefile $(STUDENT_ID)-signals-lab/
	mv *-c.txt $(STUDENT_ID)-signals-lab/
	tar -czvf $(STUDENT_ID)-signals-lab.tar.gz $(STUDENT_ID)-signals-lab
	rm -rf $(STUDENT_ID)-signals-lab

.PHONY: clean zip
