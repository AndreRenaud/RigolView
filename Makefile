CFLAGS=-g -Wall -pipe
OBJECTS=device.o rigol_view.o
CFLAGS+=$(shell pkg-config --cflags sdl2)
LFLAGS+=$(shell pkg-config --libs sdl2)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

rigol_view: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LFLAGS)

clean:
	rm -f $(OBJECTS) rigol_view
