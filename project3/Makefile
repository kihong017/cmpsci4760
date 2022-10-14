CC		      = gcc
CFLAGS		  = -g -Wall
MASTER_TARGET = matser
MASTER_OBJ    = matser.o
MASTER_C      = matser.c
SLACE_TARGET  = slvae
SLAVE_OBJ     = slvae.o
SLAVE_C       = slvae.c


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
