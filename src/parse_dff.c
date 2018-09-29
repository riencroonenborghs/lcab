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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lcab_config.h"
#include "warn.h"
#include "utils.h"
#include "cli.h"

int parse_dff(struct lcab_config *config, void add_input_file(struct lcab_config *, char *, char *))
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char dest_dir[FILENAME_MAX];
  char path[FILENAME_MAX];

  memset(dest_dir, 0, FILENAME_MAX);

  trace("> parse_dff");
  fp = fopen(config->diamond_file, "r");
  if (!fp) {
    error("An error occurred opening the diamond file");
    return errno;
  }


  // iterate over each line of the file.
  while ((read = getline(&line, &len, fp)) != -1) {
    memset(path, 0, FILENAME_MAX);
    // empty line
    if (read == 1)
      continue;
    chomp(line);

    if ((char)line[0] != '.') {
      if (strlen(dest_dir) > 0) {
        strcpy(path, dest_dir);
        strcpy(path + strlen(path), "\\");
        strcpy(path + strlen(path),
               strrchr(line, '\\')
               ? strrchr(line, '\\') + 1
               : line
               );
      } else {
        strcpy(path, line);
      }
      add_input_file(config, line, path);
      continue;
    }

    if (starts_with(".Set ", line)) {
      strtok(line, " ");
      char *setting = strtok(NULL, "=");
      char *setting_value = strtok(NULL, "=");
      if (strcmp(setting, "DiskDirectory1") == 0) {
        strcpy(config->output_dir, setting_value);
        trace(">> set output dir to %s", config->output_dir);
      } else if (strcmp(setting, "CabinetNameTemplate") == 0) {
        strcpy(config->output_file, setting_value);
        trace(">> set output file to %s", config->output_file);
      } else if (strcmp(setting, "DestinationDir") == 0) {
        memset(dest_dir, 0, FILENAME_MAX);
        strcpy(dest_dir, setting_value);
        trace(">> set current cab folder to %s", dest_dir);
      } else {
        debug("Ignoring Setting %s", setting);
      }
    }
  }

  fclose(fp);

  if (line) free(line);

  return 0;
}
