#ifndef __REGISTER_H__
#define __REGISTER_H__

#ifdef __cplusplus
extern "C" {
#endif 


//define ERROR CODE
//#define ERROR_OPEN_FILE_FAILED	-1

enum reg_type {
	VOID,
	STRING,
	INT,
	LONG,
	UINT,
	ULONG,
	FLOAT,
	DOUBLE,

	ERROR_TYPE
};

#define KEY_LEN		24
#define VALUE_LEN	24
#define COMMENTS_LEN	128
#define TYPE_LEN	24

#define REGISTER_FULLPATH_FILE_NAME "~/register.txt"
#define MAX_LAYERS_OF_PATH	10
#define MAX_LENGTH_OF_KEYWORD	(KEY_LEN)
#define MAX_LENGTH_OF_PATH	(MAX_LAYERS_OF_PATH * MAX_LENGTH_OF_KEYWORD)
#define SEPARATOR	'/'

typedef struct KEY_STU {
	int level;
	enum reg_type type;
	char key[KEY_LEN];
	char value[VALUE_LEN];
	char comments[COMMENTS_LEN];
} KEY;

extern int reg_create_key(const char * fullname, enum reg_type type, const void * value, const char * comments);
extern int reg_delete_key(const char * fullname);
extern int reg_write_key(const char * fullname, enum reg_type type, const void * value, const char * comments);
extern int reg_read_key(const char * fullname, enum reg_type type, void * value, char * comments);
extern float reg_get_version(void);


#ifdef __cplusplus
}
#endif 

#endif

