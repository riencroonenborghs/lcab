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
#include <errno.h>
#include <sys/param.h>
#include "mytypes.h"
#include "func.h"
#include "lcab.h"
#include "lcab_config.h"
#include "warn.h"
#include "utils.h"
#include "cwrite.h"
#include "file_entry.h"
#include "cfolder.h"
#include "cdata.h"
#include "cfile.h"
#include "cheader.h"

/* private methods */
int calc_datablock_count(struct sllitem *start);
int build_cab_file_meta(struct lcab_config *config, struct cmeta *meta);
int make_output_dir(struct lcab_config *config);
int write_temp_file(struct lcab_config *config);
int cat_to_file(struct sllitem *items, FILE *fp);
void cleanup(struct lcab_config *config);
void cleanup_config(struct lcab_config *config);
int remove_temp_file(struct lcab_config *config);



/**************************************************************************
*  HELPERS
**************************************************************************/
void add_input_file(struct lcab_config *config, char *real_path, char *cab_path)
{
  trace("> add_input_file(%s, %s)", real_path, cab_path);
  char *rp = changepath(real_path, '\\', '/');

  if (!exists(rp)) {
    warn("%s does not exist, ignoring...", rp);
    return;
  }
  if (!is_file(rp)) {
    warn("%s is not a file, ignoring...", rp);
    return;
  }

  if (!cab_path)
    cab_path = real_path;

  trace(">> adding file entry for (%s,%s)", rp, cab_path);
  struct file_entry *new_entry = calloc(1, sizeof(struct file_entry));
  new_entry->size = file_size(rp);
  strcpy(new_entry->real_path, rp);
  strcpy(new_entry->cab_path,
         config->makenopath
         ? strippath(cab_path)
         : changepath(cab_path, '/', '\\'));

  struct sllitem *new_slli = calloc(1, sizeof(struct sllitem));
  sllitem_init(new_slli, new_entry);

  config->fileEntryItems = prepend(config->fileEntryItems, new_slli);
  free(rp);
}

/* calculate the number of datablocks we will need:
 * cabinet files are written in blocks of 32768 bytes */
int reducer_sum_size(int acc, struct sllitem *item)
{
  struct file_entry *p = item->data;

  return acc + p->size;
}

int calc_datablock_count(struct sllitem *start)
{
  int size = 0;

  size = reduce(0, start, reducer_sum_size);
  return size / DATABLOCKSIZE + 1;
}

int reducer_cab_path_16(int acc, struct sllitem *item)
{
  struct file_entry *p = item->data;

  return acc + 16 + strlen(p->cab_path) + 1;
}

int make_output_dir(struct lcab_config *config)
{
  // super basic at this point and it will proably fail in
  // anything other that the most simple case
  errno = 0;
  if (strcmp(config->output_dir, ".") == 0 || strcmp(config->output_dir, "..") == 0)
    return 0;
  mkdir(config->output_dir, 0777);
  return errno;
}

