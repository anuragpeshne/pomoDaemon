#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <math.h>
#include <fcntl.h>

#define SOCK_PATH "/tmp/pomoSock"
#define TIMER (1 * 60)
#define TIMER_SPEED 1
#define PBAR_RESOLUTION 20
#define PSTR_SIZE (1 + 1 + 2 + PBAR_RESOLUTION + 1)

void tick(int);
void resetTimer();
void getPBarStr(char*);

volatile int elapsedTime = 0;

int main() {
  FILE *fp= NULL;
  struct sigaction sa, resetSa;
  int lSocketFd, rSocketFd, len;
  socklen_t t;
  struct sockaddr_un local, remote;
  char progressStr[PSTR_SIZE];

  pid_t process_id = 0;
  pid_t sid = 0;

  // Create child process
  process_id = fork();
  // Indication of fork() failure
  if (process_id < 0) {
    printf("fork failed!\n");
    // Return failure in exit status
    exit(1);
  }

  // PARENT PROCESS. Need to kill it.
  if (process_id > 0) {
    printf("process_id of child process %d \n", process_id);
    // return success in exit status
    exit(0);
  }

  //Child process proceeds ahead
  //unmask the file mode
  umask(0);

  //set new session
  sid = setsid();
  if(sid < 0) {
    // Return failure
    exit(1);
  }

  // Change the current working directory to root.
  chdir("/");
  // Close stdin. stdout and stderr
  close(STDIN_FILENO);
  //close(STDERR_FILENO);

  resetSa.sa_handler = resetTimer;
  resetSa.sa_flags = 0;
  sigemptyset(&resetSa.sa_mask);
  if (sigaction(SIGUSR1, &resetSa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  if ((lSocketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  //fcntl(lSocketFd, F_SETFL, O_NONBLOCK);
  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, SOCK_PATH);
  unlink(local.sun_path);
  len = strlen(local.sun_path) + sizeof(local.sun_family);
  if (bind(lSocketFd, (struct sockaddr *)&local, len) == -1) {
    perror("listen");
    exit(1);
  }
  t = sizeof(remote);

  while(1) {
    sleep(1 / TIMER_SPEED);
    tick(TIMER);

    if ((rSocketFd = accept(lSocketFd, (struct sockaddr *)&remote, &t)) != -1) {
      getPBarStr(progressStr);    //we are passing by referrence
      if (send(rSocketFd, progressStr, PSTR_SIZE, 0) < 0){
        perror("send");
      }

      close(rSocketFd);
    }
  }
  // you shall not pass - through this code path ever!
  printf("exiting");
  return 0;
}

void tick(int totalTime) {
  printf("tick");
  if (elapsedTime < totalTime) {
    elapsedTime++;
  }
}

void resetTimer() {
  elapsedTime = 0;
}

void getPBarStr(char* statusStr) {
  int coloredPieces, blankPieces;
  int i, strIndex;

  strIndex = 0;
  statusStr[strIndex++] = (char) elapsedTime;
  statusStr[strIndex++] = ' ';

  coloredPieces = (int)floor((PBAR_RESOLUTION * elapsedTime) / TIMER);
  blankPieces = PBAR_RESOLUTION - coloredPieces;

  statusStr[strIndex++] = '[';
  for (i = 0; i < PBAR_RESOLUTION; i++) {
    if (i < coloredPieces)
      statusStr[strIndex++] = '=';
    else
      statusStr[strIndex++] = ' ';
  }
  statusStr[strIndex++] = ']';
  statusStr[strIndex] = '\0';
}
