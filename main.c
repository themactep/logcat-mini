#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define LOGGER_ENTRY_MAX_LEN (4 * 1024)
#define LOGGER_FLUSH_LOG _IO(__LOGGERIO, 4)
#define __LOGGERIO 0xAE

struct logger_entry {
  uint16_t len;   /* length of the payload */
  uint16_t __pad; /* no matter what, we get 2 bytes of padding */
  int32_t pid;    /* generating process's pid */
  int32_t tid;    /* generating process's tid */
  int32_t sec;    /* seconds since Epoch */
  int32_t nsec;   /* nanoseconds */
  char msg[4096]; /* the entry's payload */
} entry;

typedef enum android_LogPriority {
  ANDROID_LOG_UNKNOWN = 0,
  ANDROID_LOG_DEFAULT, /* only for SetMinPriority() */
  ANDROID_LOG_VERBOSE,
  ANDROID_LOG_DEBUG,
  ANDROID_LOG_INFO,
  ANDROID_LOG_WARN,
  ANDROID_LOG_ERROR,
  ANDROID_LOG_FATAL,
  ANDROID_LOG_SILENT, /* only for SetMinPriority(); must be last */
} android_LogPriority;

int main(int argc, char *argv[]) {

  int fd;
  int msize;
  int readsize;
  int numread;

  if (argc > 1) {
    if (strcmp(argv[1], "-c") == 0) {
      fd = open("/dev/log_main", O_WRONLY);
      if (fd < 0) {
        perror("Error opening /dev/log_main\n");
        return -1;
      }
      ioctl(fd, LOGGER_FLUSH_LOG);
      close(fd);
      return 0;
    } else {
      printf("Usage: %s [-c] [-h]\n", argv[0]);
      printf("Options:\n");
      printf("  -c  Clear the log\n");
      printf("  -h  Show this help\n");
      return 0;
    }
  }

  fd = open("/dev/log_main", O_RDONLY);
  if (fd < 0) {
    perror("Error opening /dev/log_main\n");
    return -1;
  }
  int pos;
  while (1) {
    numread = read(fd, &entry, LOGGER_ENTRY_MAX_LEN);
    if (numread < 1) {
      perror("Error: ");
      strerror(errno);
      return -1;
    }
    memset((char *)&entry + numread, 0, 1);
    char *tag = entry.msg;
    char *prog = entry.msg + 1;
    char *msg = entry.msg + strlen(entry.msg) + 1;
    switch (*tag) {
    case ANDROID_LOG_UNKNOWN:
      *tag = 'U';
      break;
    case ANDROID_LOG_DEFAULT:
      *tag = '*';
      break;
    case ANDROID_LOG_VERBOSE:
      *tag = 'V';
      break;
    case ANDROID_LOG_DEBUG:
      *tag = 'D';
      break;
    case ANDROID_LOG_INFO:
      *tag = 'I';
      break;
    case ANDROID_LOG_WARN:
      *tag = 'W';
      break;
    case ANDROID_LOG_ERROR:
      *tag = 'E';
      break;
    case ANDROID_LOG_FATAL:
      *tag = 'F';
      break;
    case ANDROID_LOG_SILENT:
      *tag = 'S';
      break;
    default:
      *tag = '?';
      break;
    }

    printf("%c/%s(%5d): %s", tag, prog, entry.pid, msg);
  }
}
