#define MAX_MID_CODE 1024

/*---中间代码的结构体设计---*/
struct mid_code {
	int op;
	char src1[100];
	char src2[100];
	char result[100];
	int is_effective;
};
struct mid_code_list {
	struct mid_code MID_CODE[MAX_MID_CODE];
	int code_num;
};

//中间代码的操作函数声明
extern void init_code_list();
extern void generate_mid_code(int op, char src1[], char scr2[], char result[]);
extern void generate_label(char label[]);
extern void generate_temp_var(char var[]);
extern void generate_new_temp_var(char var[]);
extern void print_mid_code();
extern void print_mid_code2();
extern void print_one_code(int number, FILE* mid_code_file);