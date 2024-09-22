#include <string.h>
#include "../tests/harness/unity.h"
#include "../src/lab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/getopt_core.h>
#include <readline/readline.h>
#include <readline/history.h>

// #define lab_VERSION_MAJOR 1
// #define lab_VERSION_MINOR 0
// #define UNUSED(x) (void)x;

  // struct shell
  // {
  //   int shell_is_interactive;
  //   pid_t shell_pgid;
  //   struct termios shell_tmodes;
  //   int shell_terminal;
  //   char *prompt;
  // };

  /**
   * @brief Trim the whitespace from the start and end of a string.
   * For example "   ls -a   " becomes "ls -a". This function modifies
   * the argument line so that all printable chars are moved to the
   * front of the string
   *
   * @param line The line to trim
   * @return The new line with no whitespace
   */
  char *trim_white(char *line){
    int front_index = 0;
    int back_index = 0;
    //finding leading whitespace
    for (int i = 0; i < (int)strlen(line); i++)
    {
      if (!isspace((unsigned char)line[i])){
        front_index = i;
        break;
      }
      //if there were all blanks
      if (i == ((int)strlen(line) - 1)){
        front_index = i;
      }

    }
    //finding trailing whitespace but ignoring null character
    for (int i = (int)strlen(line) - 1; i >= 0; i--)
    {
      //accounting for null terminator
      if (!isspace((unsigned char)line[i])){
        back_index = i;
        break;
      }

    }
    //if there were no spaces, just return line
    if (front_index == 0 && back_index == (int)strlen(line)-1){
      return line;
    }

    //if all spaces
    if(front_index == ((int)strlen(line)-1)){
      char *new_line =(char *)malloc(1);
      if(new_line == NULL){
      fprintf(stderr,"failed to allocate memory for new_line");
      abort();
      }
      new_line[0] = '\0';
      return new_line;
    }

    //getting the new line's shortened length 
    size_t new_line_length = back_index - front_index + 1;
    //creating new line
    char *new_line = (char *)malloc((new_line_length + 1)*sizeof(char));
    if(new_line == NULL){
      fprintf(stderr,"failed to allocate memory for new_line");
      abort();
    }
    int j = 0;
    for(int i = front_index; i <= back_index; i++){
      new_line[j] =line[i]; 
      j++;
    }
    new_line[j] = '\0';

    return new_line;
  }

  void getInput(struct shell *sh) {
    
    // set stuff up here
    printf("In set up\n");
    
    //STARTING PROGRAM
    char *line = (char *)malloc(256 * sizeof(char)); //allocating memory for a whole line of input
    if(line == NULL){
        fprintf(stderr,"failed to allocate memory for line");
        abort();
    }

    //accessing user input
    using_history();

    //changing prompt to be prompt set in environment
    while ((line=readline(sh->prompt))){
      printf("%s\n",line);
      add_history(line);
      char *new_line = trim_white(line);
      char **strings = cmd_parse(trim_white(new_line));
      cmd_free(strings);
    }
    
  }

// void tearDown(void) {
//   // clean stuff up here
// }

