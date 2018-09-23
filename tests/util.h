#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ftw.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <utime.h>

void lcabt_write_file(const char *name, const char *contents);
void lcabt_mkdir(const char *name);
void lcabt_rm(const char *name);
int lcabt_rmrf(const char *path);
void lcabt_update_file_time(const char *filename, long epoch_seconds);
int lcabt_files_differ(const char *f1, const char *f2);
