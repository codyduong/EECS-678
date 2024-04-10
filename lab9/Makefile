FIRST_NAME=Cody
LAST_NAME=Duong
KUID=3050266
LAB=9
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

DELIVERABLES=VM_addr_map.c input desired
CMD=./VM_addr_map

all: VM_addr_map

VM_addr_map: VM_addr_map.c
	gcc -g -o $@ $< -lm

TEST_NUMS=1 2

# to test, run diffs of the output files with the desired output files
# the "desired" output is the left and your output is the right in the diff
test: output
	@(for test in $(TEST_NUMS); do echo test $${test} diff... ; diff desired/out$${test}.txt output/out$${test}.txt; done)

# create the 'output' directory, then
# generate the output file 'output/outX.txt' for each of the 'input/inpX.txt' input files
output: all
	@rm -rf output; mkdir output
	@(for test in $(TEST_NUMS); do ./VM_addr_map < input/inp$${test}.txt > output/out$${test}.txt; done)
	
tar: clean
#	create temp dir
	mkdir $(TAR_BASENAME)
#	copy the necessary files into the temp dir
	cp -r $(DELIVERABLES) Makefile $(TAR_BASENAME)
#	create the submission tar.gz
	tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#	remove the temp dir
	rm -rf $(TAR_BASENAME)

clean:
	rm -rf VM_addr_map $(TAR_BASENAME)* output

.PHONY: clean tar test
