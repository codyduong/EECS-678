# Change this line before submission
KUID = 3050266
PROGNAME = simulator

CC = gcc --std=gnu11
CFLAGS = -Wall -g


####################################################################
#                           IMPORTANT                              #
####################################################################
# Add files to their respective line to get this makefile to build #
# them.                                                            #
####################################################################
# NOTE: The submission scripts assume all files in `CFILELIST` end with
# .c and all files in `HFILES` end in .h
CFILELIST = simulator.c libscheduler/libscheduler.c libpriqueue/libpriqueue.c
HFILELIST = libscheduler/libscheduler.h libpriqueue/libpriqueue.h

# Add libraries that need linked as needed (e.g. -lm -lpthread)
LIBLIST =

# Include locations
INCLIST = ./src ./src/libscheduler ./src/libpriqueue

# Doxygen configuration file
DOXYGENCONF = ./doc/Doxyfile

####################################################################

SRCDIR = ./src/
OBJDIR = ./obj/

EXECNAME = $(patsubst %,./%,$(PROGNAME))

CFILES = $(patsubst %,$(SRCDIR)%,$(CFILELIST))
HFILES = $(patsubst %,$(SRCDIR)%,$(HFILELIST))
OFILES = $(patsubst %.c,$(OBJDIR)%.o,$(CFILELIST))

RAWC = $(patsubst %.c,%,$(addprefix $(SRCDIR), $(CFILELIST)))
RAWH = $(patsubst %.h,%,$(addprefix $(SRCDIR), $(HFILELIST)))

INCDIRS = $(patsubst %,-I%,$(INCLIST))

SUBMISSION = $(KUID)-lab9-scheduler

OBJINNERDIRS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(shell find $(SRCDIR) -type d))
SUBMISSIONDIRS = $(addprefix $(SUBMISSION)/,$(shell find $(SRCDIR) -type d))

# Build the the quash executable
all: $(PROGNAME) queuetest

# Build the object directories
$(OBJINNERDIRS):
	$(foreach dir, $(OBJINNERDIRS), mkdir -p $(dir);)

# Build the program
$(PROGNAME): $(OBJINNERDIRS) $(PROGNAME)-inner
$(PROGNAME)-inner: $(OFILES)
	$(CC) $(CFLAGS) $^ -o $(PROGNAME) $(LIBLIST)


# Generic build target for all compilation units. NOTE: Changing a
# header requires you to rebuild the entire lab
$(OBJDIR)%.o: $(SRCDIR)%.c $(HFILES)
	$(CC) $(CFLAGS) -c $(INCDIRS) -o $@ $< $(LIBS)

# Build a testing harness for the priority queue
queuetest: $(OBJINNERDIRS) queuetest-inner
queuetest-inner: ./src/queuetest.c ./src/libpriqueue/libpriqueue.o
	$(CC) $(CFLAGS) $^ -o queuetest $(LIBLIST)

# Build and run the program
test: all
	./queuetest
	./examples.pl

# Build the documentation
doc: $(DOXYGENCONF) $(CFILES)
	doxygen $(DOXYGENCONF)

# Quickly build a submission then extract it. Useful for testing if
# we will be able to build your lab from the code you gave us
testsubmit: submit unsubmit

# Build a safeassign friendly submission
submit: clean
#	Perform renaming copies across the Makefile and all .c and .h
#	files
	cp Makefile Makefile.txt
	$(foreach file, $(RAWH), cp $(file).h $(file)-h.txt;)
	$(foreach file, $(RAWC), cp $(file).c $(file)-c.txt;)

#	Make a temporary directory
	mkdir -p $(SUBMISSION)
	$(foreach dir, $(SUBMISSIONDIRS), mkdir -p $(dir);)

#	Move all the renamed files into the temporary directory
	mv Makefile.txt $(SUBMISSION)
	$(foreach file, $(RAWH), mv $(file)-h.txt $(SUBMISSION)/$(file)-h.txt &&) \
	$(foreach file, $(RAWC), mv $(file)-c.txt $(SUBMISSION)/$(file)-c.txt &&) \
	zip -r $(SUBMISSION).zip $(SUBMISSION) # Create submission zip
	-rm -rf $(SUBMISSION) # Remove temporary directory

# We will use this command to extract your submission and build
# it. Check to see if the build works before submitting.
unsubmit: $(SUBMISSION).zip
	unzip $< && \
	cd $(SUBMISSION) && \
	mv Makefile.txt Makefile && \
	$(foreach file, $(RAWH), mv $(file)-h.txt $(file).h &&) \
	$(foreach file, $(RAWC), mv $(file)-c.txt $(file).c &&) \
	make $(PROGNAME)

# Remove all generated files and directories
clean:
	-rm -rf $(PROGNAME) queuetest obj *~ $(SUBMISSION)* doc/html

.PHONY: all test submit unsubmit testsubmit doc clean
