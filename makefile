CC=g++

CFLAGS=-lcurl `pkg-config --cflags --libs glib-2.0 gdk-pixbuf-2.0 libnotify`

OBJ=main.cpp

default: xkcd-notifier


xkcd-notifier: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o xkcd-notifier

clean:
	rm xkcd-notifier