TARGET = ebizzy
OBJ = $(TARGET).o
SRC = $(TARGET).c
CC = gcc
CFLAGS = -g -Wall -Wshadow 
LDFLAGS = -nostdlib -nostartfiles -static -L $(HOME)/local/glibc/lib
GLIBCDIR = /home/mkosaki/local/glibc/lib
STARTFILES = $(GLIBCDIR)/crt1.o $(GLIBCDIR)/crti.o `gcc --print-file-name=crtbegin.o`
ENDFILES = `gcc --print-file-name=crtend.o` $(GLIBCDIR)/crtn.o
LIBGROUP = -Wl,--start-group $(GLIBCDIR)/libpthread.a $(GLIBCDIR)/libc.a -lgcc -lgcc_eh -Wl,--end-group

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(STARTFILES) $^ $(LIBGROUP) $(ENDFILES) 
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^
clean:
	rm -f *.o *.~ $(TARGET)

