#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/getopt_core.h>
#include "../src/lab.h"
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char **argv)
{
  parse_args(argc, argv);

  //STARTING PROGRAM
  struct shell *pShelly = (struct shell *)malloc(sizeof(struct shell));
  sh_init(pShelly);
  pid_t id = fork();
  //if is 0  then the parent process was able to create child process correctly
  
  
  return 0;
}
