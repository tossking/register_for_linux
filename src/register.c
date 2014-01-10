#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <unistd.h>
#include <sys/types.h>

#include "register.h"

#define MAX_LINE_LENGTH	256
#define FILE_EOF		-1


#define GET_ITEM(p, q, line, len, max_len, dest, ret) { \
	p = strchr(line, '[');				\
	q = strchr(line, ']');				\
	p++;						\
	len = q - p;					\
	if (NULL == p || NULL == q || len >= max_len) {	\
		return ret;				\
	}						\
	strncpy(dest, p, len);				\
	dest[len] = 0;					\
	line = q + 1;					\
}

const char * REG_TYPE_STR[] = {
	"VOID",
	"STRING",
	"INT",
	"LONG",
	"UINT",
	"ULONG",
	"FLOAT",
	"DOUBLE"
};

//define version number of register
const float current_version = 1.00;

static float reg_version;

float reg_get_version(void)
{
	return reg_version;
}

static FILE * OpenRegisterFile(void)
{
	FILE *fp;
	const char * file = REGISTER_FULLPATH_FILE_NAME;
	char line[MAX_LINE_LENGTH];

	fp = fopen(file, "r+");  //open a exist register file
	if (NULL==fp) {//register file does not exist, create new one
		fp = fopen(file, "w+");
		if (NULL==fp) {
			printf("open file error: %s\n", file);
			return NULL;
		}
		fprintf(fp, "%f\n\n\n", current_version);
		fseek(fp, 0, SEEK_SET);
	}
	memset(line , 0, sizeof(line));
	fgets(line, MAX_LINE_LENGTH, fp);
	reg_version = atof(line);
	if (current_version != reg_version) {
		printf("Register version not matched!\n");
		fclose(fp);
		return NULL;
	}

	return fp;
}

static void CloseRegisterFile(FILE *fp)
{
	fclose(fp);
}

static int GetKeyFromFullName(const char * fullname, char * key)
{
	char * p;

	p = strrchr(fullname, SEPARATOR);
	if (NULL==p) { //no separator in fullname, no pathway
		p = (char *)fullname;
	} else {
		p++;
	}
	if (0 == *(p)) {
		printf("Key Name Error!\n");
		return -1;
	}
	strcpy(key, p);
	return strlen(key);
}

static int GetPathFromFullName(const char * fullname, char * path)
{
	char * p;
	char * q = (char *)fullname;
	int layer = 0;

	if (SEPARATOR == *q)
		q++;
	p = strchr(q, SEPARATOR);
	while (NULL != p) {
		strncpy(path + (layer * MAX_LENGTH_OF_KEYWORD), q, p - q);
		layer++;
		q = p+1;
		p = strchr(q, SEPARATOR);
	}

	return layer;
}

static int GetType(char * type)
{
	enum reg_type i;

	for (i = VOID; i < ERROR_TYPE; i++) {
		if (!strcmp(type, REG_TYPE_STR[i]))
			break;
	}
	return i;
}

static int GetLevelFromLine(char * line)
{
	int level = 0;

	//trim tab, caclute levels 
	while('\t' == *line) {
		line++;
		level++;
	}
	
	return level;
}

//get all items from one line
static int GetItemFromLine(KEY * item, char * line)
{
	char * p;
	char * q;
	char type[TYPE_LEN];
	int len;

	item->level = GetLevelFromLine(line);	
	item->key[0] = 0;
	item->type = VOID;
	item->value[0] = 0;
	item->comments[0] = 0;

	GET_ITEM(p, q, line, len, KEY_LEN, item->key, -1);
	GET_ITEM(p, q, line, len, TYPE_LEN, type, 1);
	item->type = GetType(type);
	GET_ITEM(p, q, line, len, VALUE_LEN, item->value, 2);
	GET_ITEM(p, q, line, len, COMMENTS_LEN, item->comments, 3);

	return 0;
}

