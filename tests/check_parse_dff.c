#include <check.h>
#include <stdlib.h>
#include <ftw.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include "lcab_config.h"
#include "parse_dff.h"
#include "lcab.h"
#include "warn.h"
#include "util.h"
#include "func.h"
#include "file_entry.h"

struct lcab_config *config;

/**
 * create a local directory setup with files that reflects
 * testcab.cab.  We even set the timestamps on the files.
 */
void setup()
{
  config = malloc(sizeof(struct lcab_config));
  lcab_defaults(config);
  config->log_level = DEBUG;
  strcpy(config->diamond_file, "test.dff");
  int r = parse_dff(config, add_input_file);
  ck_assert_int_eq(0, r);


  lcabt_mkdir("dir1");

  lcabt_write_file("rootfile1.txt", "root file 1");
  lcabt_write_file("rootfile2.txt", "root file 1");
  lcabt_write_file("dir1/dir1file1.txt", "file 1 in dir 1");
  lcabt_write_file("dir1/dir1file2.txt", "file 2 in dir 1");
}

void teardown()
{
  free(config);
  // lcabt_rmrf("dir1");
  // lcabt_rm("rootfile1.txt");
  // lcabt_rm("rootfile2.txt");
}

START_TEST(test_supports_cab_name){
  ck_assert_str_eq(config->output_file, "testdff.cab");
}
END_TEST

START_TEST(test_supports_dest_dir)
{
  ck_assert_str_eq(config->output_dir, "testdff-output");
}
END_TEST

START_TEST(test_reads_files)
{
  struct sllitem *item;
  struct file_entry *fe;

  fe = nth(config->fileEntryItems, 0)->data;
  ck_assert_str_eq(fe->real_path, "rootfile2.txt");
  ck_assert_str_eq(fe->cab_path, "dir2\\rootfile2.txt");

  fe = nth(config->fileEntryItems, 1)->data;
  ck_assert_str_eq(fe->real_path, "dir1/dir1file2.txt");
  ck_assert_str_eq(fe->cab_path, "dir1\\dir1file2.txt");

  fe = nth(config->fileEntryItems, 2)->data;
  ck_assert_str_eq(fe->real_path, "dir1/dir1file1.txt");
  ck_assert_str_eq(fe->cab_path, "dir1\\dir1file1.txt");

  fe = nth(config->fileEntryItems, 3)->data;
  ck_assert_str_eq(fe->real_path, "rootfile1.txt");
  ck_assert_str_eq(fe->cab_path, "rootfile1.txt");
}
END_TEST


Suite *make_suite(void)
{
  Suite *s = suite_create("parse_dff");
  TCase *tc = tcase_create("parse_dff");

  tcase_add_checked_fixture(tc, setup, teardown);
  suite_add_tcase(s, tc);
  tcase_add_test(tc, test_supports_cab_name);
  tcase_add_test(tc, test_supports_dest_dir);
  tcase_add_test(tc, test_reads_files);
  return s;
}


int main(void)
{
  int failed;
  Suite *s = make_suite();
  SRunner *sr = srunner_create(s);

  srunner_set_tap(sr, __FILE__ ".tap");
  srunner_run_all(sr, CK_VERBOSE);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return failed ? 1 : 0;
}
