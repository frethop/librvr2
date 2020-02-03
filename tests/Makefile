CC=gcc
CFLAGS=-I. -I.. -I../src
LDFLAGS=-L.. -lrvr -lpthread
DEPS = ../librvr.a ../src/system.h ../src/power.h ../src/drive.h ../src/api.h \
       ../src/io.h ../src/connection.h ../src/sensor.h
EXE = first powertest1 powertest2 powertest3 \
      drivetest1 drivetest2 drivetest3 drivetest4 \
      system1 leds leds2 blink \
      sensortest1 sensortest2 sensortest3 sensortest4 sensortest5\
      conntest \
	  temperature1 \
	  colortest1 colortest2 \
	  project1 project2 zeemote

all: $(EXE)

%o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm *.o $(EXE)

first: first.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

powertest1: powertest1.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

powertest2: powertest2.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

powertest3: powertest3.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

drivetest1: drivetest1.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

drivetest2: drivetest2.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

drivetest3: drivetest3.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

drivetest4: drivetest4.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

system1: system1.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

leds: leds.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

leds2: leds2.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

blink: blink.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

sensortest1: sensortest1.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

sensortest2: sensortest2.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

sensortest3: sensortest3.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

sensortest4: sensortest4.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

sensortest5: sensortest5.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

conntest: conntest.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

temperature1: temperature1.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

colortest1: colortest1.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

colortest2: colortest2.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm

project1: project1.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm

project2: project2.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm

zeemote: zeemote.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm