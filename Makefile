all: logcat

logcat: main.c
	$(CC) -o logcat main.c