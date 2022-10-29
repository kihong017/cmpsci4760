CC		      = gcc
CFLAGS		  = -g -Wall
OSS_TARGET    = oss
OSS_OBJ    = oss.o
OSS_C      = oss.c
USER_PROCESS_TARGET  = userprocess
USER_PROCESS_OBJ     = userprocess.o
USER_PROCESS_C       = userprocess.c


all: $(OSS_TARGET) $(USER_PROCESS_TARGET)

$(OSS_TARGET): $(OSS_OBJ)
	$(CC) -o $(OSS_TARGET) $(OSS_OBJ)

$(USER_PROCESS_TARGET): $(USER_PROCESS_OBJ)
	$(CC) -o $(USER_PROCESS_TARGET) $(USER_PROCESS_OBJ)

$(OSS_OBJ): $(OSS_C)
	$(CC) -c $(OSS_C)

$(USER_PROCESS_OBJ): $(USER_PROCESS_C)
	$(CC) -c $(USER_PROCESS_C)

clean:
	rm -rf $(OSS_TARGET) $(OSS_OBJ) $(USER_PROCESS_TARGET) $(USER_PROCESS_OBJ) cstest logfile logfile.* *.o *~