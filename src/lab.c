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

void setUp(void) {
  // set stuff up here
  printf("In set up\n");
  struct shell shelly;
  
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
    //TODO add error handling, look at old labs
    const char *prompt = getenv(env);

    if(prompt != NULL){
      char *actual_prompt = (char *)malloc((strlen(prompt) + 1) * sizeof(char));
      strcpy(actual_prompt, prompt);
      return actual_prompt;
    }

    char *actual_prompt = (char *)malloc(7 * sizeof(char));
    strcpy(actual_prompt, "shell>");
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
    //TODO
    return 2;
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
    //TODO
    return 't';
  }

  /**
   * @brief Free the line that was constructed with parse_cmd
   *
   * @param line the line to free
   */
  void cmd_free(char ** line){
    //TODO
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
    //TODO
    return 't';
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
    //TODO
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
    //TODO
  }

  /**
   * @brief Destroy shell. Free any allocated memory and resources and exit
   * normally.
   *
   * @param sh
   */
  void sh_destroy(struct shell *sh){
    //TODO
  }

  /**
   * @brief Parse command line args from the user when the shell was launched
   *
   * @param argc Number of args
   * @param argv The arg array
   */
  void parse_args(int argc, char **argv){
    printf("in parse function\n");
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
          return 1;
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
      return 1;
    }

  }



// void test_cmd_parse2(void)
// {
//      //The string we want to parse from the user.
//      //foo -v
//      char *stng = (char*)malloc(sizeof(char)*7);
//      strcpy(stng, "foo -v");
//      char **actual = cmd_parse(stng);
//      //construct our expected output
//      size_t n = sizeof(char*) * 6;
//      char **expected = (char**) malloc(sizeof(char*) *6);
//      memset(expected,0,n);
//      expected[0] = (char*)malloc(sizeof(char)*4);
//      expected[1] = (char*)malloc(sizeof(char)*3);
//      expected[2] = (char*)NULL;

//      strcpy(expected[0], "foo");
//      strcpy(expected[1], "-v");
//      TEST_ASSERT_EQUAL_STRING(expected[0],actual[0]);
//      TEST_ASSERT_EQUAL_STRING(expected[1],actual[1]);
//      TEST_ASSERT_FALSE(actual[2]);
//      free(expected[0]);
//      free(expected[1]);
//      free(expected);
// }

// void test_cmd_parse(void)
// {
//      char **rval = cmd_parse("ls -a -l");
//      TEST_ASSERT_TRUE(rval);
//      TEST_ASSERT_EQUAL_STRING("ls", rval[0]);
//      TEST_ASSERT_EQUAL_STRING("-a", rval[1]);
//      TEST_ASSERT_EQUAL_STRING("-l", rval[2]);
//      TEST_ASSERT_EQUAL_STRING(NULL, rval[3]);
//      TEST_ASSERT_FALSE(rval[3]);
//      cmd_free(rval);
// }

// void test_trim_white_no_whitespace(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "ls -a", 10);
//      char *rval = trim_white(line);
//      TEST_ASSERT_EQUAL_STRING("ls -a", rval);
//      free(line);
// }

// void test_trim_white_start_whitespace(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "  ls -a", 10);
//      char *rval = trim_white(line);
//      TEST_ASSERT_EQUAL_STRING("ls -a", rval);
//      free(line);
// }

// void test_trim_white_end_whitespace(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "ls -a  ", 10);
//      char *rval = trim_white(line);
//      TEST_ASSERT_EQUAL_STRING("ls -a", rval);
//      free(line);
// }

// void test_trim_white_both_whitespace_single(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, " ls -a ", 10);
//      char *rval = trim_white(line);
//      TEST_ASSERT_EQUAL_STRING("ls -a", rval);
//      free(line);
// }

// void test_trim_white_both_whitespace_double(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "  ls -a  ", 10);
//      char *rval = trim_white(line);
//      TEST_ASSERT_EQUAL_STRING("ls -a", rval);
//      free(line);
// }

// void test_trim_white_all_whitespace(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "  ", 10);
//      char *rval = trim_white(line);
//      TEST_ASSERT_EQUAL_STRING("", rval);
//      free(line);
// }

// void test_trim_white_mostly_whitespace(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "    a    ", 10);
//      char *rval = trim_white(line);
//      TEST_ASSERT_EQUAL_STRING("a", rval);
//      free(line);
// }

// void test_get_prompt_default(void)
// {
//      char *prompt = get_prompt("MY_PROMPT");
//      TEST_ASSERT_EQUAL_STRING(prompt, "shell>");
//      free(prompt);
// }

// void test_get_prompt_custom(void)
// {
//      const char* prmpt = "MY_PROMPT";
//      if(setenv(prmpt,"foo>",true)){
//           TEST_FAIL();
//      }

//      char *prompt = get_prompt(prmpt);
//      TEST_ASSERT_EQUAL_STRING(prompt, "foo>");
//      free(prompt);
//      unsetenv(prmpt);
// }

// void test_ch_dir_home(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "cd", 10);
//      char **cmd = cmd_parse(line);
//      char *expected = getenv("HOME");
//      change_dir(cmd);
//      char *actual = getcwd(NULL,0);
//      TEST_ASSERT_EQUAL_STRING(expected, actual);
//      free(line);
//      free(actual);
//      cmd_free(cmd);
// }

// void test_ch_dir_root(void)
// {
//      char *line = (char*) calloc(10, sizeof(char));
//      strncpy(line, "cd /", 10);
//      char **cmd = cmd_parse(line);
//      change_dir(cmd);
//      char *actual = getcwd(NULL,0);
//      TEST_ASSERT_EQUAL_STRING("/", actual);
//      free(line);
//      free(actual);
//      cmd_free(cmd);
// }

// int main(void) {
//   UNITY_BEGIN();
//   RUN_TEST(test_cmd_parse);
//   RUN_TEST(test_cmd_parse2);
//   RUN_TEST(test_trim_white_no_whitespace);
//   RUN_TEST(test_trim_white_start_whitespace);
//   RUN_TEST(test_trim_white_end_whitespace);
//   RUN_TEST(test_trim_white_both_whitespace_single);
//   RUN_TEST(test_trim_white_both_whitespace_double);
//   RUN_TEST(test_trim_white_all_whitespace);
//   RUN_TEST(test_get_prompt_default);
//   RUN_TEST(test_get_prompt_custom);
//   RUN_TEST(test_ch_dir_home);
//   RUN_TEST(test_ch_dir_root);

//   return UNITY_END();
// }