int build_cab_file_meta(struct lcab_config *config, struct cmeta *meta)
{
  int i = 0;
  long mysize = 0;
  int mysize2;
  int file_count = 0;                           /* number of files */
  int datablock_count = 0;                      /* number of datablocks */
  int offsetdata = 0;
  struct mydatablock *ptrdbs;
  struct mycfile *ptrcfs;
  struct mycfile *prevcfile;

  trace("> build_cab_file_meta");
  // BUILD the CAB FILE META DATA
  datablock_count = calc_datablock_count(config->fileEntryItems);
  trace(">> calc_datablock_count is %d", datablock_count);



  // HEADER INIT
  file_count = reduce(0, config->fileEntryItems, reducer_count);
  cheader_init(&meta->mycheader, "MSCF", 1, file_count, 0, 1234, 0);
  cheader_offsetfiles(&meta->mycheader, CFI_START);

  // calculate all the file sizes plus buffers
  int total_size = reduce(0, config->fileEntryItems, reducer_sum_size);
  mysize += total_size;               /* total file size */
  int file_name_size = reduce(0, config->fileEntryItems, reducer_cab_path_16);

  // HEADER SIZE PART 1
  mysize2 = CFI_START;                  /* first part of cfile struct = 16 bytes */
  mysize2 += file_name_size;
  mysize2 += datablock_count * 8;       /* first part cdata struct is 8 bytes */

  // FOLDER
  cfolder_init(&meta->mycfolder, datablock_count, 0);

  // FOLDER->offsetdata
  offsetdata += file_name_size;
  cfolder_offsetdata(&meta->mycfolder, CFI_START + offsetdata);

  // DATABLOCKS
  meta->mydatablocks = (struct mydatablock *)calloc(1, sizeof(struct mydatablock));
  meta->mydatablocks->next = NULL;
  ptrdbs = meta->mydatablocks;


  cdata_init(&ptrdbs->cd, 0);
  if (mysize < DATABLOCKSIZE) {
    cdata_ncbytes(&ptrdbs->cd, mysize);
    cdata_nubytes(&ptrdbs->cd, mysize);
    mysize2 += mysize;                                                  /* header part 2 */
  } else {
    for (i = 0; i < datablock_count; ++i) {
      if (i != datablock_count - 1) {
        cdata_ncbytes(&ptrdbs->cd, DATABLOCKSIZE);
        cdata_nubytes(&ptrdbs->cd, DATABLOCKSIZE);
        mysize2 += DATABLOCKSIZE;                                 /* header part 2 */

        ptrdbs->next = (struct mydatablock *)calloc(1, sizeof(struct mydatablock));
        ptrdbs = ptrdbs->next;
        ptrdbs->next = NULL;
        cdata_init(&ptrdbs->cd, 0);
      } else {
        mysize -= DATABLOCKSIZE * (datablock_count - 1);
        cdata_ncbytes(&ptrdbs->cd, mysize);
        cdata_nubytes(&ptrdbs->cd, mysize);
        mysize2 += mysize;                                      /* header part 2 */
      }
    }
  }


  // HEADER SIZE PART 2
  cheader_size(&meta->mycheader, mysize2);

  mysize += reduce(0, config->fileEntryItems, reducer_sum_size);
  debug("cabfile         : %d bytes (approx. %.2f Kbytes)", (int)mysize, (float)mysize / 1024);

  // FILES
  meta->mycfiles = (struct mycfile *)calloc(1, sizeof(struct mycfile));
  meta->mycfiles->next = NULL;
  ptrcfs = meta->mycfiles;

  struct sllitem *p_item = config->fileEntryItems;
  struct file_entry *p, *prev_entry;
  i = 0;
  while (p_item) {
    p = p_item->data;
    cfile_init(&ptrcfs->cf, p->size, 0, p->cab_path, p->real_path);

    // offset 1st cfile = 0, 2nd = offset 1st + filesize 1st, etc..
    if (i == 0)
      cfile_uoffset(&ptrcfs->cf, 0);
    else
      cfile_uoffset(&ptrcfs->cf, prevcfile->cf.uoffset + prev_entry->size);

    if (p_item->next) {
      prev_entry = p;
      prevcfile = ptrcfs;
      ptrcfs->next = (struct mycfile *)calloc(1, sizeof(struct mycfile));
      prevcfile = ptrcfs;
      ptrcfs = ptrcfs->next;
      ptrcfs->next = NULL;
    }
    p_item = p_item->next;
    i++;
  }

  return 0;
}

void lcab_defaults(struct lcab_config *config)
{
  if (strlen(config->temp_file) == 0) strcpy(config->temp_file, DEFAULT_TEMP_FILE);
  if (strlen(config->output_file) == 0) strcpy(config->output_file, DEFAULT_OUTPUT_CAB);
  if (strlen(config->output_dir) == 0) strcpy(config->output_dir, DEFAULT_OUTPUT_DIR);
}

