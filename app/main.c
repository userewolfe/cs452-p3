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
  
  
  return 0;
}
