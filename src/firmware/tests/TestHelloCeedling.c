#include <unity.h>

TEST_FILE("HelloCeedling.c")

extern int hello_ceedling(void);

void setUp(void)
{
}

void tearDown(void)
{
}

void test_hello_ceedling(void)
{
	TEST_ASSERT_EQUAL(123, hello_ceedling());
}
