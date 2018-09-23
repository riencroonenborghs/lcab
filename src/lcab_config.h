#ifndef LCAB_CONFIGH
#define LCAB_CONFIGH

#include "func.h"

struct lcab_config {
  int             makenopath; /* create no path */
  int             recursive;  /* recursive through dirs */
  int             diamond;
  int             quiet;
  int             dryrun;
  int             log_level;
  char            outputfile[FILENAME_MAX];
  char            output_dir[FILENAME_MAX];
  char            output_path[FILENAME_MAX];
  char            diamond_file[FILENAME_MAX];
  char            temp_file[FILENAME_MAX];
  struct sllitem *fileEntryItems;
  struct sllitem *rawFileItems;
};

#define DEFAULT_TEMP_FILE  "~lcab.tmp"
#define DEFAULT_OUTPUT_CAB "output.cab"
#define DEFAULT_OUTPUT_DIR "."

#endif
