#include <stdio.h>
#include <string.h>
#include "filesystem.h"
#include "memory-checking.h"

/* This driver uses some features of C I/O that won't be explained in the
 * course.  You don't need to read or to understand this code (unless you
 * want to).
 *
 * Note that it imposes a limit of 511 characters for an input line, and 80
 * characters for any individual argument.  These limits are NOT part of
 * your code- the project does not specify any maximum length for an
 * argument- but are just part of this driver.
 *
 * Lastly note that the project tests may or may not use this driver (or
 * some may and some might not).
 */

#define LINE_MAX 512
#define WORD_MAX (80 + 1)

static int command_idx(char name[]);

/* these are all the commands the driver recognizes, which include a few that
   are not functions appearing in filesystem.h */
enum COMMANDS {LOGOUT, EXIT, MKFS, TOUCH, MKDIR, CD, LS, PWD, RM, RENAME, RMFS,
               SET, UNSET} commands;
static char *command_names[]= {"logout", "exit", "mkfs", "touch", "mkdir",
                               "cd", "ls", "pwd", "rm", "rename", "rmfs",
                               "set", "unset"};

/* convert command names to indices that match the value of one of the enum
   constants in COMMANDS; an unrecognized command name results in -1 being
   returned */
static int command_idx(char name[]) {
  int i= 0, pos= -1;

  while (i < sizeof(command_names) / sizeof(command_names[0]) && pos == -1)
    if (strcmp(name, command_names[i]) == 0)
      pos= i;
    else i++;

  /* returns -1 if the command name was not found */
  return pos;
}

int main() {
  Filesystem filesystem;
  char line[LINE_MAX]= "", command[WORD_MAX]= "", temp[WORD_MAX],
       arg1[WORD_MAX]= "", arg2[WORD_MAX]= "", prompt[WORD_MAX]= "%";
  int verbose= 0, length= 0, num_matched= 0, argument_error, done= 0;

  setup_memory_checking();

  printf("%s ", prompt);
  /* continue reading lines until the end of the input */
  while (!done && fgets(line, sizeof(line) - sizeof(*line), stdin) != NULL) {

    temp[0]= '\0';
    sscanf(line, "%[ \t\n]%n", temp, &length);

    if (strcmp(temp, "") != 0 && length == strlen(temp))
      printf("%s ", prompt);  /* ignore empty input lines */

      else {

        /* num_matched is the number of format specifiers that were matched */
        num_matched= sscanf(line, "%s %s %s %s", command, arg1, arg2, temp);
        if (verbose == 1)
          printf("%s", line);

        /* argument_error will be 1 if the wrong number of arguments are
           given for any command */
        argument_error= 0;

        switch(command_idx(command)) {

          case LOGOUT:
          case EXIT:
            if (num_matched == 1)
              done= 1;  /* also quit if the command entered is logout or
                           exit */
            else argument_error= 1;
            break;

          /* call mkfs() if the line began with "mkfs" with no following
             arguments */
          case MKFS:
            if (num_matched == 1)
              mkfs(&filesystem);
            else argument_error= 1;
            break;

          /* call touch() if the line began with "touch" and had one
             following argument; if touch() returns -1 print an appropriate
             error message */
          case TOUCH:
            if (num_matched != 2)
              argument_error= 1;
            else
              if (touch(&filesystem, arg1) == -1)
                printf("Missing or invalid operand.\n");
            break;

          /* call mkdir() if the line began with "mkdir" and had one
             following argument; if mkdir() returns -1 or -2 print an
             appropriate error message */
          case MKDIR:
            if (num_matched != 2)
              argument_error= 1;
            else
              switch (mkdir(&filesystem, arg1)) {
                case -1: printf("Missing or invalid operand.\n");
                         break;
                case -2: printf("Cannot create directory %s: File exists.\n",
                                arg1);
                         break;
                default: break;  /* no-op; 0 return is expected */
              }
            break;

          /* call cd() if the line began with "cd" and had one following
             argument; if cd() returns -1 or -2 print an appropriate error
             message */
          case CD:
            if (num_matched != 1 && num_matched != 2)
              argument_error= 1;
            else
              switch (cd(&filesystem, arg1)) {
                case -1: printf("%s: No such file or directory.\n", arg1);
                         break;
                case -2: printf("%s: Not a directory.\n", arg1);
                         break;
                default: break;  /* no-op; 0 return is expected */
              }
            break;

          /* call ls() if the line began with "ls" and had one following
             argument; if ls() returns -1 print an appropriate error
             message */
          case LS:
            if (num_matched != 1 && num_matched != 2)
              argument_error= 1;
            else
              if (ls(filesystem, arg1) == -1)
                printf("%s: No such file or directory.\n", arg1);
            break;

          /* call pwd() if the line began with "pwd" with no following
             arguments */
          case PWD:
            if (num_matched == 1)
              pwd(filesystem);
            else argument_error= 1;
            break;

          /* call rm() if the line began with "rm" and had one following
             argument; if rm() returns -1, -2, or -3 print an appropriate
             error message */
          case RM:
            if (num_matched != 2)
              argument_error= 1;
            else
              switch (rm(&filesystem, arg1)) {
                case -1: printf("%s: No such file or directory.\n", arg1);
                         break;
                case -2: printf("Cannot remove directory '%s'.\n", arg1);
                         break;
                case -3: printf("Missing or invalid operand.\n");
                         break;
                default: break;  /* no-op; 0 return is expected */
              }
            break;

           /* call re_name() if the line began with "rename" with two
              following arguments; if it returns an error code (-1 through
              -4) print an appopriate error message */
          case RENAME:
            if (num_matched != 3)
              argument_error= 1;
            else
              switch (re_name(&filesystem, arg1, arg2)) {
                case -1: printf("%s: No such file or directory.\n", arg1);
                         break;
                case -2: printf("Missing or invalid operand.\n");
                         break;
                case -3: printf("File or directory %s already exists.\n",
                                arg1);
                         break;
                case -4: printf("%s and %s are the same file.\n", arg1, arg2);
                         break;
                default: break;  /* no-op; 0 return is expected */
              }
            break;

           /* call rmfs() if the line began with "rmfs" with no following
              arguments */
          case RMFS:
            if (num_matched == 1)
              rmfs(&filesystem);
            else argument_error= 1;
            break;

          /* the variable verbose is set to 1 if the "set verbose" command
             is entered. */
          case SET:
            if (num_matched == 2 && strcmp(arg1, "verbose") == 0)
              verbose= 1;
            else argument_error= 1;
            break;

            /* the variable verbose is set to 0 if "unset verbose" is
               entered. */
          case UNSET:
            if (num_matched == 2 && strcmp(arg1, "verbose") == 0)
              verbose= 0;
            else argument_error= 1;
            break;

          /* error message for a command not matching one of the function
             names */
          default: printf("%s: Command not found.\n", command);
            break;
        }

        /* error message for a command with the wrong number of arguments */
        if (argument_error)
          printf("Invalid arguments.\n");

        if (verbose == 1)
          printf("\n");
        printf("%s ", prompt);

        /* sscanf() will fail if the line read is empty, in which case the
           prior command will be executed again unless it's first cleared
           out */
        arg1[0]= arg2[0]= command[0]= '\0';
      }
  }

  check_memory_leak();

  return 0;
}
