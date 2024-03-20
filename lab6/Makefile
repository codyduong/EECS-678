# Add the dp_asymmetric and dp_waiter targets to all as you implement
# them

STUDENT_ID=3050266

SRCDIR = ./
CFILELIST = dining_philosophers.c dp_asymmetric.c dp_waiter.c

RAWC = $(patsubst %.c,%,$(addprefix $(SRCDIR), $(CFILELIST)))

all: dp # dp_asymmetric dp_waiter

dp: dining_philosophers.c
	gcc -g dining_philosophers.c -lpthread -lm -o dp

dp_asymmetric: dp_asymmetric.c
	gcc -g dp_asymmetric.c -lpthread -lm -o dp_asymmetric

dp_waiter: dp_waiter.c
	gcc -g dp_waiter.c -lpthread -lm -o dp_waiter

# Add the dp_asymmetric_test and dp_waiter_test targets to test as you implement
# them

test: dp_test dp_asymmetric_test #dp_waiter_test

dp_test: dp
	./dp

dp_asymmetric_test: dp_asymmetric
	./dp_asymmetric

dp_waiter_test: dp_waiter
	./dp_waiter

clean:
	rm -f dp dp_asymmetric dp_waiter
	rm -rf *-c.txt $(STUDENT_ID)-pthreads_dp-lab

zip: 
	make clean
	mkdir $(STUDENT_ID)-pthreads_dp-lab
#	get all the c files to be .txt for archiving	
	$(foreach file, $(RAWC), cp $(file).c $(file)-c.txt;)
#	copy files into temp folder
	cp Makefile dining_philosophers.c dp_asymmetric.c dp_waiter.c $(STUDENT_ID)-pthreads_dp-lab/
	mv *-c.txt $(STUDENT_ID)-pthreads_dp-lab/
	zip -r $(STUDENT_ID)-pthreads_dp-lab.zip $(STUDENT_ID)-pthreads_dp-lab
	rm -rf $(STUDENT_ID)-pthreads_dp-lab
