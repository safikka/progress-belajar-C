TARGET = tes
LIBGTK = `pkg-config --cflags --libs gtk+-3.0 glib-2.0`
CC = gcc
PTHREAD = -lpthread

all:
	$(CC) main.c $(PTHREAD) $(LIBGTK) -o $(TARGET)