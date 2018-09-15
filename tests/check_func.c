#include <check.h>
#include <stdlib.h>
#include "../src/func.h"


struct sllitem item1, item2, item3, item4;
struct sllitem *head;

void setup()
{
	head = NULL;

	sllitem_init(&item1, "1");
	sllitem_init(&item2, "2");
	sllitem_init(&item3, "3");
	sllitem_init(&item4, "4");

	append(&item1, &item2);
	append(&item1, &item3);
	append(&item1, &item4);
}

void teardown()
{
}

START_TEST(test_reduce_null_rets_initial){
	int actual = reduce(-7, NULL, reducer_count);

	ck_assert_int_eq(actual, -7);
}
END_TEST

START_TEST(test_reduce_fnnull_rets_initial)
{
	int actual = reduce(-7, &item1, NULL);

	ck_assert_int_eq(actual, -7);
}
END_TEST

START_TEST(test_count_0)
{
	int ct = reduce(0, NULL, reducer_count);

	ck_assert_int_eq(0, ct);
}
END_TEST


START_TEST(test_count)
{
	int ct = reduce(0, &item1, reducer_count);

	ck_assert_int_eq(4, ct);
}
END_TEST


START_TEST(test_for_each_no_fn)
{
	struct sllitem item1;

	sllitem_init(&item1, "trever");
	int ret = for_each(&item1, NULL, NULL);
	ck_assert_int_eq(0, ret);
}
END_TEST


int test_for_each_retval_fn(struct sllitem *value, void *ctx)
{
	return 7;
}
START_TEST(test_for_each_retval){
	sllitem_init(&item1, "trever");
	int ret = for_each(&item1, test_for_each_retval_fn, NULL);
	ck_assert_int_eq(7, 7);
}
END_TEST


START_TEST(test_for_each_empty)
{
	for_each(NULL, NULL, NULL);// throws no errors
}
END_TEST


START_TEST(test_sllitem_init)
{
	struct sllitem item;

	sllitem_init(&item, "trever");
	ck_assert_ptr_eq(item.next, NULL);
	ck_assert_str_eq(item.data, "trever");
}
END_TEST

START_TEST(test_prepend_null)
{
	head = prepend(NULL, &item1);
	ck_assert_ptr_eq(head, &item1);

	head = prepend(&item1, NULL);
	ck_assert_ptr_eq(head, &item1);
}
END_TEST

START_TEST(test_prepend)
{
	head = prepend(&item1, &item2);
	ck_assert_ptr_eq(head, &item2);
}
END_TEST

START_TEST(test_append_null)
{
	head = append(NULL, &item1);
	ck_assert_ptr_eq(head, &item1);

	head = append(&item1, NULL);
	ck_assert_ptr_eq(head, &item1);
}
END_TEST

START_TEST(test_append)
{
	head = append(&item1, &item2);
	ck_assert_ptr_eq(head, &item1);
}
END_TEST

Suite *make_func_suite(void)
{
	Suite *s = suite_create("func");

	TCase *tc_sllitem_init = tcase_create("for_each");

	suite_add_tcase(s, tc_sllitem_init);
	{
		tcase_add_test(tc_sllitem_init, test_sllitem_init);
	}


	TCase *tc_for_each = tcase_create("for_each");
	suite_add_tcase(s, tc_for_each);
	{
		tcase_add_test(tc_for_each, test_for_each_retval);
		tcase_add_test(tc_for_each, test_for_each_empty);
		tcase_add_test(tc_for_each, test_for_each_no_fn);
	}

	TCase *tc_reducer_count = tcase_create("reducer_count");
	suite_add_tcase(s, tc_reducer_count);
	tcase_add_checked_fixture(tc_reducer_count, setup, teardown);
	{
		tcase_add_test(tc_reducer_count, test_count);
		tcase_add_test(tc_reducer_count, test_count_0);
	}

	TCase *tc_append = tcase_create("append");
	suite_add_tcase(s, tc_append);
	tcase_add_checked_fixture(tc_append, setup, teardown);
	{
		tcase_add_test(tc_append, test_append_null);
		tcase_add_test(tc_append, test_append);
	}

	TCase *tc_prepend = tcase_create("prepend");
	suite_add_tcase(s, tc_prepend);
	tcase_add_checked_fixture(tc_prepend, setup, teardown);
	{
		tcase_add_test(tc_prepend, test_prepend_null);
		tcase_add_test(tc_prepend, test_prepend);
	}

	TCase *tc_reduce = tcase_create("reduce");
	suite_add_tcase(s, tc_reduce);
	tcase_add_checked_fixture(tc_reduce, setup, teardown);
	{
		tcase_add_test(tc_reduce, test_reduce_null_rets_initial);
		tcase_add_test(tc_reduce, test_reduce_fnnull_rets_initial);
	}

	return s;
}

int main(void)
{
	int failed;
	Suite *s = make_func_suite();
	SRunner *sr = srunner_create(s);

	srunner_set_tap(sr, __FILE__ ".tap");
	srunner_run_all(sr, CK_VERBOSE);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed ? 1 : 0;
}
