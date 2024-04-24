STUDENT_ID=3050266

all:
	@echo "Please use an explicit target. We suggest test1"

dine: dine.c
	gcc -Wall -g -o dine dine.c -lpthread

procstat: procstat.c
	gcc -o procstat procstat.c

test1: dine
	./dine

test2: procstat
	./procstat $(PID)

clean:
	rm -f dine procstat
	rm -f *~

zip: 
	make clean
	mkdir $(STUDENT_ID)-procfs-lab
	cp Makefile dine.c $(STUDENT_ID)-procfs-lab/
	zip -r $(STUDENT_ID)-procfs-lab.zip $(STUDENT_ID)-procfs-lab
	rm -rf $(STUDENT_ID)-procfs-lab
