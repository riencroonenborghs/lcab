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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mytypes.h"
#include "warn.h"

#define YES "yes"
#define NO "no"
const char *yes_or_no(bool value)
{
  return value ? YES : NO;
}

void chomp(char *str)
{
  str[strcspn(str, "\r\n")] = 0;
}

/* strips the path from a filename */
char *strippath(const char *filename)
{
  char *loc;
  char *result;

  trace("> strippath %s", filename);
  loc = strrchr(filename, '/');
  trace(">> loc %s", loc);

  if (!loc) {
    result = calloc(strlen(filename) + 1, sizeof(char));
    strcpy(result, filename);
  } else {
    loc++; // +1 to get past the '/'
    result = calloc(strlen(loc) + 1, sizeof(char));
    strcpy(result, loc);
  }
  return result;
}

/* replace "/" in filename with "\" (= windows path separator) */
char *changepath(const char *filename, const char from, const char to)
{
  char *tmp;
  int i = 0;
  int j = 0;

  tmp = (char *)calloc(strlen(filename) + 1, sizeof(char));
  for (i = 0; i < strlen(filename); ++i) {
    if (filename[i] != from) tmp[j] = filename[i];
    else tmp[j] = to;
    ++j;
  }
  tmp[j] = STRINGTERM;
  return tmp;
}

/* calculate filesize in bytes for filename
 * could've used stat here, but returns blocksize */
long file_size(char *filename)
{
  errno = 0;

  long size = 0;
  FILE *fp = fopen(filename, "rb");

  if (!fp) {
    error("Could not open %s", filename);
    exit(1);
  }
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);
  fclose(fp);
  return size;
}


bool exists(const char *file)
{
  struct stat st;

  errno = 0;
  if (stat(file, &st) < 0) {
    errno = 0;
    return false;
  }
  return true;
}

bool is_file(const char *file)
{
  struct stat st;

  errno = 0;
  if (stat(file, &st) < 0) {
    errno = 0;
    return false;
  }
  return 0 == S_ISDIR(st.st_mode);
}

/**
 * clears errno
 */
bool is_dir(const char *file)
{
  struct stat st;

  errno = 0;
  if (stat(file, &st) < 0) {
    errno = 0;
    return false;
  }
  return S_ISDIR(st.st_mode);
}

bool starts_with(const char *pre, const char *str)
{
  size_t lenpre = strlen(pre),
         lenstr = strlen(str);

  return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