/**
    * @brief Set the shell prompt. This function will attempt to load a prompt
    * from the requested environment variable, if the environment variable is
    * not set a default prompt of "shell>" is returned.  This function calls
    * malloc internally and the caller must free the resulting string.
    *
    * @param env The environment variable
    * @return const char* The prompt
*/
char *get_prompt(const char *env) {
  printf("in get prompt\n");
    //TODO add error handling, look at old labs
    const char *prompt = getenv(env);

    if(prompt != NULL){
      printf("in get prompt !=null\n");
      char *actual_prompt = (char *)malloc((strlen(prompt) + 1) * sizeof(char));
      if(actual_prompt == NULL){
        fprintf(stderr,"failed to allocate memory for actual_prompt");
        abort();
      }
      strcpy(actual_prompt, prompt);
      return actual_prompt;
    }

    char *actual_prompt = (char *)malloc(7 * sizeof(char));
    
    if(actual_prompt == NULL){
      fprintf(stderr,"failed to allocate memory for actual_prompt");
      abort();
    }
    strcpy(actual_prompt, "shell>");
    printf("in get promp did strcpy\n");
    return actual_prompt;

}


  // /**
  //  * Changes the current working directory of the shell. Uses the linux system
  //  * call chdir. With no arguments the users home directory is used as the
  //  * directory to change to.
  //  *
  //  * @param dir The directory to change to
  //  * @return  On success, zero is returned.  On error, -1 is returned, and
  //  * errno is set to indicate the error.
  //  */
  // int change_dir(char **dir){
  //   //TODO
  //   return 2;
  // }

  /**
   * @brief Convert line read from the user into to format that will work with
   * execvp. We limit the number of arguments to ARG_MAX loaded from sysconf.
   * This function allocates memory that must be reclaimed with the cmd_free
   * function.
   *
   * @param line The line to process
   *
   * @return The line read in a format suitable for exec
   */
  char **cmd_parse(char const *line){
    //creating array of strings that are arguments
    char **strings = (char **)malloc(_SC_ARG_MAX * sizeof(char));
      if(strings == NULL){
      fprintf(stderr,"failed to allocate memory for parsed string array");
      abort();
      }
    
    //allocating string sizes and transferring strings to strings array
    // i is going to be the string index for the new strings array
    // k is going to be the length of each parsed string
    // j is going to keep track of index of original line
    int j = 0;
    int k = 0;
    for (int i = 0; i < _SC_ARG_MAX; i++) {

      //new string so start counting again
      while (!isspace((unsigned char)line[j])){
        j++;
        k++;
      }
      //set the size of that string plus null pointer
      strings[i] = (char *)malloc((k + 1) * sizeof(char));

      //going back in index to set string contents
      j -= k;

      //transfering strings
      for (int l = 0; l < k ; l++){
        strings[i][l] = line[j];
        j++;
      }

      //adding null character to new string
      strings[i][k] = '/0';

      //reset k to use again for next string to parse
      k = 0;
        
      //if we have reached the end of the line
      if(&line[i] == '/0'){
        i = _SC_ARG_MAX;
      }
      
    }

    // //transfering strings
    // int j = 0;
    // for (int i = 0; i < (int)strlen(line); i++) {
    //   //new string so start counting again
    //   j = 0;
    //   while (strlen(strings[i])> j){
    //     strings[i][j]= line[i];
    //   }
    //   //set the size of that string plus null pointer
    //   strings[i] = (char *)malloc((j + 1) * sizeof(char));
    // }

    return strings;
  }

  /**
   * @brief Free the line that was constructed with parse_cmd
   *
   * @param line the line to free
   */
  void cmd_free(char ** line){
    for(int i = 0; i < (int)strlen(line) ; i++ ){
      free(line[i]);
    }
  }

  


  // /**
  //  * @brief Takes an argument list and checks if the first argument is a
  //  * built in command such as exit, cd, jobs, etc. If the command is a
  //  * built in command this function will handle the command and then return
  //  * true. If the first argument is NOT a built in command this function will
  //  * return false.
  //  *
  //  * @param sh The shell
  //  * @param argv The command to check
  //  * @return True if the command was a built in command
  //  */
  // bool do_builtin(struct shell *sh, char **argv){
  //   //TODO
  //   return true;
  // }

  /**
   * @brief Initialize the shell for use. Allocate all data structures
   * Grab control of the terminal and put the shell in its own
   * process group. NOTE: This function will block until the shell is
   * in its own program group. Attaching a debugger will always cause
   * this function to fail because the debugger maintains control of
   * the subprocess it is debugging.
   *
   * @param sh
   */
  void sh_init(struct shell *sh){
    printf("in get sh_init\n");
    sh->shell_is_interactive = 1;
    sh->shell_pgid = 1;
    sh->shell_terminal = 1;
    //getting prompt from environment
    const char *name = "MY_PROMPT";
    char *env_prompt = get_prompt(name);
    sh->prompt = (char *)malloc((strlen(env_prompt)+1)*sizeof(char));
    if(sh->prompt == NULL){
      fprintf(stderr,"failed to allocate memory for shell struct's prompt");
      abort();
    }
    
    strcpy(sh->prompt, env_prompt);
    free(env_prompt);
    printf("in get promp did strcpy\n");

    getInput(sh);
  }

  // /**
  //  * @brief Destroy shell. Free any allocated memory and resources and exit
  //  * normally.
  //  *
  //  * @param sh
  //  */
  // void sh_destroy(struct shell *sh){
    //free(prompt);
  //   //TODO
  // }

  /**
   * @brief Parse command line args from the user when the shell was launched
   *
   * @param argc Number of args
   * @param argv The arg array
   */
  void parse_args(int argc, char **argv){
    printf("in parse function\n");
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
    return 0;

  }
