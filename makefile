CC=g++

CFLAGS=-lcurl

OBJ=main.cpp

default: xkcd-notifier


xkcd-notifier: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o xkcd-notifier