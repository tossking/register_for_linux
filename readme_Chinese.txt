注册表说明

A.注册表的结构
注册表文件是一个纯文本文件，在了解其结构的情况下完全可以手工编辑。
第一行为版本号，程序操作版本不同的注册表文件时可能出错，版本号是为了防止两者不匹配的情况出现。
每一个注册表项佔一行，每项最多有4个字段，分别用"[]"括起来，
4个字段依次为[name] [type] [value] [comments],后3个字段可以省略。
每一个注册表项可以有多个下属子项组成树形结构，每个子项比他的父项右移一个TAB以便观察。

目前支持的数据类型如下 value's type(如有新类型需求可以添加)
	VOID,		空
	STRING,		string
	INT,		int
	LONG,		long
	UINT,		unsigned int
	ULONG,		unsigned long
	FLOAT,		float
	DOUBLE,		double

下面是一个真实的注册表例子：
-------------------------------------------------------------------------------------
1.00

[system]
	[1]
		[2]
			[3]
[user]
	[testcase6]
		[negative float] [FLOAT] [-9.870000] [Test float type 2!]
		[float] [FLOAT] [1.230000] [Test float type!]
	[testcase5]
		[negative long] [LONG] [-1] [Test long type 2!]
		[long] [LONG] [123456] [Test long type!]
	[testcase4]
		[unsigned long] [ULONG] [1234567890] [Test unsigned long type !]
		[unsigned int] [UINT] [12345678] [Test unsigned int type!]
	[testcase3]
		[negative int] [INT] [-987654321] [Test int type 2!]
		[int] [INT] [123456789] [Test int type!]
	[testcase2]
		[negative double] [DOUBLE] [-98765.432100] [Test double type 2!]
		[double] [DOUBLE] [123.456789] [Test double type!]
-------------------------------------------------------------------------------------

B.对外接口

1.返回当前的注册表版本号
float reg_get_version(void);

2.创建一个注册表项
int reg_create_key(const char * fullname, enum reg_type type, const void * value, const char * comments);
使用举例 创建一项名为"6.A"的注册表项，其路径为"/system/1/2/3/4/5/6/"，路径中的项如果注册表中没有，会自动建立
const char * fullname = "/system/1/2/3/4/5/6/6.A";
const char * value = "This is a test!";
const char * comments = "Test string!";
reg_create_key(fullname, STRING, value, comments);

3.删除一个注册表项
int reg_delete_key(const char * fullname);
使用举例 删除/system/1/2/3/4这项及其下属的所有子项
const char * fullname2 = "/system/1/2/3/4";
reg_delete_key(fullname2);

4.读取一个注册表项
int reg_read_key(const char * fullname, enum reg_type type, void * value, char * comments);
使用举例	
const char * test2_name1 = "/user/testcase2/double";
double value;
char comment[128];
reg_read_key(test2_name1, DOUBLE, &value, comment);

5.写入一个注册表项
int reg_write_key(const char * fullname, enum reg_type type, const void * value, const char * comments);
使用举例	
const char * test2_name1 = "/user/testcase2/double";
double value = 123.456789;
const char * comments1 = "Test double type!";
reg_write_key(test2_name1, DOUBLE, &value, comments1);
	
C.使用注意事项：
1.调用API的文件需包含register.h, 链接时需要加上-ldm320reg將register的实现库链入。
#include "register.h"
add -ldm320reg in Makefile 

2.目前注册表项的长度限制
#define KEY_LEN		24
#define VALUE_LEN	24
#define COMMENTS_LEN	128
#define MAX_LAYERS_OF_PATH	10

3.以下字符为注册表保留字，不能在KEY和COMMENTS中使用
'/'	'['	']'	'\n'

4.创建，写入或读出一项时，传入的两个参数value和comments指针可以为NULL，当value为NULL时，type必须为VOID型。
const char * fullname = "/system/1/2/3/4/5/6";
reg_create_key(fullname, VOID, NULL, NULL);
