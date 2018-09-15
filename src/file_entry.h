#ifndef FILENTRYH
#define FILENTRYH
struct file_entry {
  /* the actual FS path */
  char  real_path[255];
  /* the path in the cb */
  char  cab_path[255];
  long  size;
};
#endif
