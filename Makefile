CC = gcc
LIBS = `pkg-config --libs dbus-1` -L/usr/lib
INCS = -Isrc/ `pkg-config --cflags dbus-1` -I/usr/include/
INSTALL = install

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MKDIR = mkdir -p

CFLAGS = -std=gnu99

QUIET = @
DEBUG = -g

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = sind

all: $(TARGET)

$(TARGET): $(OBJ)
	$(QUIET) $(CC) $(DEBUG) -o $@ $^ $(LIBS)

%.o: %.c
	$(QUIET) $(CC) $(DEBUG) -c $(INCS) $(CFLAGS) -o $@ $<

clean:
	@ rm -f $(OBJ)
	@ rm -f $(TARGET)

install: all
	$(MKDIR) '$(DESTDIR)/$(BINDIR)'
	$(INSTALL) $(TARGET) '$(DESTDIR)/$(BINDIR)'
