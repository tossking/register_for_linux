#include "stdio.h"
#include "register.h"


static char comment[128];

//test string and Delete
void test1(void)
{
	const char * fullname = "/system/1/2/3/4/5/6/6.A";
	const char * fullname2 = "/system/1/2/3/4";
	const char * value = "This is a test!";
	const char * comments = "Test string!";
	int i;
	char value3[24];
	int ret = 0;

	ret |= reg_create_key(fullname, STRING, value, comments);
	reg_read_key(fullname, STRING, value3, comment);
	printf("[%s] [%s] [%s]\n", fullname, value3, comment);
	if (strcmp(value3, value)) {
		ret = -1;
	}
	ret |= reg_delete_key(fullname2);
	
	if (ret) {
		printf("test case 1 failed\n");
	}
}

//test + - double 
void test2(void)
{
	const char * test2_name1 = "/user/testcase2/double";
	const char * test2_name2 = "/user/testcase2/negative double";
	const char * comments1 = "Test double type!";
	const char * comments2 = "Test double type 2!";
	double value = 123.456789;
	double value2 = -98765.4321;
	double value3;
	int ret = 0;

	reg_create_key(test2_name1, DOUBLE, &value, comments1);
	reg_create_key(test2_name2, DOUBLE, &value2, comments2);

	reg_read_key(test2_name1, DOUBLE, &value3, comment);
	printf("[%s] [%f] [%s]\n", test2_name1, value3, comment);
	if (value3 != value) {
		ret = -1;
	}
	reg_read_key(test2_name2, DOUBLE, &value3, comment);
	printf("[%s] [%f] [%s]\n", test2_name2, value3, comment);
	if (value3 != value2) {
		ret = -1;
	}

	if (ret) {
		printf("test case 2 failed\n");
	}
}

//test + - int
void test3(void)
{
	const char * test3_name1 = "/user/testcase3/int";
	const char * test3_name2 = "/user/testcase3/negative int";
	const char * comments1 = "Test int type!";
	const char * comments2 = "Test int type 2!";
	int value = 123456789;
	int value2 = -987654321;
	int value3;
	int ret = 0;
	
	reg_create_key(test3_name1, INT, &value, comments1);
	reg_create_key(test3_name2, INT, &value2, comments2);

	reg_read_key(test3_name1, INT, &value3, comment);
	printf("[%s] [%d] [%s]\n", test3_name1, value3, comment);
	if (value3 != value) {
		ret = -1;
	}
	reg_read_key(test3_name2, INT, &value3, comment);
	printf("[%s] [%d] [%s]\n", test3_name2, value3, comment);
	if (value3 != value2) {
		ret = -1;
	}

	if (ret) {
		printf("test case 3 failed\n");
	}
}

//test uint ulong
void test4(void)
{
	const char * test3_name1 = "/user/testcase4/unsigned int";
	const char * test3_name2 = "/user/testcase4/unsigned long";
	const char * comments1 = "Test unsigned int type!";
	const char * comments2 = "Test unsigned long type !";
	unsigned int value = 12345678;
	unsigned long value2 = 1234567890;
	unsigned long value3;
	int ret = 0;
	
	reg_create_key(test3_name1, UINT, &value, comments1);
	reg_create_key(test3_name2, ULONG, &value2, comments2);

	reg_read_key(test3_name1, UINT, &value3, comment);
	printf("[%s] [%u] [%s]\n", test3_name1, value3, comment);
	if (value3 != value) {
		ret = -1;
	}
	reg_read_key(test3_name2, ULONG, &value3, comment);
	printf("[%s] [%lu] [%s]\n", test3_name2, value3, comment);
	if (value3 != value2) {
		ret = -1;
	}

	if (ret) {
		printf("test case 4 failed\n");
	}
}

//test + - long
void test5(void)
{
	const char * test3_name1 = "/user/testcase5/long";
	const char * test3_name2 = "/user/testcase5/negative long";
	const char * comments1 = "Test long type!";
	const char * comments2 = "Test long type 2!";
	long value = 123456;
	long value2 = -1;
	long value3;
	int ret = 0;
	
	reg_create_key(test3_name1, LONG, &value, comments1);
	reg_create_key(test3_name2, LONG, &value2, comments2);

	reg_read_key(test3_name1, LONG, &value3, comment);
	printf("[%s] [%ld] [%s]\n", test3_name1, value3, comment);
	if (value3 != value) {
		ret = -1;
	}
	reg_read_key(test3_name2, LONG, &value3, comment);
	printf("[%s] [%ld] [%s]\n", test3_name2, value3, comment);
	if (value3 != value2) {
		ret = -1;
	}

	if (ret) {
		printf("test case 5 failed\n");
	}
}

//test + - long
void test6(void)
{
	const char * test3_name1 = "/user/testcase6/float";
	const char * test3_name2 = "/user/testcase6/negative float";
	const char * comments1 = "Test float type!";
	const char * comments2 = "Test float type 2!";
	float value = 1.23;
	float value2 = -9.87;
	float value3;
	int ret = 0;
	
	reg_create_key(test3_name1, FLOAT, &value, comments1);
	reg_create_key(test3_name2, FLOAT, &value2, comments2);

	reg_read_key(test3_name1, FLOAT, &value3, comment);
	printf("[%s] [%f] [%s]\n", test3_name1, value3, comment);
	if (value3 != value) {
		ret = -1;
	}
	reg_read_key(test3_name2, FLOAT, &value3, comment);
	printf("[%s] [%f] [%s]\n", test3_name2, value3, comment);
	if (value3 != value2) {
		ret = -1;
	}

	if (ret) {
		printf("test case 6 failed\n");
	}
}

//test reg_get_version
void test7(void)
{
	printf("version = %f\n" ,reg_get_version());
}

int main(void)
{
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
}


