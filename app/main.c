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

    //declare-initialize variables
    //flags
    int zflag = 0;
    int bflag = 0;
    int index;
    int c;
    //might need to add this back in, turns off error messages
    opterr = 0;
    char *cvalue = NULL;

    while ((c = getopt (argc, argv, "zbc:")) != -1)
      switch (c)
        {
        case 'z':
          zflag = 1;
          break;
        case 'b':
          bflag = 1;
          break;
        case 'c':
          //arg parameter for c
          cvalue = optarg;
          break;
        case '?':
          if (optopt == 'c')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
            fprintf (stderr,
                    "Unknown option character `\\x%x'.\n",
                    optopt);
        default:
          abort ();
        }

    printf ("zflag = %d, bflag = %d, cvalue = %s\n",
            zflag, bflag, cvalue);

    if (zflag > 0){
      printf ("horray you picked z\n");
      fprintf(stdout, "version %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
      return 0;
    }

    for (index = optind; index < argc; index++){
      printf ("Non-option argument %s\n", argv[index]);
    }

  //STARTING PROGRAM
  struct shell *pShelly = (struct shell *)malloc(sizeof(struct shell));
  sh_init(pShelly);
  
  
  return 0;
}
