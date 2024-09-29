#define _POSIX_SOURCE
#include <string.h>
#include "../tests/harness/unity.h"
#include "../src/lab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <bits/getopt_core.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

    struct background_process
  {
    pid_t process_pid;
    char *strings;
    int job_num;
    int running; // 0 for not running, 1 for running, -1 for never set correctly
    bool printed;
  
  };

  //free jobs array
  void destroy_jobs(struct background_process **jobs){
    for(int i = 0; jobs[i] != NULL; i++){
      //TODO might need to check if these are null first
        free(jobs[i]->strings);
        free(jobs[i]);
    }
    free(jobs);
  }

  void print_jobs(struct background_process **jobs){
    //if the first job has not been initialized return
    if(jobs != NULL && jobs[0] == NULL){
      return;
    }
    for(int i = 0; jobs[i]!= NULL; i++){
      //if its already been printed continue
      if(jobs[i]->printed){
        continue;
      }
      if(jobs[i]->running == 1){
         fprintf(stdout, "\n[%d] %d Running %s\n", jobs[i]->job_num, jobs[i]->process_pid, jobs[i]->strings);
      } else if (jobs[i]->running == 1){
         fprintf(stdout, "\n[%d] Done %s\n", jobs[i]->process_pid, jobs[i]->strings);
      }
    }
  }

  //print jobs that are done in between enters
  void print_finished_jobs(struct background_process **jobs){
    //if the first job has not been initialized return
    if(jobs != NULL && jobs[0] == NULL){
      return;
    }
    for(int i = 0; jobs[i]!= NULL; i++){
      //if its already been printed continue
      if(jobs[i]->printed){
        continue;
      }
      int status = check_running(jobs[i]);
      //if the status has changed
      if (status != jobs[i]->process_pid){
        //update status
        jobs[i]->process_pid = status;
        //set to printed
        jobs[i]->printed = true;
        //print to stdout
        fprintf(stdout, "\n[%d] Done %s\n", jobs[i]->job_num, jobs[i]->strings);
      }
    }
  }

  //checks to see if the process is running
  //returns 1 if still running, 0 if not
  int check_running(pid_t job_pid){
    int status;
    pid_t running = waitpid(job_pid, &status, WNOHANG);
    if(running == job_pid){
      return 0;
    }else if (running == 0){
      return 1;
    }
    //hopefully we don't get here :)
    return (int)running;
  }

  
  pid_t launch_background(char **strings, struct background_process **jobs, int num_jobs){
    
    /*This is the background child process*/
    pid_t child = fork();
    // jobs[num_jobs]->process_pid = child;
    // jobs[num_jobs]->job_num = num_jobs;
    // jobs[num_jobs]->running = -1;
  

    if(child < 0){
      fprintf(stderr, "fork failed\n%s\n", strerror(errno));
      return child;
      // jobs[num_jobs]->running = 0;
    } else if (child == 0){
      // jobs[num_jobs]->running = 1;
      //putting child in own process group
      setpgid(child, child);

      if(strings == NULL || strings[0] == NULL){
        printf("null strings");
        return -1;
      }
      //fprintf(stdout, "\n[%d] %d %s\n", num_jobs, (int)jobs[num_jobs]->process_pid, jobs[num_jobs]->strings);
      execvp(strings[0], strings);
      //print process
      
      cmd_free(strings);
    }
    

    //fprintf(stderr, "exec failed\n%s\n", strerror(errno));
    return child;


  }


  pid_t launch_child(char **strings, struct shell *sh){
    /*This is the child process*/
    pid_t child = fork();
    if(child < 0){
      fprintf(stderr, "fork failed\n%s\n", strerror(errno));
      return child;
    } else if (child == 0){
      // fprintf("\n%ld\n", child);
      setpgid(child, child);
      tcsetpgrp(sh->shell_terminal, getpid()); //child has control
      signal (SIGINT, SIG_DFL);
      signal (SIGQUIT, SIG_DFL);
      signal (SIGTSTP, SIG_DFL);
      signal (SIGTTIN, SIG_DFL);
      signal (SIGTTOU, SIG_DFL);
      if(strings == NULL || strings[0] == NULL){
        printf("null strings");
        return -1;
      }
      execvp(strings[0], strings);


    }
    

    //fprintf(stderr, "exec failed\n%s\n", strerror(errno));
    return child;


  }


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
      // printf("i=%d\t", i);
      // printf("\nfirst for loop: char=%c\n", line[i]);
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
      // printf("i=%d\t", i);
      // printf("\nsecond for loop: char=%c\n", line[i]);
      //accounting for null terminator
      if (!isspace((unsigned char)line[i])){
        back_index = i;
        break;
      }

    }
    //if there were no spaces, just return line
    if (front_index == 0 && back_index == (int)strlen(line)-1){
      // printf("there were no spaces");
      return line;
    }

    //if all spaces
    if(front_index == ((int)strlen(line)-1)){
      // printf("there were all spaces");
      char *new_line =(char *)malloc(sizeof(char)*strlen(line));
      if(new_line == NULL){
        fprintf(stderr,"failed to allocate memory for new_line");
        abort();
      }
      new_line[0] = '\0';
      strcpy(line, new_line);
      free(new_line);
      return line;
    }

    // //getting the new line's shortened length 
    // size_t new_line_length = back_index - front_index + 1;
    // //creating new line
    // char *new_line = (char *)malloc((new_line_length + 1));

    //trying to use just line's length instead
    char *new_line = (char *)malloc((strlen(line)+1));

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

    strcpy(line, new_line);
    free(new_line);

    return line;
  }

  void getInput(struct shell *sh) {
    
    //STARTING PROGRAM
    char *line;
    //settings up array of background jobs
    int num_jobs = 0;
    int max_jobs = 10;
    struct background_process **jobs = malloc(sizeof(struct background_process)*max_jobs);
    if(jobs == NULL){
      fprintf(stderr,"failed to allocate memory for jobs array");
      abort();
    }
    for(int i = 0; i <= max_jobs; i++){
      jobs[i] = NULL;
    }

    //accessing user input
    using_history();
    bool is_command;

    //changing prompt to be prompt set in environment
    while ((line=readline(sh->prompt) )){

      //handle enter
      if (strcmp(line, "") == 0){
        // printf("%s\n", line);
        free(line);
        if(jobs != NULL){
          print_finished_jobs(jobs);
        }
        continue;
      }

      //EOF exit 
      if(line == NULL){
        printf("EOF");
        //might take this free out
        free(line);
        rl_clear_history();
        clear_history();
        if(jobs != NULL){
          destroy_jobs(jobs);
        }
        sh_destroy(sh);
        exit(EXIT_SUCCESS);
      }

      char *new_line = trim_white(line);
      if (strcmp(new_line, "") == 0){
        // printf("%s\n",new_line);
        free(line);
        free(new_line);
        if (jobs != NULL){
          print_finished_jobs(jobs);
        }
        continue;
      }
      add_history(new_line);
      char **strings;
      
      //check for BACKGROUND PROCCESS with &
      if(new_line[strlen(new_line)-1] == '&'){
        //getting rid of &
        new_line[strlen(new_line)-1] = '\0';
        //trimming whitespace again
        new_line = trim_white(new_line);
        strings = cmd_parse(new_line);
        //checking array size
        //plus one because array must end with a NULL
        if (max_jobs == num_jobs + 1){
          //if the array needs to be bigger, double it
          max_jobs *= 2;
          jobs = realloc(jobs, sizeof(struct background_process *)*max_jobs);
          if(jobs == NULL){
            fprintf(stderr,"failed to allocate memory for jobs array double");
            abort();
          }
          for(int i = (max_jobs/2); i <= max_jobs; i++){
            jobs[i] = NULL;
          }
        }
        //allocating memory for child job beckground
        jobs[num_jobs] = (struct background_process *)malloc(sizeof(struct background_process));
        //indicating end of array of jobs
        jobs[num_jobs + 1] = NULL;
        if(jobs[num_jobs] == NULL){
            fprintf(stderr,"failed to allocate memory for child job");
            abort();
        }
        //storing command for child job
        jobs[num_jobs]->strings = (char *)malloc((strlen(line)+1)*sizeof(char));
        strcpy(jobs[num_jobs]->strings, line);


        //launching child job in background
        pid_t child_pid = launch_background(strings, jobs, num_jobs);
        jobs[num_jobs]->process_pid = child_pid;
        jobs[num_jobs]->job_num = num_jobs;
        jobs[num_jobs]->running = check_running(jobs[num_jobs]->process_pid);
        jobs[num_jobs]->printed = false;
        //print process
        fprintf(stdout, "\n[%d] %d %s\n", num_jobs, (int)jobs[num_jobs]->process_pid, jobs[num_jobs]->strings);
        num_jobs++;
        cmd_free(strings);
        if(line != NULL){
          free(line);
        }
        // if(new_line != NULL){
        //   free(new_line);
        // }

        //once this is added to the bckground, skip rest of while and go back into the while
      
        continue;
      }

      strings = cmd_parse(new_line);
      // free(new_line);
      is_command = do_builtin(sh, strings);

      if(is_command && strcmp(strings[0], "jobs") == 0){
        if(jobs != NULL){
          print_jobs(jobs);
        }
      }

      //regular exit with exit command
      if (!is_command && strcmp(strings[0], "exit") == 0){
        if(jobs != NULL){
          destroy_jobs(jobs);
        }
        
        rl_clear_history();
        cmd_free(strings);
        free(line);
        sh_destroy(sh);
        exit(EXIT_SUCCESS);
      }
      

      //other complication
      //create a foreground child process here
      if (!is_command){

        pid_t child_pid = launch_child(strings, sh);
        int status;
        pid_t parent_wait = 0;
        //wait until child process is done
        while (parent_wait != child_pid){
          parent_wait = waitpid(child_pid, &status, 0);
        }
        if(WIFEXITED(status)){
          // printf("\n%d\n", WEXITSTATUS(status));
          tcsetpgrp(sh->shell_terminal, sh->shell_pgid); //giving the parent control back
        }
        

        cmd_free(strings);
        

      }

      free(line);
      
    } 
    sh_destroy(sh);
    rl_clear_history();
    destroy_jobs(jobs);
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
    char **strings = (char **)malloc(ARG_MAX*(sizeof(char *)));
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
        if(line[j] != '&'){
          strings[i][l] = line[j];
          
        }
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
    
    // free(ARG_MAX);
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

    //jobs command
    if (strcmp(argv[0], "jobs") == 0){
      return true;
    }

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
      // //not sure if we need this state history
      // HISTORY_STATE *history = history_get_history_state();
      // if(history == NULL){
      //   return false;
      // }
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

    sh->shell_terminal = STDIN_FILENO;
    sh->shell_is_interactive = isatty(sh->shell_terminal);


    sh->shell_pgid = getpgrp();//get process group function getpgrp()

    if(sh->shell_is_interactive == 1){

      /* Loop until we are in the foreground.  */
      while (tcgetpgrp (sh->shell_terminal) != (sh->shell_pgid = getpgrp ()))
        kill (- sh->shell_pgid, SIGTTIN);

    }
    //from task 8
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    //might need this one
    // signal (SIGCHLD, SIG_IGN);

    /* Put ourselves in our own process group.  */
    sh->shell_pgid = getpid ();
    if (setpgid (sh->shell_pgid, sh->shell_pgid) < 0)
    {
      perror ("Couldn't put the shell in its own process group");
      // tearDown();
      exit (1);
    }

    /* Grab control of the terminal.  */
    tcsetpgrp (sh->shell_terminal, sh->shell_pgid);

    //potentially fixing ctl-d being handled by host
    sh->shell_tmodes.c_cflag |= ICANON;

    /* Save default terminal attributes for shell.  */
    tcgetattr (sh->shell_terminal, &sh->shell_tmodes);

    //getting prompt from environment
    const char *name = "MY_PROMPT";
    char *env_prompt = get_prompt(name);
    sh->prompt = (char *)malloc((strlen(env_prompt)+1)*sizeof(char));

    // sh->prompt = get_prompt(name);
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
    printf("in sh destroy");
    free(sh->prompt);
    free(sh);
    
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
