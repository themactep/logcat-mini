# Simple Makefile for logcat-mini
# Pure C implementation, no C++ required

TARGET = logcat
SOURCES = main.c

# Compiler flags for embedded systems
CFLAGS ?= -Os -ffunction-sections -fdata-sections -fomit-frame-pointer -flto
LDFLAGS ?= -Wl,--gc-sections -flto

# Build rules
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)
	$(STRIP) $(TARGET)

.PHONY: clean install
clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -D -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
