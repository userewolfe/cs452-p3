#include <string.h>
#include "../tests/harness/unity.h"
#include "../src/lab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <bits/getopt_core.h>
#include <readline/readline.h>
#include <readline/history.h>


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
      char *new_line =(char *)malloc((strlen(line) + 1));
      strcpy(new_line, line);
      return new_line;
    }

    //if all spaces
    if(front_index == ((int)strlen(line)-1)){
      char *new_line =(char *)malloc(1*sizeof(char));
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
    char *new_line = (char *)alloc((new_line_length + 1)*sizeof(char));
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
    char *line; 
    //(char *)malloc(sizeof(char) + 1); //allocating memory for a whole line of input
    // if(line == NULL){
    //     fprintf(stderr,"failed to allocate memory for line");
    //     abort();
    // }

    //accessing user input
    using_history();

    //changing prompt to be prompt set in environment
    while ((line=readline(sh->prompt) )){
      char *new_line = trim_white(line);
      while (strcmp(new_line, "") == 0){
        printf("%s\n",new_line);
        //strcpy(line, '\0');
        line=readline(sh->prompt);
        new_line = trim_white(line);
      }sh_init
      printf("%s\n",new_line);

      add_history(new_line);
      char **strings = cmd_parse(new_line);
      bool is_command = do_builtin(sh, strings);
      fork();
      //exit
      if (!is_command && strcmp(strings[0], "exit") == 0){
        free(line);
        //might be freeing already freed line
        free(new_line);
        cmd_free(strings);
        sh_destroy(sh);
        exit(EXIT_SUCCESS);
      }
      //other complication
      if (!is_command){
        fprintf(stderr, "not a command");
        free(line);
        free(new_line);
        cmd_free(strings);
        sh_destroy(sh);
        exit(EXIT_FAILURE);
      }
      cmd_free(strings);
      free(new_line);
    }
    free(line);
    
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


  /**
   * Changes the current working directory of the shell. Uses the linux system
   * call chdir. With no arguments the users home directory is used as the
   * directory to change to.
   *
   * @param dir The directory to change to
   * @return  On success, zero is returned.  On error, -1 is returned, and
   * errno is set to indicate the error.
   */
  int change_dir(char **dir){

    //no specified directory, go to home directory
    if(dir == NULL || dir[1] == NULL)
    {
      //grabbing home directory from environment
      char *home = getenv("HOME");
      //if not present, grab home directory with system calls
      if(home == NULL){
        struct passwd *nameHome;
        nameHome = getpwuid(getuid());
        //if system calls fail, exit with failure
        if (nameHome == NULL){
          return -1;
        }
        //copy home directory from nameHome password struct to home variable
        strcpy(home, nameHome->pw_dir);
      }
      int changed = chdir(home);
      return changed;
    } else 
    {
      //else go to specified directory
      int changed = chdir(dir[1]);
      return changed;
    }
    
  }

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
    //getting arg max
    const long ARG_MAX = sysconf(_SC_ARG_MAX); 

    //creating array of strings that are arguments
    char **strings = (char **)malloc(ARG_MAX * sizeof(char *));
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
    for (int i = 0; i < ARG_MAX - 1; i++) {


      //new string so start counting again
      while (!isspace((char)line[j]) && line[j] != '\0'){
        j++;
        k++;
      }


      //set the size of that string plus null pointer
      strings[i] = (char *)malloc((k + 1) * sizeof(char));

      //going back in index to set string contents
      j -= k;

      //transfering strings
      for (int l = 0; l < k; l++){
        strings[i][l] = line[j];
        j++;
      }

      //adding null character to end of new string
      strings[i][k] = '\0';

      //reset k to use again for next string to parse
      k = 0;
      
      //exit condition
      //if we have reached the end of the line
      if(line[j] == '\0'){
        strings[i + 1] = NULL;
        break;
      }

      //skip the possible space in between arguments(should only be one)
      j++;

      //exit condition, can't exceed this many arguments
      if(i == (ARG_MAX - 2)){
        strings[i + 1] = NULL;
        break;
      }
      
    }
    
    return strings;
  }

  /**
   * @brief Free the line that was constructed with parse_cmd
   *
   * @param line the line to free
   */
  void cmd_free(char ** line){
    for(int i = 0; line[i] != NULL; i++ ){
      free(line[i]);
    }
    free(line);
  }

  
  /**
   * @brief Takes an argument list and checks if the first argument is a
   * built in command such as exit, cd, jobs, etc. If the command is a
   * built in command this function will handle the command and then return
   * true. If the first argument is NOT a built in command this function will
   * return false.
   *
   * @param sh The shell
   * @param argv The command to check
   * @return True if the command was a built in command
   */
  bool do_builtin(struct shell *sh, char **argv){

    //changing directory
    if (strcmp(argv[0], "cd") == 0){
      int changed = change_dir(argv);
      if (changed < 0){
        return false;
      }
      return true;
    }

    //exit
    if (strcmp(argv[0], "exit") == 0){
      return false;
    }

    //history
    if (strcmp(argv[0], "history") == 0){
      //not sure if we need this state history
      HISTORY_STATE *history = history_get_history_state();
      if(history == NULL){
        return false;
      }
      //history entries
      HIST_ENTRY **list_commands = history_list();
      
      //print history entries
      for (int i = 0; list_commands[i] != NULL; i++ ){
        printf("%s\n", list_commands[i]->line);
      }
      return true;
    }

    return false;
  }

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
    sh->shell_pgid = 1;//get process group function getpgrp()
    sh->shell_terminal = 1;
    //TODO from tutor, figure out
    // sh->shell_terminal = STDIN_FILENO;
    // sh->shell_is_interactive = isatty(sh->shell_terminal);
    //getting prompt from environment
    const char *name = "MY_PROMPT";
    // char *env_prompt = get_prompt(name);
    // sh->prompt = (char *)malloc((strlen(env_prompt)+1)*sizeof(char));

    sh->prompt = get_prompt(name);
    if(sh->prompt == NULL){
      fprintf(stderr,"failed to allocate memory for shell struct's prompt");
      abort();
    }
    
    strcpy(sh->prompt, env_prompt);
    free(env_prompt);
    printf("in get promp did strcpy\n");

    getInput(sh);
  }

  /**
   * @brief Destroy shell. Free any allocated memory and resources and exit
   * normally.
   *
   * @param sh
   */
  void sh_destroy(struct shell *sh){
    free(sh->prompt);
    //TODO I am sure there is more to free
  }

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
    int vflag = 0;
    int c;
    //might need to add this back in, turns off error messages
    opterr = 0;
    char *cvalue = NULL;

    while ((c = getopt (argc, argv, "v")) != -1)
      switch (c)
        {
        case 'v':
          vflag = 1;
          break;
        case '?':
            fprintf (stderr,
                    "Unknown option character `\\x%x'.\n",
                    optopt);
        default:
          return;
        }
    if (vflag > 0){
      printf ("horray you picked v\n");
      fprintf(stdout, "version %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
      exit(EXIT_SUCCESS);
    }
    return;

  }
