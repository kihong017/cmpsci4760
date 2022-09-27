CC		     = gcc
CFLAGS		 = -g -Wall
TARGET       = master
OBJECTS      = master.o slave.o
C_PRGMS      = master.c slave.c

.SUFFIXES: .c .o

$(OBJECTS): $(C_PRGMS)
	$(CC) $(CFLAGS) $(C_PRGMS) -o $(TARGET)

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -rf $(TARGET) *.o *~
