OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g -Wall $(OMPFLAGS) -std=c99
LDFLAGS = -g -Wall -std=c99
LDLIBS = $(OMPLIBS) -lm

all: sense_reversing dissemination

sense_reversing: sense_reversing.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

dissemination: dissemination.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
clean:
	rm -f *.o sense_reversing dissemination
