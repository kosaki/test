TARGETS = sbrk_test parent

CFLAGS = -Wall

all: $(TARGETS)

parent: parent.c
	gcc -o $@  $< $(CFLAGS) -m32

clean:
	-rm $(TARGETS)
