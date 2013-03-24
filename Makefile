CC = gcc
LIBS = `pkg-config --libs dbus-1` -L/usr/lib
INCS = -Isrc/ `pkg-config --cflags dbus-1` -I/usr/include/
INSTALL = install

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

CFLAGS = -std=gnu99

QUIET = @
DEBUG = -g

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = sind

$(TARGET): $(OBJ)
	$(QUIET) $(CC) $(DEBUG) -o $@ $^ $(LIBS)

%.o: %.c
	$(QUIET) $(CC) $(DEBUG) -c $(INCS) $(CFLAGS) -o $@ $<

clean:
	@ rm -f $(OBJ)
	@ rm -f $(TARGET)

install:
	$(INSTALL) $(TARGET) '$(DESTDIR)/$(BINDIR)'
