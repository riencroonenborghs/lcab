#ifndef UTILSH
#include "mytypes.h"
#define UTILSH
bool is_dir(const char *file);
bool is_file(const char *file);
bool exists(const char *file);

long file_size(char *filename);

bool starts_with(const char *pre, const char *str);
void chomp(const char *str);
char *strippath(char *);
char *changepath(char *, const char from_char, const char to_char);
const char *yes_or_no(bool value);
#endif