static int DeleteLine(FILE *fp)
{
	fpos_t pos_w, pos_r;
	char line[MAX_LINE_LENGTH];
	int i, ret = 0;
	long l1, l2, len;
	char * p = line;
	char * p1;

	fgetpos(fp, &pos_w);
	fgets(line, MAX_LINE_LENGTH, fp);
	fgetpos(fp, &pos_r);

	l1 = ftell(fp);
	fseek(fp, 0, SEEK_END);
	l2 = ftell(fp);
	len = l2 - l1;
	if (len > 0) {
		p = malloc(len);
		if (NULL == p) {
			printf("REGISTER ERROR : NOT ENOUGH MEMORY!\n");
			return -1;
		}
		fsetpos(fp, &pos_r);
		i = fread(p, len, 1, fp);
		fsetpos(fp, &pos_w);
		fwrite(p, len, 1, fp);
		free(p);
	} else if (0 == len) {
		fsetpos(fp, &pos_w);
		ret = FILE_EOF;
	}
	

	len = ftell(fp);

	truncate(REGISTER_FULLPATH_FILE_NAME, len);
	fsetpos(fp, &pos_w);
/*	fgetpos(fp, &pos_r);
	do {
		fsetpos(fp, &pos_r);
		i = fread(line, MAX_LINE_LENGTH, 1, fp);
		fgetpos(fp, &pos_r);
		fsetpos(fp, &pos_w);
		fwrite(line, i, 1, fp);
		fgetpos(fp, &pos_w);
	}while (i == MAX_LINE_LENGTH && !feof(fp));
*/

	return ret;
}

static int InsertLine(FILE * fp, KEY * item)
{
	fpos_t pos_r, pos_w, pos_i;
	char line[MAX_LINE_LENGTH];
	int i;
	long l1, l2, len;
	char * p = line;
	char * p1;

	//init newline
	memset(line, 0, sizeof(line)); 
	for (i = 0; i < item->level; i++) {
		*p++ = '\t';
	}
	*p++ = '[';
	strcpy(p, item->key);
	p += strlen(item->key);
	*p++ = ']';
	if (item->type > VOID && item->type < ERROR_TYPE) {
		*p++ = ' ';
		*p++ = '[';
		strcpy(p, REG_TYPE_STR[item->type]);
		p += strlen(REG_TYPE_STR[item->type]);
		*p++ = ']';
		*p++ = ' ';
		*p++ = '[';
		strcpy(p, item->value);
		p += strlen(item->value);
		*p++ = ']';
		*p++ = ' ';
		*p++ = '[';
		strcpy(p, item->comments);
		p += strlen(item->comments);
		*p++ = ']';
	}
	*p++ = '\n';
	*p++ = '\0';

	//insert newline
	fgetpos(fp, &pos_i);
	l1 = ftell(fp);
	fseek(fp, 0, SEEK_END);
	l2 = ftell(fp);
	len = l2 - l1;
	p = malloc(len);
	if (NULL == p) {
		printf("REGISTER ERROR : NOT ENOUGH MEMORY!\n");
		return -1;
	}
	fsetpos(fp, &pos_i);
	i = fread(p, len, 1, fp);
	fsetpos(fp, &pos_i);
	fwrite(line, strlen(line), 1, fp);
	fgetpos(fp, &pos_i);//file pointer set to newline end
	fwrite(p, len, 1, fp);
	fsetpos(fp, &pos_i);
	free(p);

	return 0;
}

