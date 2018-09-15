/*  Copyright 2001 Rien Croonenborghs, Ben Kibbey, Shaun Jackman, Ivan Brozovic
 *
 *  This file is part of lcab.
 *  lcab is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  lcab is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with lcab; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <getopt.h>
#include <errno.h>
#include "cstruct.h"
#include "cli.h"
#include "parse_dff.h"
#include "lcab_config.h"
#include "file_entry.h"
#include "lcab.h"
#include "warn.h"
#include "utils.h"
#include "config.h"

int process_argv(struct sllitem *item, void *ctx);
void print_usage_and_exit(char **argv, int exit_code);
int print_entry_real_cab_path(struct sllitem *item, void *template);
int recurse_directory(struct lcab_config *config, const char *file);



/* print the usage of lcab */
void print_usage_and_exit(char **argv, int exit_code)
{
  char *binary;

  binary = strippath(argv[0]);
  printf("Usage: %s [options] <input files...> <outputfile.cab>\n", binary);
  printf("       %s [options] -F <descriptor file>\n", binary);
  printf("\nlcab will create a Cabinet File from input files or from a supplied directives file.\n");
  printf("Inputfiles can also be directories together with option -r.\n");

  printf("General Options\n");
  printf(" -%-3s | --%-15s %s\n", "q", "quiet", "Reduce output to nil");
  printf(" -%-3s | --%-15s %s\n", "h", "help", "Display this help output");
  printf(" %4s   --%-15s %s\n", "", "dry-run", "Dry run only, do not create the .cab file");
  printf(" -%-3s | --%-15s %-10s %s\n", "V", "verbosity", "<number 0-100>", "Specify the level of output (higher = more)");
  printf("\n");
  printf("Command Line Only Options (not valid with a Descriptor File)\n"); printf(" -%-3s | --%-15s %s\n", "n", "make-no-path", "Add files to the Cabinet without path names");
  printf(" -%-3s | --%-15s %s\n", "r", "recursive", "Recurse into directories");

  printf("\n");
  printf("Directive File Specific Options\n");
  printf(" -%-3s | --%-15s %-10s %s\n", "F", "directives-file", "<file>", "Specifies the directive file to load");


  printf("\n%s v%s (2003) by Rien (rien@geekshop.be)\n", binary, VERSION);
  free(binary);
  exit(exit_code);
}

int print_entry_real_cab_path(struct sllitem *item, void *template)
{
  struct file_entry *p = item->data;

  debug((char *)template, p->real_path, p->cab_path);
  return 0;
}

