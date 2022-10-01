CC		      = gcc
CFLAGS		  = -g -Wall
MASTER_TARGET = master
MASTER_OBJ    = master.o
MASTER_C      = master.c
SLACE_TARGET  = slave
SLAVE_OBJ     = slave.o
SLAVE_C       = slave.c


all: $(MASTER_TARGET) $(SLACE_TARGET)

$(MASTER_TARGET): $(MASTER_OBJ)
	$(CC) -o $(MASTER_TARGET) $(MASTER_OBJ)

$(SLACE_TARGET): $(SLAVE_OBJ)
	$(CC) -o $(SLACE_TARGET) $(SLAVE_OBJ)

$(MASTER_OBJ): $(MASTER_C)
	$(CC) -c $(MASTER_C)

$(SLAVE_OBJ): $(SLAVE_C)
	$(CC) -c $(SLAVE_C)

clean:
	rm -rf $(MASTER_TARGET) $(MASTER_OBJ) $(SLACE_TARGET) $(SLAVE_OBJ) cstest logfile logfile.* *.o *~
