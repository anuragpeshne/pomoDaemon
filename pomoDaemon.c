#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <math.h>

void startTimer(int);
void resetTimer();
void printElapsedTime(int);
void printProgressBar(int, int, int);

const int timer = 1 * 60;
const int timerSpeed = 1;
const int progressBarResolution = 20;

volatile int elapsedTime = 0;

int main() {
  FILE *fp= NULL;
  struct sigaction sa, resetSa;

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
  close(STDERR_FILENO);

  sa.sa_handler = printElapsedTime;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  //attach handlers
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  resetSa.sa_handler = resetTimer;
  resetSa.sa_flags = 0;
  sigemptyset(&resetSa.sa_mask);
  if (sigaction(SIGUSR2, &resetSa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
  
  liveForever();
  // you shall not pass - through this code path ever!
  return 0;
}

void liveForever() {
  while(1) {
    sleep(1);
    startTimer(timer);
  }
}

void startTimer(int totalTime) {
  while(elapsedTime < totalTime) {
    sleep(1 / timerSpeed);
    elapsedTime++;
  }
}

void resetTimer() {
  elapsedTime = 0;
  liveForever();
}

void printElapsedTime(int sig) {
  printf("%d => ", elapsedTime);
  printProgressBar(elapsedTime, timer, progressBarResolution);
  printf("\n");
  fflush(stdout);
}

void printProgressBar(int current, int total, int resolution) {
  int coloredPieces, blankPieces;
  int i;

  coloredPieces = (int)floor((resolution * current) / total);
  blankPieces = resolution - coloredPieces;

  printf("[");
  for (i = 0; i < resolution; i++) {
    if (i < coloredPieces)
      printf("=");
    else
      printf("_");
  }
  printf("]");
}
