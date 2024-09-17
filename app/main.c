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
  //are you there world?
  printf("hello world\n");

  //PARSE ARGS
  parse_args(argc, **argv);
  
  //STARTING PROGRAM

  //getting prompt from environment
  const char *name = "MY_PROMPT";
  char *prompt = get_prompt(name);
  char *line = (char *)malloc(256 * sizeof(char)); //allocating memory for a whole line of input


  //accessing user input
  using_history();
  //changing prompt to be prompt set in environment
  while ((line=readline(prompt))){
    printf("%s\n",line);
    add_history(line);
    //setUp(line);  
    free(line);
  }
  

  free(name);
  free(prompt);
  return 0;
}
