#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/pomo.sock"

int main() {
  int rSocketFd, len, t;
  struct sockaddr_un remote;
  char statusMsg[100];

  if ((rSocketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, SOCK_PATH);
  len = strlen(remote.sun_path) + sizeof(remote.sun_family);
  if (connect(rSocketFd, (struct sockaddr *)&remote, len) == -1) {
    perror("connect");
    exit(1);
  }

  if ((t=recv(rSocketFd, statusMsg, 100, 0)) > 0) {
    statusMsg[t] = '\0';
    printf("%s\n", statusMsg);
  } else {
    if (t < 0) perror("recv");
    else printf("Server closed connection\n");
    exit(1);
  }

  close(rSocketFd);

  return 0;
}