static int CreateKey(const char * fullname, enum reg_type type, const char * value, const char * comments)
{
	FILE *fp;
	fpos_t pos, ptmp;
	char line[MAX_LINE_LENGTH];
	char key[MAX_LENGTH_OF_KEYWORD];
	char path[MAX_LAYERS_OF_PATH][MAX_LENGTH_OF_KEYWORD];
	int layer, key_len, level, i;
	KEY item;

	if (NULL == (fp = OpenRegisterFile())) {
		return -1;
	}

	memset(path, 0, MAX_LAYERS_OF_PATH * MAX_LENGTH_OF_KEYWORD);
	layer = GetPathFromFullName(fullname, (char *)path);
	key_len = GetKeyFromFullName(fullname, key);
	strcpy(path[layer], key);

	fgetpos(fp, &pos);

	i = 0;
	level = 0;
	while (i <= layer && i <= level && !feof(fp)) {
		fgetpos(fp, &ptmp);
		fgets(line, MAX_LINE_LENGTH, fp);
		level = GetLevelFromLine(line);
		if (i == level) {
			GetItemFromLine(&item, line);
			if (!strcmp(item.key, path[i])) {
				i++;
				level++;
				memcpy((char *)(&pos), (char *)(&ptmp), sizeof(pos));
			}
		}
	}
	fsetpos(fp, &pos);
	
	if (i == (layer+1)) {
		i--;
		DeleteLine(fp);
	} else {
		fgets(line, MAX_LINE_LENGTH, fp);
	}

	while (i < layer) {
		item.level = i;
		item.type = VOID;
		strcpy(item.key, path[i]);
		InsertLine(fp, &item);
		i++;
	}
	
	if (i == layer) {
		item.level = i;
		item.type = type;
		strncpy(item.key, key, key_len+1);
		strncpy(item.value, value, VALUE_LEN);
		strncpy(item.comments, comments, COMMENTS_LEN);
		InsertLine(fp, &item);
	}

	CloseRegisterFile(fp); 
	return 0;
}

static int DeleteKey(const char * fullname)
{
	FILE *fp;
	fpos_t pos, ptmp;
	char line[MAX_LINE_LENGTH];
	char key[MAX_LENGTH_OF_KEYWORD];
	char path[MAX_LAYERS_OF_PATH][MAX_LENGTH_OF_KEYWORD];
	int layer, key_len, level, i, ret = 0;
	KEY item;

	if (NULL == (fp = OpenRegisterFile())) {
		return -1;
	}

	memset(path, 0, MAX_LAYERS_OF_PATH * MAX_LENGTH_OF_KEYWORD);
	layer = GetPathFromFullName(fullname, (char *)path);
	key_len = GetKeyFromFullName(fullname, key);
	strcpy(path[layer], key);

	fgetpos(fp, &pos);

	i = 0;
	level = 0;
	while (i <= layer && i <= level && !feof(fp)) {
		fgetpos(fp, &ptmp);
		fgets(line, MAX_LINE_LENGTH, fp);
		level = GetLevelFromLine(line);
		if (i == level) {
			GetItemFromLine(&item, line);
			if (!strcmp(item.key, path[i])) {
				i++;
				level++;
				memcpy((char *)(&pos), (char *)(&ptmp), sizeof(pos));
			}
		}
	}
	fsetpos(fp, &pos);
	
	if (i == (layer+1)) {
		do {
			if (FILE_EOF == DeleteLine(fp))
				break;
			fgetpos(fp, &ptmp);
			fgets(line, MAX_LINE_LENGTH, fp);
			level = GetLevelFromLine(line);
			fsetpos(fp, &ptmp);
		} while(level >= i);
	} else {
		printf("Can Find the key to be deleted!\n");
		ret = -1;
	}
	
	CloseRegisterFile(fp); 
	return ret;
}

static int WriteKey(const char * fullname, enum reg_type type, const char * value, const char * comments)
{
	return CreateKey(fullname, type, value, comments);
}