int write_cab_file(struct lcab_config *config)
{
  FILE *fp, *fptmp;
  char outputpath[MAXPATHLEN];
  long pos = 0;
  struct cmeta meta;
  struct mydatablock *ptrdbs;
  struct mycfile *ptrcfs;

  lcab_defaults(config);

  build_cab_file_meta(config, &meta);
  make_output_dir(config);
  write_temp_file(config);

  fptmp = fopen(config->temp_file, "rb");
  if (!fptmp) {
    error("error: could not open %s for writing", config->temp_file);
    return EXIT_FAILURE;
  }

  // WRITE TO ACTUAL FILE
  // TODO - take output_dir into consideration
  cab_output_path(config, outputpath);
  fp = fopen(outputpath, "wb");
  if (!fp) {
    error("error: could not open %s for writing", config->output_file);
    remove_temp_file(config);
    return EXIT_FAILURE;
  }
  if (cheaderwrite(&meta.mycheader, fp)) {
    error("couldn't write header");
    fclose(fp);
    return EXIT_FAILURE;
  }

  if (cfolderwrite(&meta.mycfolder, fp)) {
    error("couldn't write folder");
    fclose(fp);
    return EXIT_FAILURE;
  }

  ptrcfs = meta.mycfiles;
  while (ptrcfs) {
    if (cfilewrite(&ptrcfs->cf, fp)) {
      error("couldn't write file");
      fclose(fp);
      return EXIT_FAILURE;
    }
    ptrcfs = ptrcfs->next;
  }


  ptrdbs = meta.mydatablocks;
  pos = cdatawrite(&ptrdbs->cd, fp, pos, fptmp);
  if (pos == -1) {
    error("error: could not write datablock at pos: %ld", pos);
    fclose(fptmp);
    fclose(fp);
    remove_temp_file(config);
  }
  while (ptrdbs->next != NULL) {
    ptrdbs = ptrdbs->next;
    pos = cdatawrite(&ptrdbs->cd, fp, pos, fptmp);
    if (pos == -1) {
      error("error: could not write datablock at pos: %ld", pos);
      fclose(fptmp);
      fclose(fp);
      remove_temp_file(config);
    }
  }

  fclose(fptmp);
  fclose(fp);
  return 0;
}


void cleanup(struct lcab_config *config)
{
  // cleanup
  remove_temp_file(config);
  cleanup_config(config);
}

int remove_temp_file(struct lcab_config *config)
{
  if (!exists(config->temp_file))
    return 0;
  int ret = remove(config->temp_file);
  if (ret) {
    error("could not remove %s", config->temp_file);
    return ret;
  }
  return 0;
}

void cleanup_config(struct lcab_config *config)
{
  trace("> cleaning up");
  struct sllitem *p_item = config->fileEntryItems;
  struct sllitem *next;

  while (p_item) {
    next = p_item->next;
    free(p_item->data);
    free(p_item);
    p_item = next;
  }
}

/**
 * @param ctx FILE*
 */
int append_entry_to_file(struct sllitem *item, void *ctx)
{
  byte *buffer;
  struct file_entry *entry;
  FILE *fp;
  int sz;

  entry = item->data;

  fp = fopen(entry->real_path, "rb");
  if (!fp)
    return errno;
  sz = entry->size;
  buffer = (byte *)calloc(sz, sizeof(byte));
  fread(buffer, sz, 1, fp);
  fwrite(buffer, sz, 1, (FILE *)ctx);
  free(buffer);
  fclose(fp);
  return 0;
}

int write_temp_file(struct lcab_config *config)
{
  // WRITE TEMP FILE
  errno = 0;
  FILE *fptmp = fopen(config->temp_file, "wb");
  if (!fptmp) {
    error("error: could not open lcab.tmp for writing");
    return errno;
  } else if (for_each(config->fileEntryItems, append_entry_to_file, fptmp)) {
    error("couldn't write temp file");
    return EXIT_FAILURE;
  }
  fclose(fptmp);
  return errno;
}


void cab_output_path(struct lcab_config *config, char *p)
{
  strcpy(p, config->output_dir);
  strcpy(p + strlen(p), "/");
  strcpy(p + strlen(p), config->output_file);
}
