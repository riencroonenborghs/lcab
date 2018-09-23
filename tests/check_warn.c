#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "../src/warn.h"


char *bout;
char *berr;

FILE *out;
FILE *err;

void setup()
{
	errno = 0;
	set_log_level(TRACE);
	bout = calloc(1024, sizeof(char));
	berr = calloc(1024, sizeof(char));

	out = fmemopen(bout, 1024, "a");
	err = fmemopen(berr, 1024, "a");
	init_logging(out, err);
}

void teardown()
{
	fclose(out);
	fclose(err);
	free(bout);
	free(berr);
}

START_TEST(test_trace){
	trace("Test %s", "trever");
	ck_assert_str_eq(bout, "trace Test trever\n");
}
END_TEST

START_TEST(test_debug)
{
	debug("Test %s", "trever");
	ck_assert_str_eq(bout, "debug Test trever\n");
}
END_TEST

START_TEST(test_info)
{
	info("Test %s", "trever");
	ck_assert_str_eq(bout, " info Test trever\n");
}
END_TEST

START_TEST(test_warn)
{
	warn("Test %s", "trever");
	ck_assert_str_eq(berr, " warn Test trever\n");
}
END_TEST

START_TEST(test_error)
{
	error("Test %s", "trever");
	ck_assert_str_eq(berr, "error Test trever\n");
}
END_TEST

START_TEST(test_fatal)
{
	fatal("Test %s", "trever");
	ck_assert_str_eq(berr, "fatal Test trever\n");
}
END_TEST


START_TEST(test_warn2)
{
	set_log_level(WARN);
	warn("Test %s", "trever");
	ck_assert_str_eq(berr, "Warning: Test trever\n");
}
END_TEST

START_TEST(test_error2)
{
	set_log_level(WARN);
	error("Test %s", "trever");
	ck_assert_str_eq(berr, "Error: Test trever\n");
}
END_TEST

START_TEST(test_fatal2)
{
	set_log_level(WARN);
	fatal("Test %s", "trever");
	ck_assert_str_eq(berr, "Fatal: Test trever\n");
}
END_TEST


START_TEST(test_levels)
{
	set_log_level(INFO);
	debug("This is Debug");
	info("This is Info");
	ck_assert_str_eq(bout, "This is Info\n");
	ck_assert_str_eq(berr, "");
}
END_TEST


START_TEST(test_errno)
{
	set_log_level(WARN);
	errno = EDQUOT;
	warn("This is Warn");


	char buffer[256];
	sprintf(buffer, "Warning: This is Warn : %s\n", strerror(EDQUOT));

	ck_assert_str_eq(berr, buffer);
	errno = 0;
}
END_TEST

Suite *make_warn_suite(void)
{
	Suite *s = suite_create("func");

	TCase *tc_warn = tcase_create("warn");

	tcase_add_checked_fixture(tc_warn, setup, teardown);

	suite_add_tcase(s, tc_warn);
	{
		tcase_add_test(tc_warn, test_trace);
		tcase_add_test(tc_warn, test_debug);
		tcase_add_test(tc_warn, test_info);
		tcase_add_test(tc_warn, test_warn);
		tcase_add_test(tc_warn, test_error);
		tcase_add_test(tc_warn, test_fatal);
		tcase_add_test(tc_warn, test_warn2);
		tcase_add_test(tc_warn, test_error2);
		tcase_add_test(tc_warn, test_fatal2);
		tcase_add_test(tc_warn, test_levels);
		tcase_add_test(tc_warn, test_errno);
	}

	return s;
}

int main(void)
{
	int failed;
	Suite *s = make_warn_suite();
	SRunner *sr = srunner_create(s);

	srunner_set_tap(sr, __FILE__ ".tap");
	srunner_run_all(sr, CK_VERBOSE);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed ? 1 : 0;
}
