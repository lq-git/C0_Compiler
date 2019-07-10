#define MAXLIST 1024
#define MAXNAME 50
#define MAXFUNC 100

//符号表设计
struct one_symbol {
	char name[MAXNAME];            //标识符名称
	int kind;                 /*种类
							  0：常量
							  1：变量
							  2：函数
							  3：函数参数
							  */
	int type;                 /*类型
							  0：void(仅对于无返回值函数)
							  1：int
							  2: char
							  */
	int value;                /*常量：值或ascii码
							  其他：0
							  */
	int size;                 /*函数：参数个数
							  数组：元素个数
							  其他：-1
							  */
	int in_address;              //相对于所在AR首地址的偏移量(数组记录首地址)
};
struct table {
	struct one_symbol List[MAXLIST];     //符号表
	int list_index;                      //索引
	int num_of_func;                     //已经声明的函数个数
	int index_of_func[MAXFUNC];          //初始化全-1，存放对应顺序的函数的table索引
};


/*
对各个成分的定义(含有&的意思为：没有单独设计出来)
void program();               程序
void const_dec();             常量声明
void var_dec();               变量声明                  (实际设计中由于多种环境下判断不同，所以这个设计在有些时候不会使用)
void val_func_def();          有返回值函数定义
void void_func_def();         无返回值函数定义
void main_func();             主函数
void const_def();             常量定义
void var_def();               变量定义
void dec_head();              声明头部
void para_list();             参数表
void comp_state();            复合语句
void integer();               &整数
void type_id();               &类型识别符
void state_list();            语句列
void if_state();              条件语句
void state();                 语句
void condition();             条件
void rela_op();               关系运算符
void loop_state();            循环语句
void switch_state();          情况语句
void situation_list();        情况表
void case_state();            情况子语句
void constant();              &常量
void val_fun_call();          有返回值函数调用语句
void void_fun_call();         无返回值函数调用语句
void val_para_list();         值参数表
void assign_state();          赋值语句
void read_state();            读语句
void write_state();           写语句
void return_sate();           返回语句
void expr();                  表达式
void plus_op();               加法运算符
void mult_op();               乘法运算符
void term();                  项
void factor();                因子
*/
//所有函数声明
extern void program();
extern void const_dec();
extern void var_dec();
extern void val_func_def();
extern void void_func_def();
extern void main_func();
extern void const_def();
extern void var_def();
extern void dec_head();
extern void para_list();
extern void comp_state();
extern void state_list();
extern void if_state();
extern void state();
extern void condition();
extern void rela_op();
extern void loop_state();
extern void switch_state();
extern void situation_list();
extern void case_state();
extern void val_fun_call();
extern void val_fun_call2();
extern void void_fun_call();
extern void val_para_list();
extern void assign_state();
extern void read_state();
extern void write_state();
extern void return_state();
extern void expr();
extern void term();
extern void factor();
extern void initial_table();
extern void append_to_table(char name[], int kind, int type, int value, int size, int in_address);
extern void append_para(int num);
extern int index_in_table(char name[], int kind);
extern int get_func_type(char name[]);
//int get_func_total_size(char name[]);
extern int get_const_value(char name[]);