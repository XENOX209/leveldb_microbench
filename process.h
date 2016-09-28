
class Process{
  public:
    Process(void (*func)());
    int wait(void);
  private:
    int pid;
};
