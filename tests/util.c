#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <utime.h>

#include <check.h>

#include "util.h"

#define MAX 1000

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
  int rv = remove(fpath);

  if (rv)
    ck_abort_msg("Couldn't remove directory %s", fpath);

  return rv;
}

int lcabt_rmrf(const char *path)
{
  int err = nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);

  if (err)
    ck_abort_msg("Couldn't remove directory %s recursively.", path);
  return err;
}

void lcabt_write_file(const char *name, const char *contents)
{
  FILE *f = fopen(name, "w");

  if (f == NULL)
    ck_abort_msg("Error opening file %s", name);

  if (contents)
    fputs(contents, f);
  fclose(f);
}

void lcabt_mkdir(const char *name)
{
  struct stat st = { 0 }; \
  if (stat(name, &st) == -1) mkdir(name, 0700); \
}

void lcabt_rm(const char *name)
{
  struct stat st = { 0 };

  if (stat(name, &st) != -1) remove(name);
}

void lcabt_update_file_time(const char *filename, const long epoch_seconds)
{
  struct stat foo;
  time_t mtime;
  struct utimbuf new_times;

  stat(filename, &foo);
  new_times.actime = foo.st_atime;    // don't modify
  new_times.modtime = epoch_seconds;
  utime(filename, &new_times);
}

/* https://stackoverflow.com/questions/15455242/comparing-two-files-in-c */
int lcabt_files_differ(const char *f1, const char *f2)
{
  char c1, c2;
  char s1[MAX], s2[MAX];
  char *p1;
  char *p2;
  FILE *fp1;
  FILE *fp2;

  p1 = s1;
  p2 = s2;

  fp1 = fopen(f1, "r");
  fp2 = fopen(f2, "r");
  if (fp1 == NULL) {
    fprintf(stderr, "Unable to open %s", f1);
    return -1;
  }
  if (fp2 == NULL) {
    fprintf(stderr, "Unable to open %s", f2);
    return -1;
  }
  c1 = getc(fp1);
  c2 = getc(fp2);
  while ((c1 != EOF) && (c2 != EOF)) {
    for (; c1 != '\n'; p1++) {
      *p1 = c1;
      c1 = getc(fp1);
    }
    *p1 = '\0';

    for (; c2 != '\n'; p2++) {
      *p2 = c2;
      c2 = getc(fp2);
    }
    *p2 = '\0';
    if ((strcmp(s1, s2)) != 0) {
      printf("%s\n", s1);
      printf("%s\n", s2);
      return 2;
    }
    c1 = getc(fp1);
    c2 = getc(fp2);
    p1 = s1;
    p2 = s2;
  }
  if (c1 != EOF || c2 != EOF)
    // one file prematurely ended
    return 1;

  return 0;
}
