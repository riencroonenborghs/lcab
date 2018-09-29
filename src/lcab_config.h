#ifndef LCAB_CONFIGH
#define LCAB_CONFIGH

#include "func.h"

struct lcab_config {
  int             makenopath; /* create no path */
  int             recursive;  /* recursive through dirs */
  int             diamond;
  int             quiet;
  int             dryrun;

  /* see warn.h */
  int             log_level;

  /* the name of the cab file to produce */
  char            output_file[FILENAME_MAX];

  /* into which directory should the cab file be created */
  char            output_dir[FILENAME_MAX];

  /* the path of the .dff file to process */
  char            diamond_file[FILENAME_MAX];

  /* the actual file entries to be added to the cab */
  struct sllitem *fileEntryItems;

  /* the are coming from the command line */
  struct sllitem *rawFileItems;
};

#define DEFAULT_TEMP_FILE  "~lcab.tmp"
#define DEFAULT_OUTPUT_CAB "output.cab"
#define DEFAULT_OUTPUT_DIR "."

#endif
