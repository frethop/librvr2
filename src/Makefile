CC=gcc
CFLAGS=-I. 
AR=ar
DEPS = rvrio.h rvrcmd.h rvrdefs.h api.h power.h drive.h drive_helper.h system.h connection.h sensor.h bounds.h
OBJ = rvrio.o api.o power.o io.o drive.o drive_helper.o system.o connection.o sensor.o 

librvr.a: $(OBJ) 
	$(AR) rcs librvr.a $(OBJ)
	cp librvr.a ..

%o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm $(OBJ) ../librvr.a
