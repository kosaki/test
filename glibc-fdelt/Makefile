TARGET = fdset
OBJ = $(TARGET).o
SRC = $(TARGET).c
CC = gcc
GLIBCROOT = /home/kosaki/local/glibc
CFLAGS = -g -Wall -O2 -D_FORTIFY_SOURCE=2 -I$(GLIBCROOT)/include  -D_STRICT_FD_SIZE_CHECK=1

LDFLAGS = -nostdlib -nostartfiles -static
GLIBCDIR = $(GLIBCROOT)/lib
STARTFILES = $(GLIBCDIR)/crt1.o $(GLIBCDIR)/crti.o `gcc --print-file-name=crtbegin.o`
ENDFILES = `gcc --print-file-name=crtend.o` $(GLIBCDIR)/crtn.o
LIBGROUP = -Wl,--start-group $(GLIBCDIR)/libc.a -lgcc -lgcc_eh -Wl,--end-group

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(STARTFILES) $^ $(LIBGROUP) $(ENDFILES) 
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f *.o *.~ $(TARGET)
