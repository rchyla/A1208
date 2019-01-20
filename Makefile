CC=vc
RMRF=del

CFLAGS=
LDFLAGS=-lamiga

OBJECTS=  cfgreg.o addmem.o main.o
ADDHEADERS=config.h hardware.h

EXECUTABLE=A1208cfg

all: $(EXECUTABLE) 

A1208cfg : $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(LDFLAGS) $(OBJECTS)

%.o : %.c %.h $(ADDHEADERS)
	$(CC) $(CFLAGS) -c $<

clean :
	$(RMRF) $(OBJECTS) $(EXECUTABLE)