static int ReadKey(const char * fullname, KEY *destKey)
{
	FILE *fp;
	fpos_t pos, ptmp;
	char line[MAX_LINE_LENGTH];
	char key[MAX_LENGTH_OF_KEYWORD];
	char path[MAX_LAYERS_OF_PATH][MAX_LENGTH_OF_KEYWORD];
	int layer, key_len, level, i, ret = 0;
	KEY item;

	if (NULL == (fp = OpenRegisterFile())) {
		return -1;
	}

	memset(path, 0, MAX_LAYERS_OF_PATH * MAX_LENGTH_OF_KEYWORD);
	layer = GetPathFromFullName(fullname, (char *)path);
	key_len = GetKeyFromFullName(fullname, key);
	strcpy(path[layer], key);

	fgetpos(fp, &pos);

	i = 0;
	level = 0;
	while (i <= layer && i <= level && !feof(fp)) {
		fgetpos(fp, &ptmp);
		fgets(line, MAX_LINE_LENGTH, fp);
		level = GetLevelFromLine(line);
		if (i == level) {
			GetItemFromLine(&item, line);
			if (!strcmp(item.key, path[i])) {
				i++;
				level++;
				memcpy((char *)(&pos), (char *)(&ptmp), sizeof(pos));
			}
		}
	}
	fsetpos(fp, &pos);
	
	if (i == (layer+1)) {
		fgets(line, MAX_LINE_LENGTH, fp);
		GetItemFromLine(destKey, line);
	} else {
		printf("Can Find the key to be readed!\n");
		ret = -1;
	}
	
	CloseRegisterFile(fp); 
	return ret;
}

int reg_create_key(const char * fullname, enum reg_type type, const void * value, const char * comments)
{
	static char val[KEY_LEN];
	static char com[COMMENTS_LEN];
	char *p = val;
	char *s = (char *)comments;

	if (NULL == fullname) {
		return -1;
	}

	if (NULL == value && VOID != type) {
		return -2;
	}

	if (NULL == comments) {
		s = com;
	}

	memset(val, 0, sizeof(val));
//	memset(com, 0, sizeof(com));
	
	switch(type) {
		case VOID:
			break;
		case STRING: 
			p = (char *)value;
			break;
		case INT:
			sprintf(p, "%d", *(int *)value);
			break;
		case LONG: 
			sprintf(p, "%ld", *(long *)value);
			break;
		case UINT: 
			sprintf(p, "%u", *(unsigned int *)value);
			break;
		case ULONG: 
			sprintf(p, "%lu", *(unsigned long *)value);
			break;
		case FLOAT: 
			sprintf(p, "%f", *(float *)value);
			break;
		case DOUBLE: 
			sprintf(p, "%lf", *(double *)value);
			break;
		default:
			printf("Create REGISTER type error!\n");
			return -3;
	}

	return CreateKey(fullname, type, p, s);
}

int reg_delete_key(const char * fullname)
{
	if (NULL == fullname)
		return -1;
	return DeleteKey(fullname);
}

int reg_write_key(const char * fullname, enum reg_type type, const void * value, const char * comments)
{
	return reg_create_key(fullname, type, value, comments);
}

//user who call this function must assure that the variable who "value" pointed his type is samed as "type"
int reg_read_key(const char * fullname, enum reg_type type, void * value, char * comments)
{
	KEY key;

	if (NULL == fullname) {
		return -1;
	}
	if (NULL == value && VOID != type) {
		return -2;
	}

	if ( 0 != ReadKey(fullname, &key)) {
		return -3;
	}
	if (key.type != type) {
		return -4;
	}
	
	if (0 != key.value[0]) {
		switch(type) {
			case VOID:
				break;
			case STRING: 
				strncpy((char *)value, key.value, KEY_LEN);
				break;
			case INT:
				*(int *)value = atoi(key.value);
				break;
			case LONG: 
				*(long *)value = atol(key.value);
				break;
			case UINT: 
				*(unsigned int *)value = atoi(key.value);
				break;
			case ULONG: 
				*(unsigned long *)value = atol(key.value);
				break;
			case FLOAT: 
				*(float *)value = atof(key.value);
				break;
			case DOUBLE: 
				*(double *)value = atof(key.value);
				break;
			default:
				printf("REGISTER type error!\n");
				return -1;
		}
	}

	if (NULL != comments && 0 != key.comments[0]) {
		strncpy(comments, key.comments, COMMENTS_LEN);
	}

	return 0;
}


