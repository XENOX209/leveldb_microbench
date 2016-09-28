#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "process.h"
//class Process {
//public:
    Process::Process(void (*func)()) {
    pid = fork();
    if (pid == 0) {
      (*func)();
      exit(0);
    }
    else if (pid > 0) {
      return;
    }else {
      throw "Fail to create child process";
    }
  }
  int Process::wait(void) {
    return waitpid(pid,NULL,0);
  }
//private:
//  int pid;
//};
