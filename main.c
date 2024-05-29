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

  int follow = 0;
  int fd;
  int msize;
  int readsize;
  int numread;
  int timestamps=0;

  if (argc > 1) {
    for (int x = 1; x < argc; x++) {
      if (strcmp(argv[x], "-c") == 0) {
        fd = open("/dev/log_main", O_WRONLY);
        if (fd < 0) {
          perror("Error opening /dev/log_main\n");
          return -1;
        }
        ioctl(fd, LOGGER_FLUSH_LOG);
        close(fd);
        return 0;
      } else if (strncmp(argv[x], "-f", 2) == 0) {
        follow = 1;
      } else if (strncmp(argv[x], "-t", 2) == 0) {
        timestamps = 1;
      } else {
        printf("Usage: %s [-c] [-h]\n", argv[0]);
        printf("Options:\n");
        printf("  -f  Follow the log\n");
        printf("  -c  Clear the log\n");
        printf("  -h  Show this help\n");
        printf("  -t  include timestamps\n");
        return 0;
      }
    }
  }

  if (follow) {
    fd = open("/dev/log_main", O_RDONLY);
    if (fd < 0) {
      perror("Error opening /dev/log_main\n");
      return -1;
    }
  } else {
    fd = open("/dev/log_main", O_RDONLY | O_NONBLOCK);
  }
  if (fd < 0) {
    perror("Error opening /dev/log_main\n");
    return -1;
  }
  int pos;
  while (1) {
    numread = read(fd, &entry, LOGGER_ENTRY_MAX_LEN);
    if (numread < 1) {
      if ((errno == EAGAIN || errno == EWOULDBLOCK) && !follow) {
        return 0;
      } else {
        printf("numread: %d\n", numread);
        printf("Errno %d", errno);
        perror("Error: \n");
        strerror(errno);
        return -1;
      }
      if (!follow && numread == 0) {
        return 0;
      }
    }
    memset((char *)&entry + numread, 0, 1);
    char *tag = entry.msg;
    char *prog = entry.msg + 1;
    char *msg = entry.msg + strlen(entry.msg) + 1;
    char tagbyte;
    switch (*tag) {
    case ANDROID_LOG_UNKNOWN:
      tagbyte = 'U';
      break;
    case ANDROID_LOG_DEFAULT:
      tagbyte = '*';
      break;
    case ANDROID_LOG_VERBOSE:
      tagbyte = 'V';
      break;
    case ANDROID_LOG_DEBUG:
      tagbyte = 'D';
      break;
    case ANDROID_LOG_INFO:
      tagbyte = 'I';
      break;
    case ANDROID_LOG_WARN:
      tagbyte = 'W';
      break;
    case ANDROID_LOG_ERROR:
      tagbyte = 'E';
      break;
    case ANDROID_LOG_FATAL:
      tagbyte = 'F';
      break;
    case ANDROID_LOG_SILENT:
      tagbyte = 'S';
      break;
    default:
      tagbyte = '?';
      break;
    }
    if (timestamps) {
      printf("%d.%d ", entry.sec, entry.nsec);
    }
    printf("%c/%s(%5d): %s", tagbyte, prog, entry.pid, msg);
  }
}
