CC		     = gcc
CFLAGS		 = -g -Wall
TARGET       = logParse
LOGPARSE_C	 = logParse.c
LOGPARSE_OBJ = logParse.o

.SUFFIXES: .c .o

$(LOGPARSE_OBJ): $(LOGPARSE_C)
	$(CC) $(CFLAGS) $(LOGPARSE_C) -o $(TARGET)

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -rf $(TARGET) *.o *~
