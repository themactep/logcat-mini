#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LOGGER_ENTRY_MAX_LEN (4 * 1024)

struct logger_entry {
  uint16_t len;    /* length of the payload */
  uint16_t __pad;  /* no matter what, we get 2 bytes of padding */
  int32_t pid;     /* generating process's pid */
  int32_t tid;     /* generating process's tid */
  int32_t sec;     /* seconds since Epoch */
  int32_t nsec;    /* nanoseconds */
  char msg[65535]; /* the entry's payload */
} entry;

int main(int argc, char *argv[]) {

  int fd;
  int msize;
  int readsize;
  int numread;

  fd = open("/dev/log_main", O_RDONLY);
  if (fd < 0) {
    printf("Error opening /dev/log_main\n");
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
    char *msg = entry.msg + strlen(entry.msg) + 1;
    time_t t = entry.sec;
    struct tm *tm = localtime(&t);
    char date[64];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm);

    printf("%s %d %d %s: %s", date, entry.pid, entry.tid, tag, msg);
  }
}
