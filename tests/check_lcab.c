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
#include "lcab.h"
#include "warn.h"
#include "util.h"


/**
 * create a local directory setup with files that reflects
 * testcab.cab.  We even set the timestamps on the files.
 */
void setup()
{
  lcabt_mkdir("testcab");
  lcabt_mkdir("testcab/dir2");
  lcabt_write_file("testcab/file2.txt", "file2\n");
  lcabt_write_file("testcab/file1.txt", "file1\n");
  lcabt_write_file("testcab/dir2/file2.txt", "dir2file2\n");
  lcabt_write_file("testcab/dir2/file1.txt", NULL);

  // setup the local files to have the same timestamp
  // as the testcab.cab files
  lcabt_update_file_time("testcab/file2.txt", 1537624364);
  lcabt_update_file_time("testcab/file1.txt", 1537624358);
  lcabt_update_file_time("testcab/dir2/file2.txt", 1537624386);
  lcabt_update_file_time("testcab/dir2/file1.txt", 1537624392);
}

void teardown()
{
  lcabt_rmrf("testcab");
  lcabt_rm(DEFAULT_OUTPUT_CAB);
}

/**
 * the entire purpose of this test is for regression.  i manually
 * created a cabinet file using lcab v1.0b11 and it's checked into
 * tests/.  Given the same inputs, we run write_cab_file and ensure
 * that the two files produced are the same.  This allows us to
 * better refactor with confidence.
 */
START_TEST(test_reproduces_cab){
  struct lcab_config config = {
    .log_level  = INFO,
  };

  lcabt_rm(DEFAULT_OUTPUT_CAB);
  add_input_file(&config, "testcab/dir2/file1.txt", NULL);
  add_input_file(&config, "testcab/dir2/file2.txt", NULL);
  add_input_file(&config, "testcab/file1.txt", NULL);
  add_input_file(&config, "testcab/file2.txt", NULL);
  write_cab_file(&config);
  cleanup(&config);

  // compare the two files.
  int files_diff = lcabt_files_differ(DEFAULT_OUTPUT_CAB, "testcab.cab");
  ck_assert_int_eq(0, files_diff);
}
END_TEST


Suite *make_suite(void)
{
  Suite *s = suite_create("regression");

  TCase *tc_comparative = tcase_create("reproduces_cab");

  tcase_add_checked_fixture(tc_comparative, setup, teardown);
  suite_add_tcase(s, tc_comparative);
  {
    tcase_add_test(tc_comparative, test_reproduces_cab);
  }
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
