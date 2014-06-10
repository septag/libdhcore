
//
// commander.h
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

/* Sepehr Taghdisian: I modified the API to be able to export functions as DLL 
 * Also added more support for positional arguments */

#ifndef COMMANDER_H
#define COMMANDER_H

/**
 * @defgroup cmd Command line parser
 * Simple command line parser, from a code by TJ Holowaychuk\n
 * Example:\n
 * @code
 * int verbose = FALSE;
 * char filepath[255];
 *
 * // callback for parsing @e verbose argument 
 * static void cmdline_verbose(command_t* cmd, void* param)
 * {
 *    verbose = TRUE;
 * }
 * // callback for parsing @e filepath argument
 * static void cmdline_filepath(command_t* cmd, void* param)
 * {
 *    strcpy(filepath, cmd->arg);    
 * }
 *
 * int main(int argc, char** argv)
 * {
 *   command_t cmd;
 *   command_init(&cmd, "myapp", "1.0");
 *   command_option(&cmd, "-v", "--verbose", "Verbose mode", cmdline_verbose);
 *   command_option_pos(&cmd, "filepath", "Source filepath", cmdline_filepath); // positional arg
 *   command_parse(&cmd, argc, argv, NULL);
 *   command_free(&cmd);
 *   return 0;
 * }
 * @encode
 */

#include "core-api.h"

/*
 * Max options that can be defined.
 */

#ifndef COMMANDER_MAX_OPTIONS
#define COMMANDER_MAX_OPTIONS 32
#endif

/*
 * Max arguments that can be passed.
 */

#ifndef COMMANDER_MAX_ARGS
#define COMMANDER_MAX_ARGS 32
#endif

/*
 * Command struct.
 */

struct command;

/**
 * Callback function for parsing arguments\n
 * @param self Commander object that is used to parse arguments, @e self->arg is the optional 
 * argument value or is NULL if no value is provided.
 * @see command_option
 * @see command_option_pos
 * @ingroup cmd
 */
typedef void (* command_callback_t)(struct command *self, void* param);

/*
 * Command option.
 */

typedef struct {
  int optional_arg;
  int required_arg;
  char *argname;
  char *large;
  const char *small;
  const char *large_with_arg;
  const char *description;
  command_callback_t cb;
} command_option_t;

typedef struct {
  const char* name;
  const char* description;
  int optional;
  command_callback_t cb;
} command_option_pos_t;

/*
 * Command.
 */

typedef struct command {
  void *data;
  const char *usage;
  const char *arg;
  const char *name;
  const char *version;
  int option_count;
  command_option_t options[COMMANDER_MAX_OPTIONS];
  int pos_count;
  command_option_pos_t poss[COMMANDER_MAX_ARGS];
  int argc;
  char *argv[COMMANDER_MAX_ARGS];
  char **nargv;
} command_t;


/**
 * Initialize command line parser 
 * @param self A valid commander object
 * @param name Name of the application
 * @param version Version of the application
 * @ingroup cmd
 */
CORE_API void command_init(command_t *self, const char *name, const char *version);

/**
 * Free command line parser
 * @ingroup cmd
 */
CORE_API void command_free(command_t *self);

/**
 * Adds command line argument 
 * @param self Commander object
 * @param small Short argument name (example: "-v")
 * @param large Long argument name (example: "--verbose"). Long argument name can contain optional or
 * required value, for example "--file <filename>" defines a @e --file argument with mandatory 
 * "filename" argument (accessed with cmd->arg in callbacks), and "--file [filename]" defines a @e --file 
 * argument with optional "filename" argument (in case of no filename, cmd->arg is NULL).
 * @param desc Description message for argument. Can be seen with @e --help flag
 * @param cb Callback function for parsing argument value
 * @ingroup cmd
 */
CORE_API void command_option(command_t *self, const char *small, const char *large, 
  const char *desc, command_callback_t cb);

/** 
 * Adds positional command line argument
 * @param self Commander object
 * @param name Argument name, shown in help
 * @param desc Argument description
 * @param optional Boolean value that defines if argument can be optional or mandatory
 * @param cb Callback function for pasrsing argument value (accessed with command_t->arg)
 * @ingroup cmd
 */
CORE_API void command_option_pos(command_t *self, const char *name, const char* desc, 
  int optional, command_callback_t cb);

/**
 * Parses command line arguments from @e main function inputs. Must be called after initialization 
 * and arguments setup
 * @param self Commander object
 * @param argc Number of total arguments (from the main function)
 * @param argv Actual command line arguments (from the main function)
 * @param param Custom user-defined pointer, this pointer will be passed to callbacks
 * @ingroup cmd
 */
CORE_API void command_parse(command_t *self, int argc, char **argv, void* param);

#endif /* COMMANDER_H */