/**************************************************************************
*  MAIN
**************************************************************************/
int main(int argc, char *argv[])
{
  trace("> main");

  struct lcab_config config = {
    .makenopath     = 0,
    .recursive      = 0,
    .diamond        = 0,
    .quiet          = 0,
    .temp_file      = DEFAULT_TEMP_FILE,
    .outputfile     = DEFAULT_OUTPUT_CAB,
    .output_dir     = DEFAULT_OUTPUT_DIR,
    .log_level      = INFO,
    .fileEntryItems = NULL,
  };


  int opt, err;


  static struct option long_options[] =
  {
    { "make-no-path",    no_argument,       0, 'n'         },
    { "recursive",       no_argument,       0, 'r'         },
    { "help",            no_argument,       0, 'h'         },
    { "quiet",           no_argument,       0, 'q'         },
    { "dry-run",         no_argument,       0, OPT_DRY_RUN },
    { "directives-file", required_argument, 0, 'F'         },
    { "verbosity",       required_argument, 0, 'V'         },
    { 0,                 0,                 0, 0           }
  };
  int option_index = 0;

  while ((opt = getopt_long(argc, argv, "nrhqF:V:", long_options, &option_index)) != EOF) {
    switch (opt) {
    case 'n':
      config.makenopath = 1;
      break;
    case 'r':
      config.recursive = 1;
      break;
    case 'h':
      print_usage_and_exit(argv, EXIT_OPTION_H);
      break;
    case 'q':
      config.quiet = 1;
      break;
    case 'F':
      config.diamond = 1;
      strcpy(config.diamond_file, optarg);
      break;
    case 'V':
      config.log_level = atoi(optarg);
      break;
    case OPT_DRY_RUN:
      config.dryrun = 1;
      break;
    default:
      print_usage_and_exit(argv, EXIT_UNKNOWN_OPTION);
    }
  }

  // need to set the log level first thing
  if (config.quiet)
    config.log_level = FATAL + 1;
  set_log_level(config.log_level);
  info("%s v%s (2003) by Rien (rien@geekshop.be)", strippath(argv[0]), VERSION);

  if (config.diamond && !exists(config.diamond_file)) {
    error("%s does not exist.", config.diamond_file);
    exit(EXIT_SPEC_FILE_NO_EXIST);
  }

  if (config.diamond && (config.recursive || config.makenopath)) {
    fatal("If you specify a spec file you cannot specify any other options.");
    print_usage_and_exit(argv, EXIT_SPEC_FILE_ONLY);
  }



  // list files to add
  while (optind < argc - 1) {
    /* if the inputfile starts with a .. continue, but warn the user */
    char *pargv = argv[optind++];
    trace(">> processing argv %s", pargv);
    if (!strncmp(pargv, "..", 2)) warn("WARNING : %s has a .. in front of it! #####", pargv);

    // store off the values in the config
    struct sllitem *new_slli = calloc(1, sizeof(struct sllitem));
    sllitem_init(new_slli, pargv);
    config.rawFileItems = prepend(config.rawFileItems, new_slli);
  }

  if (optind < argc)
    // peel off the last value
    strcpy(config.outputfile, argv[optind]);
  // end processing raw args


  // begin processing the configuration

  for_each(config.rawFileItems, process_argv, &config);

  if (strlen(config.diamond_file)) {
    trace(">> Diamond file set to %s", config.diamond_file);
    trace(">> Parsing %s", config.diamond_file);
    err = parse_dff(&config, add_input_file);
    if (err) {
      fatal("An error occurred parsing the specification file.");
      return errno;
    }
  }

  trace(">> set up the output_path");
  strcpy(config.output_path, config.output_dir);
  strcpy(config.output_path + strlen(config.output_path), "/");
  strcpy(config.output_path + strlen(config.output_path), config.outputfile);


  int ct = reduce(0, config.fileEntryItems, reducer_count);
  // end processing of arguments






  debug("%-20s : %s", "directives-file", config.diamond_file);
  debug("%-20s : %s", "dry-run", yes_or_no(config.dryrun));
  debug("%-20s : %s", "make-no-path", yes_or_no(config.makenopath));
  debug("%-20s : %s", "recursive", yes_or_no(config.recursive));
  debug("%-20s : %s", "quiet", yes_or_no(config.quiet));
  debug("%-20s : %s", "output directory", config.output_dir);
  debug("%-20s : %s", "output .cab", config.outputfile);
  debug("%-20s : %s", "output path", config.output_path);

  debug("%-20s", "input files");
  for_each(config.fileEntryItems, print_entry_real_cab_path, "%20s := %-20s");
  debug("%-20s : %d", "file count", ct);



  // validate we have > 0 files
  if (ct == 0) {
    warn("There are no files to package, exiting.");
    return EXIT_NO_FILES_TO_CAB;
  }
  // end validation

  if (!config.dryrun)
    write_cab_file(&config);

  cleanup(&config);

  info("Created %s with %d files.", config.output_path, ct);
  return 0;
}


int process_argv(struct sllitem *item, void *ctx)
{
  trace("> process_argv");
  struct lcab_config *config = ctx;
  char *path = item->data;

  trace(">> process_argv from %s", path);
  int r = config->recursive;

  /* if the given input file argument is a dir, loop through it */
  if (is_dir(path)) {
    trace(">> %s is directory, recurse? %s", path, yes_or_no(r));
    if (r)
      recurse_directory(config, path);
  } else {
    add_input_file(config, path, NULL);
  }
  return 0;
}

/* loop through a file (actually it's a dir) */
int recurse_directory(struct lcab_config *config, const char *file)
{
  DIR *d;
  struct dirent *dp;

  if ((d = opendir(file)) == NULL)
    return 1;

  while ((dp = readdir(d)) != NULL) {
    struct stat st;
    char buf[FILENAME_MAX];

    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
      continue;

    snprintf(buf, sizeof(buf), "%s/%s", file, dp->d_name);

    if (stat(buf, &st) == -1)
      continue;

    if (S_ISDIR(st.st_mode)) {
      recurse_directory(config, buf);
      continue;
    }

    add_input_file(config, buf, NULL);
  }

  closedir(d);
  return 0;
}
