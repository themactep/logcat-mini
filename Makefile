all: logcat

logcat: main.c
	$(CC) -o logcat main.c

install: logcat
	install -Dm755 logcat $(DESTDIR)/usr/bin/logcat