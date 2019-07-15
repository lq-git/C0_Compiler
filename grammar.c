#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "global.h"
#include "grammar.h"
#include "lex.h"
#include "mid_code.h"
#include "error.h"

int symbol;                   //读入的单词类型号
              /*---特殊设计的全局变量---*/

/*---1.保存现场用的---*/
int pre_symbol;               //暂时记录symbol
char pre_ch;                  //暂时记录CHAR
int pre_line_index;           //暂时记录index_in_line
char PRETOKEN[100];           //用于在特殊时刻暂时记录TOKEN的值

/*---2.填符号表用的全局变量---*/
/*---(0)公用的---*/
int address = 0;              //相对于当前AR的偏移地址
char TEMP[100];
int array_flag = 0;           //标识符是不是数组
int var_flag = 0;
int const_flag = 0;
int para_flag = 0;
int global_flag = 0;          //当前的变量或者常量是不是全局的

/*---(1)函数声明和调用中需要的---*/
int type_symbol;              //在函数声明中记录函数的type
char func_name[100];          //记录函数名称，供填表和参数表应用
int para_num;                 //记录函数的参数个数，填表用
int rt_flag;                  //检查有返回值函数是不是有return语句
int main_flag = 0;            //在处理RT语句时，如果是main，直接退出
int void_flag = 0;
int val_flag = 0;

/*---(2)变量声明中需要的---*/
char name_of_id[100];         //用于记录标识符的名字(包括常量和变量的名字)
int var_type;                 //变量定义中记录变量的类型 或者 参数的类型
int var_array_size;           //变量定义中记录数组变量的大小 变量声明中需要的
int const_type;               //常量定义中记录常量的类型
int const_value;              //常量定义中记录常量的值 表达式等需要的
int rela_op_symbol;           //关系运算符的symbol值
int factor_type = 0;          //记录当前factor的类型

char now_result[100];
int var_index;                //查询变量类型时记录变量在符号表中的位置，便于下次查询变量值时使用

struct table Table;
struct table new_Table;
struct table new_Table2;

/*---初始化符号表---*/
void initial_table() {
	Table.num_of_func = 0;
	Table.list_index = -1;
}

/*
---加入符号表，在加入时查重
---符号表已满 处理方式：报错后直接退出程序
---遇到重复变量或函数 处理方式：报错但不加入符号表
*/
void append_to_table(char name[], int kind, int type, int value, int size, int in_address) {
	int index, func_index;
	if (Table.list_index >= (MAXLIST - 1)) {
		//若符号表已满，报错后直接退出程序
		error(OUT_OF_TABLE_ERROR, line);
		fclose(compile_file);
		fclose(output_file);
		exit(-1);
		return;
	}
	if (kind == 2) {
		for (index = 0; index<Table.num_of_func; index++) {
			func_index = Table.index_of_func[index];
			if (strcmp(Table.List[func_index].name, name) == 0) {
				error(FUNC_REDEF_ERROR, line);//函数名字重复，报错不加入符号表
				return;
			}
		}
		//添加新的函数，list_index指向当前最后添加的索引
		Table.index_of_func[Table.num_of_func] = Table.list_index + 1;
		Table.num_of_func++;
	}
	else {
		func_index = Table.index_of_func[Table.num_of_func - 1];
		for (index = func_index; index <= Table.list_index; index++) {
			if (strcmp(Table.List[index].name, name) == 0) {
				error(VAR_REDEF_ERROR, line);//变量或常量同层有重名，报错后不加入符号表
				return;
			}
		}
	}
	Table.list_index++;
	strcpy(Table.List[Table.list_index].name, name);
	Table.List[Table.list_index].kind = kind;
	Table.List[Table.list_index].type = type;
	Table.List[Table.list_index].size = size;
	Table.List[Table.list_index].value = value;
	Table.List[Table.list_index].in_address = in_address;
}

/*---设置函数参数个数---*/
void append_para(int num) {
	int index;
	index = Table.index_of_func[Table.num_of_func - 1];
	Table.List[index].size = num;
}

/*
---查询ID类型，并设置全局flag
---依据标识符名返回标识符类型
---如查不到该标识符 处理方式：报错后返回-1 （返回-1后的处理方式）
*/
int get_type_by_name(char name[], int kind) {
	int i, index, first_func_index;
	array_flag = 0;
	var_flag = 0;
	const_flag = 0;
	para_flag = 0;
	global_flag = 0;
	if (kind == 2) {
		for (i = 0; i<Table.num_of_func; i++) {
			index = Table.index_of_func[i];
			if (strcmp(name, Table.List[index].name) == 0)
				break;//在本层找到相同名称时退出循环
		}
		if (i == Table.num_of_func) {
			error(FUNC_NO_DEF_ERROR, line);
			return -1;
		}
		else {
			var_index = index;
			return Table.List[index].type;//返回函数类型
		}
	}
	else { //普通标识符
		index = Table.index_of_func[Table.num_of_func - 1];
		for (; index <= Table.list_index; index++) {
			if (strcmp(Table.List[index].name, name) == 0) {
				break;//在本层找到相同名称时退出循环
			}
		}
		if (index > Table.list_index) { //本层找不到，去全局变量找
			first_func_index = Table.index_of_func[0];
			for (index = 0; index < first_func_index; index++) {
				if (strcmp(name, Table.List[index].name) == 0)
					break;
			}
			//全局变量层也找不到
			if (index == first_func_index) {
				error(ID_NO_DEF_ERROR, line);
				printf("wrong: %s\n", name);
				return -1;
			}
			global_flag = 1;//全局变量层找到后置为1
		}
		//变量在全局变量层或局部变量层找到后
		var_index = index;
		if (Table.List[index].kind == 1) {
			if (Table.List[index].size != -1){
				array_flag = 1;
			}
			else{
				var_flag = 1;
			}
			return Table.List[index].type;
		}
		else if (Table.List[index].kind == 0) {
			const_flag = 1;
			return Table.List[index].type;
		}
		else if (Table.List[index].kind == 3) {
			para_flag = 1;
			return Table.List[index].type;
		}
	}
	return 0;
}

//在调用函数时，查找函数是否在符号表中，并核对实参与形参数目是否相同
//实参数目即为para_num，在执行函数调用处理时初始化为0

/*---以函数名为条件在符号表中查询其类型并返回---*/
int get_func_type(char name[]) {
	int i, index;
	for (i = 0; i<Table.num_of_func; i++) {
		index = Table.index_of_func[i];
		if (strcmp(name, Table.List[index].name) == 0)
			break;
	}
	if (i == Table.num_of_func) {
		error(FUNC_NO_DEF_ERROR, line);
		return -1;
	}
	else {
		return Table.List[index].type;
	}
}

/*
---判断函数实参与形参是否相同
---若不相同 处理方式：报错后返回
*/
int is_correct_para(int index) {
	if (Table.List[index].size != para_num) {
		error(PARA_NUM_ERROR, line);
		return -1;
	}
	return 1;
}

/*
---查询常量值
---注意char类型常量存储其ASCII码
---此时已确定该变量为常量
*/
int get_const_value(char name[]) {
	int index, first_func_index;
	index = Table.index_of_func[Table.num_of_func - 1];
	for (; index <= Table.list_index; index++) {
		if (strcmp(Table.List[index].name, name) == 0) {
			break;//在本层找到相同名称时退出循环
		}
	}
	if (index > Table.list_index) { //本层找不到，去全局变量找
		first_func_index = Table.index_of_func[0];
		for (index = 0; index < first_func_index; index++) {
			if (strcmp(name, Table.List[index].name) == 0)
				break;
		}
		if (index == first_func_index) {
			error(ID_NO_DEF_ERROR, line);
			printf("wrong: %s\n", name);
			return -1;
		}
		return Table.List[index].value;
	}
	else {//本层找到
		return Table.List[index].value;
	}
}

/*
---查询常量值
---注意char类型常量存储其ASCII码
---此时已确定该变量为常量
*/
int get_const_value1() {
	return Table.List[var_index].value;
}

/*
---主处理程序
---常量声明并定义，在常量定义模块循环
---函数声明并定义
---变量声明无变量定义
---函数/变量说明时的错误
---1.标识符定义错误：不加入符号表，继续读取到下一个函数或者变量声明
---2.变量说明不以;结尾
---3.标识符后是非法字符
---处理方式：报错后继续读取到下一个函数或者变量声明
---必须遵循顺序：常量定义-->变量声明（包括函数声明）
*/
void program() {
	//常量说明部分
	if (symbol == CONSY) {
		const_dec();
	}

	//变量说明部分
	while (symbol == INTSY || symbol == CHARSY) {
		//因为int或者char开头的可能是变量声明或者函数声明，那种情况需要恢复现场
		pre_ch = CHAR;
		pre_line_index = index_in_line;
		pre_symbol = symbol;
		strcpy(PRETOKEN, TOKEN);
		symbol = insymbol();
		if (symbol != IDSY) {
			error(ID_ERROR, line);
			to_next_int_char_void();//标识符定义错误 读取到下一个函数或者变量声明
			continue;
		}
		//检测下一个单词，从而判别是变量还是函数(函数对应的是小括号)
		symbol = insymbol();
		if (symbol == LPARSY) {//检测到小括号，确定是函数说明
			strcpy(TOKEN, PRETOKEN);//恢复现场，跳出循环，进入函数说明处理
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			break;
		}
		else if (symbol == SEMISY || symbol == COMMASY || symbol == LBRASY) {//确认是变量说明
			strcpy(TOKEN, PRETOKEN);//恢复现场，进入变量定义模块
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			var_def();//变量声明
			if (symbol != SEMISY) {
				error(SEMI_ERROR, line);
				to_next_int_char_void();//跳到下一个函数或者变量声明
				continue;
			}
			else {
				symbol = insymbol();
			}
		}
		else {
			//这里应该输出错误信息：标识符后面的字符错误(非法声明)，并skip到下一个int、char、void
			error(ID_DEC_ERROR, line);
			to_next_int_char_void();//跳到下一个函数或者变量声明
			continue;
		}
	}

	//函数说明部分
	while (symbol == INTSY || symbol == CHARSY || symbol == VOIDSY) {
		if (symbol == INTSY || symbol == CHARSY) {

			val_func_def();//有返回值函数定义
		}
		else {
			pre_ch = CHAR;//记录现场
			pre_line_index = index_in_line;
			pre_symbol = symbol;
			strcpy(PRETOKEN, TOKEN);
			symbol = insymbol();
			if (symbol == MAINSY) {
				strcpy(TOKEN, PRETOKEN); //恢复现场，跳出进入主函数定义模块
				CHAR = pre_ch;
				index_in_line = pre_line_index;
				symbol = pre_symbol;
				break;
			}
			else {
				strcpy(TOKEN, PRETOKEN);//恢复现场，进入无返回值函数定义模块(此处可优化？)
				CHAR = pre_ch;
				index_in_line = pre_line_index;
				symbol = pre_symbol;
				void_func_def();
			}
		}
	}
	main_func();
	fprintf(output_file, "This is a program!\n");
	//printf("This is a program!\n");
}

/*
---常量说明 const type ID=_;
---常量声明立即定义
---会遇到的错误：不以';'结束
---处理方式：报错后继续读取到包括const、int、char的所有语句列
*/
void const_dec() {
	while (symbol == CONSY) {
		symbol = insymbol();
		const_def();//常量定义
		if (symbol != SEMISY) {
			error(SEMI_ERROR, line);
			//跳到下一句常量声明或者变量声明或语句列
			do {
				symbol = insymbol();
			} while (symbol != CONSY && symbol != INTSY && symbol != CHARSY && symbol != VOIDSY && symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY && symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != RTSY);
		}
		else {
			symbol = insymbol();
		}
	}
	fprintf(output_file, "This is a constant declaration!\n");
	//printf("This is a constant declaration!\n");
}

/*
---常量定义部分
---char 类型记录的值为其ASCII码
---会遇到的错误：
---1.常量定义类型不匹配错误；
---2.在整数中，+-号后面的不是无符号整数；
---3.在常量定义中，应该是等号的位置不是等号；
---4.标识符定义错误;
---处理方式：不加入符号表，继续读取，直至读到','|';' 也就是下一个常量定义或句子结束
---5.常量声明中，常量的类型标识符不是int或char
---处理方式：不加入符号表，继续读取，直至读到;' 也就是句子结束
*/
void const_def() {
	int sign = 1;//1:+ 0:-
	if (symbol == INTSY || symbol == CHARSY) {
		const_type = symbol;
		do {
			symbol = insymbol();
			if (symbol == IDSY) {
				strcpy(name_of_id, TOKEN);
				symbol = insymbol();
				if (symbol == ASSIGNSY) {
					symbol = insymbol();
					if (symbol == PLUSSY || symbol == MINUSSY) {//判断正负
						sign = (symbol == PLUSSY) ? 1 : 0;
						symbol = insymbol();
						if (symbol == DIGITSY && const_type == INTSY) {
							if (trans_num == 0) {
								error(AFTER_OP_DIGIT_ERROR, line);//在整数中，+-号后面的不是无符号整数
								to_next_comma_semi();
								continue;
							}
							if (sign == 1)
								const_value = trans_num;
							else
								const_value = 0 - trans_num;
							//填表，生成四元式
							address = address + 1;
							append_to_table(name_of_id, 0, 1, const_value, -1, address);
							sprintf(TEMP, "%d", const_value);
							generate_mid_code(1, name_of_id, TEMP, CODE_EMPTY);
							symbol = insymbol();
							fprintf(output_file, "This is an integer!\n");
							//printf("This is an integer!\n");
						}
						else {
							error(CONST_DEF_TYPE_ERROR, line);
							to_next_comma_semi();
							continue;
						}
					}
					else if (symbol == DIGITSY && const_type == INTSY) {
						sign = 1;
						const_value = trans_num;
						//填表，生成四元式
						address = address + 1;
						append_to_table(name_of_id, 0, 1, const_value, -1, address);
						sprintf(TEMP, "%d", const_value);
						generate_mid_code(1, name_of_id, TEMP, CODE_EMPTY);
						symbol = insymbol();
						fprintf(output_file, "This is an integer!\n");
						//printf("This is an integer!\n");
					}
					else if (symbol == CHAR2SY && const_type == CHARSY) {
						sign = 1;
						const_value = TOKEN[1];//char 记录的是ASCII码
						//填表，生成四元式
						address = address + 1;
						append_to_table(name_of_id, 0, 2, const_value, -1, address);
						sprintf(TEMP, "%d", const_value);
						generate_mid_code(2, name_of_id, TEMP, CODE_EMPTY);
						symbol = insymbol();
						fprintf(output_file, "This is a char!\n");
						//printf("This is a char!\n");
					}
					else {
						error(CONST_DEF_TYPE_ERROR, line);//常量定义类型错误
						to_next_comma_semi();
						continue;
					}
				}
				else {
					error(CONST_DEF_ASSIGN_ERROR, line);//在常量定义中，应该是等号的位置不是等号
					to_next_comma_semi();
					continue;
				}
			}
			else {
				error(ID_ERROR, line);//标识符定义错误
				to_next_comma_semi();
				continue;//如果不是标识符，就就继续读入 直到,或者;
			}
		} while (symbol == COMMASY);//处理完一个常量赋值语句
		fprintf(output_file, "This is a constant defination!\n");
		//printf("This is a constant defination!\n");
		return;
	}
	else {
		error(CONST_DEC_TYPE_ERROR, line);
		to_next_semi();
		return;
	}
}

/*
---有返回值函数定义
---会遇到的错误：
---会遇到的错误：
---1.标识符非法定义；
---2.标识符后不是(；
---3.)缺失；
---4.)后不是{;
---处理方式：报错后继续读入，直到},也就是直接跳过此函数处理下一函数
---5.函数形参标识符非法定义；
---处理方式：报错后继续读入，直到)，也就是直接跳过形参定义
---6.}缺失；
---处理方式：报错后继续读入，直到int|char|void，也就是读入下一函数
---7.缺失return 处理方式:报错
---*/
void val_func_def() {
	type_symbol = symbol;
	rt_flag = 0;
	dec_head();//函数定义类型+名称处理
	if (symbol == LPARSY) {//再次检查的原因：进入函数定义模块后，会有多个函数定义
		//标明有返回值函数
		val_flag = 1;
		void_flag = 0;
		if (type_symbol == CHARSY) {
			//char型函数 函数名称+类型生成中间代码
			generate_mid_code(8, func_name, CODE_EMPTY, CODE_EMPTY);
			add_func_block();
		}
		else {
			//int型函数 函数名称+类型生成中间代码
			generate_mid_code(7, func_name, CODE_EMPTY, CODE_EMPTY);
			add_func_block();
		}
		symbol = insymbol();
		para_list();//函数形参处理
		if (symbol == RPARSY) {
			symbol = insymbol();
			if (symbol == LBRASSY) {
				symbol = insymbol();
				comp_state();
				if (symbol == RBRASSY) {
					symbol = insymbol();
					if (rt_flag == 0) {
						printf("%s\n", func_name);
						error(NO_RT_FUNDEF_ERROR, line);
					}
					//生成四元式：函数的结束语句?
					rt_flag = 0;
					fprintf(output_file, "This is a value function defination!\n");
					//printf("This is a value function defination!\n");
					return;
				}
				else {
					error(RBRAS_ERROR, line);
					to_next_int_char_void();
					return;
				}
			}
			else {
				error(LBRAS_ERROR, line);
				to_next_rbras();//一旦出错，直接跳过此函数
				symbol = insymbol();
				return;
			}
		}
		else {
			error(RPAR_ERROR, line);
			to_next_rbras();
			symbol = insymbol();
			return;
		}
	}
	else {
		error(LPAR_ERROR, line);
		to_next_rbras();
		symbol = insymbol();
		return;
	}
}

/*
---无返回值函数定义
---会遇到的错误：
---1.标识符非法定义；
---2.标识符后不是(；
---3.)缺失；
---4.)后不是{;
---处理方式：报错后继续读入，直到},也就是直接跳过此函数处理下一函数
---5.函数形参标识符非法定义；
---处理方式：报错后继续读入，直到)，也就是直接跳过形参定义
---6.}缺失；
---处理方式：报错后继续读入，直到int|char|void，也就是读入下一函数
--------------------函数的结束语句存在问题----------------------------
*/
void void_func_def() {
	type_symbol = VOIDSY;
	symbol = insymbol();
	if (symbol == IDSY) {
		strcpy(func_name, TOKEN);
		//标识无返回值函数类型
		void_flag = 1;
		val_flag = 0;
		generate_mid_code(9, func_name, CODE_EMPTY, CODE_EMPTY);
		add_func_block();
		address = 0;
		append_to_table(func_name, 2, 0, 0, 0, address);//函数参数个数在para_list中修改
		symbol = insymbol();
		if (symbol == LPARSY) {
			symbol = insymbol();
			para_list();
			if (symbol == RPARSY) {
				symbol = insymbol();
				if (symbol == LBRASSY) {
					symbol = insymbol();
					comp_state();
					if (symbol == RBRASSY) {
						symbol = insymbol();
						//生成四元式：函数的结束语句？？？
						generate_mid_code(25, CODE_EMPTY, CODE_EMPTY, CODE_EMPTY);
						//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
						fprintf(output_file, "This is a void function defination!\n");
						//printf("This is a void function defination!\n");
						return;
					}
					else {
						error(RBRAS_ERROR, line);
						to_next_int_char_void();
						return;
					}
				}
				else {
					error(LBRAS_ERROR, line);
					to_next_rbras();
					symbol = insymbol();
					return;
				}
			}
			else {
				error(RPAR_ERROR, line);
				to_next_rbras();
				symbol = insymbol();
				return;
			}
		}
		else {
			error(LPAR_ERROR, line);
			to_next_rbras();
			symbol = insymbol();
			return;
		}
	}
	else {
		error(ID_ERROR, line);
		to_next_rbras();
		symbol = insymbol();
		return;
	}
}

/*
---主函数定义部分
---只支持void main()
---主函数生成四元式时与其他函数并无区别
---会遇到的错误：
---1.缺失(、)；
---处理方式：读取到{，也就是跳到主函数开始执行；
---2.缺失{；
---处理方式：结束主函数（有点不妥）
------------------错误处理需要改进----------------------
*/
void main_func() {
	/*
	*有待改进
	*出现了int main()提示标识符错误
	*不会出现那NOT_VOID_MAIN_ERROR错误
	*/
	if (symbol != VOIDSY) {
		error(NOT_VOID_MAIN_ERROR, line);
		return;
	}
	symbol = insymbol();
	if (symbol == MAINSY) {
		//填表+生成四元式
		strcpy(func_name, TOKEN);
		//标识着无返回值主函数
		val_flag = 0;
		void_flag = 1;
		main_flag = 1;
		generate_mid_code(9, func_name, CODE_EMPTY, CODE_EMPTY);
		add_func_block();
		address = 0;
		append_to_table(func_name, 2, 0, 0, 0, address);
		symbol = insymbol();
		if (symbol != LPARSY) {
			error(LPAR_ERROR, line);
			to_next_lbras();//跳到主函数开始初执行
		}
		else {
			symbol = insymbol();
		}
		if (symbol != RPARSY) {
			error(RPAR_ERROR, line);
			to_next_lbras();/*---------跳到主函数开始初执行，万一没有{呢---------*/
		}
		else {
			symbol = insymbol();
		}
		if (symbol != LBRASSY) {
			error(LBRAS_ERROR, line);
			return;
		}
		symbol = insymbol();
		comp_state();
		if (symbol == RBRASSY) {
			fprintf(output_file, "This is a main function defination!\n");
			//sprintf("This is a main function defination!\n");
			return;
		}
		else {
			error(RBRAS_ERROR, line);
			return;
		}
	}
	else {
		error(MAINSY_ERROR, line);
		return;//根本不会出现这种错误
	}
}

/*
---函数定义类型+名称处理
*/
void dec_head() {
	int real_type;
	if (type_symbol == INTSY) {
		real_type = 1;
	}
	else {
		real_type = 2;
	}
	symbol = insymbol();
	if (symbol == IDSY) {
		strcpy(func_name, TOKEN);
		address = 0;//每一函数定义时address置0
		//将函数的类型+名称填表
		append_to_table(func_name, 2, real_type, 0, 0, address);
		symbol = insymbol();
		fprintf(output_file, "This is a defination head!\n");
		//printf("This is a defination head!\n");
		return;
	}
	else {
		error(ID_ERROR, line);
		return;
	}
}

/*
---函数形参处理
*/
void para_list() {
	int real_type;
	para_num = 0;
	do {
		if (symbol == COMMASY) {
			symbol = insymbol();
		}
		if (symbol == INTSY || symbol == CHARSY) {
			var_type = symbol;
			if (var_type == INTSY) {
				real_type = 1;
			}
			else {
				real_type = 2;
			}
			symbol = insymbol();
			if (symbol == IDSY) {
				para_num++;
				strcpy(name_of_id, TOKEN);
				//函数形参加入表中
				address = address + 1;
				append_to_table(name_of_id, 3, real_type, 0, -1, address);
				//中间代码生成
				if (real_type == 1) {
					generate_mid_code(10, name_of_id, CODE_INT, CODE_EMPTY);
				}
				else {
					generate_mid_code(10, name_of_id, CODE_CHAR, CODE_EMPTY);
				}
				symbol = insymbol();
			}
			else {
				error(ID_ERROR, line);
				to_next_rpar();
				return;
			}
		}
	} while (symbol == COMMASY);
	append_para(para_num);//把函数的形参总体个数填表
	fprintf(output_file, "This is a parameter list!\n");
	//printf("This is a parameter list!\n");
}

/*
---变量说明模块（用于局部变量说明）
---调用var_dec()循环处理变量声明
---只有变量声明无定义
---数组变量加入符号表时，存储其偏移地址为其基地址+数组长度
---句子结束没有; 
---处理方式：继续读入，直至出现int char 或者if以及其他语句（注意不包括{}）
*/
void var_dec() {
	while (symbol == INTSY || symbol == CHARSY) {
		var_def();
		if (symbol != SEMISY) {
			error(SEMI_ERROR, line);
			to_next_int_char_state_nolbras();
		}
		else {
			symbol = insymbol();
		}
	}
	fprintf(output_file, "This is a variable declaration!\n");
	//printf("This is a variable declaration!\n");
}

/*
---变量说明模块（用于全局变量说明和局部变量说明）
---只有变量声明无定义
---数组变量加入符号表时，存储其偏移地址为其基地址+数组长度
---变量说明模块期间的错误：
---1.标识符定义错误（包括本身不是标识符和标识符后紧接的）
---2.右中括号不匹配错误(数组声明）
---3.数组的长度不是无符号整数（数组声明）
---处理方式：报错后，读取到下一个变量声明或句子
*/
void var_def() {
	int real_type;
	var_type = symbol;
	if (var_type == INTSY) {
		real_type = 1;
	}
	else {
		real_type = 2;
	}
	do {
		symbol = insymbol();
		if (symbol == IDSY) {//反复读入下一个词法单元，若下一个变量声明出现的是非标识符
			strcpy(name_of_id, TOKEN);
			symbol = insymbol();
			var_array_size = 0;
			if (symbol == COMMASY || symbol == SEMISY) {
				//是普通变量，该变量填表
				address = address + 1;
				append_to_table(name_of_id, 1, real_type, 0, -1, address);
				if (real_type == 1) {
					generate_mid_code(3, name_of_id, CODE_EMPTY, CODE_EMPTY);
				}
				else {
					generate_mid_code(4, name_of_id, CODE_EMPTY, CODE_EMPTY);
				}
				continue;
			}
			else if (symbol == LBRASY) {         //数组
				symbol = insymbol();
				if (symbol == DIGITSY) {
					var_array_size = trans_num;
					if (var_array_size != 0) {
						symbol = insymbol();
						if (symbol == RBRASY) {
							//把数组的名字、大小、类型等填表
							address = address + var_array_size;
							append_to_table(name_of_id, 1, real_type, 0, var_array_size, address);
							sprintf(TEMP, "%d", var_array_size);
							if (real_type == 1) {
								generate_mid_code(5, name_of_id, TEMP, CODE_EMPTY);
							}
							else {
								generate_mid_code(6, name_of_id, TEMP, CODE_EMPTY);
							}
							symbol = insymbol();
						}
						else {
							error(RBRA_ERROR, line);//不是右中括号
							to_next_comma_semi();
							continue;
						}
					}
					else {
						error(VAR_DEF_ARRAYINDEX_ERROR, line); //数组的长度不是无符号整数
						to_next_comma_semi();
						continue;
					}
				}
				else {
					error(VAR_DEF_ARRAYINDEX_ERROR, line);//数组的长度不是无符号整数
					to_next_comma_semi();
					continue;
				}
			}
			else {
				error(ID_DEC_ERROR, line);//变量声明错误
				to_next_comma_semi();
				continue;
			}
		}
		else {
			error(ID_ERROR, line);
			to_next_comma_semi();
			continue;
		}
	} while (symbol == COMMASY);
	fprintf(output_file, "This is a variable defination!\n");
	//printf("This is a variable defination!\n");
}

/*
---复合语句处理
---常量声明并定义，在子模块循环处理该类型；
---变量声明无定义，在子模块循环处理该类型；
---函数类的变量必须遵循先声明后赋值
---遵循常量定义-->变量声明-->语句列处理 的顺序
---语句列处理
*/
void comp_state() {
	if (symbol == CONSY) {
		const_dec();
	}
	if (symbol == INTSY || symbol == CHARSY) {
		var_dec();
	}
	state_list();
	fprintf(output_file, "This is a composite statement!\n");
	//printf("This is a composite statement!\n");
}

/*
---语句列处理
---若干语句，循环调用语句处理
*/
void state_list() {
	while (symbol == IFSY || symbol == DOSY || symbol == SWISY || symbol == LBRASSY || symbol == IDSY || symbol == SCANSY || symbol == PRISY || symbol == SEMISY || symbol == RTSY) {
		state();
	}
	fprintf(output_file, "This is a statement list!\n");
	//printf("This is a statement list!\n");
}

/*
---语句处理
---识别各种语句并进入相应语句处理
---条件语句|循环语句|情况语句|{语句列}|有返回值函数调用;|无返回值函数调用;|赋值语句;|读语句;|写语句;|空;|返回语句;
---{}会遇到的错误：} 丢失 处理方式：继续读入直至state（不包含{}）
---函数调用或赋值会遇到的错误：
---1.标识符后非合法定义的字符；
---2.句子结束但;缺失；
---3.函数未定义；
---处理方式：报错后继续读入直至下一个state（不包含{}）；

*/
void state() {
	int func_type;
	init_var_num(); //初始化函数调用参数总数为0
	if (symbol == IFSY) {
		if_state();//条件语句处理
	}
	else if (symbol == DOSY) {
		loop_state();//循环语句处理
	}
	else if (symbol == SWISY) {
		switch_state();//情况语句处理
	}
	else if (symbol == LBRASSY) {
		symbol = insymbol();
		state_list();//语句列处理
		if (symbol == RBRASSY) {
			symbol = insymbol();
		}
		else {
			error(RBRAS_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
	}
	else if (symbol == IDSY) {
		//函数调用或者赋值语句
		pre_ch = CHAR;//记录现场
		pre_line_index = index_in_line;
		pre_symbol = symbol;
		strcpy(PRETOKEN, TOKEN);
		symbol = insymbol();
		if (symbol == LPARSY) {
			//函数调用
			strcpy(TOKEN, PRETOKEN);//恢复现场，进入函数调用处理
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;//先一并处理了
			func_type = get_type_by_name(TOKEN, 2);
			//查表，确认是哪种函数，暂时都写成这样
			if (func_type == 0) {
				void_fun_call();//无返回值函数调用处理
			}
			else if (func_type == 1 || func_type == 2) {
				val_fun_call();
			}
			else {
				to_next_state_semi_nolbras();
				return;
			}
			if (symbol == SEMISY) {
				symbol = insymbol();
				fprintf(output_file, "This is a statement!\n");
				//printf("This is a statement!\n");
				return;
			}
			else {
				error(SEMI_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
		}
		else if (symbol == ASSIGNSY || symbol == LBRASY) {
			strcpy(TOKEN, PRETOKEN);//恢复现场，进入赋值语句处理 以标识符开始的语句
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			assign_state();//赋值语句处理
			if (symbol == SEMISY) {
				symbol = insymbol();
				fprintf(output_file, "This is a statement!\n");
				//printf("This is a statement!\n");
				return;
			}
			else {
				error(SEMI_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
		}
		else {
			error(STATE_AFTER_ID_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
	}
	else if (symbol == SCANSY) {
		read_state();//读语句处理
		if (symbol == SEMISY) {
			symbol = insymbol();
		}
		else {
			error(SEMI_ERROR, line);
			do {
				symbol = insymbol();
			} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY && symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != SEMISY && symbol != RTSY);
			return;
		}
	}
	else if (symbol == PRISY) {
		write_state();//写语句处理
		if (symbol == SEMISY) {
			symbol = insymbol();
		}
		else {
			error(SEMI_ERROR, line);
			do {
				symbol = insymbol();
			} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY && symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != SEMISY && symbol != RTSY);
			return;
		}
	}
	else if (symbol == RTSY) {
		return_state();//返回语句处理
		if (symbol == SEMISY) {
			symbol = insymbol();
		}
		else {
			error(SEMI_ERROR, line);
			do {
				symbol = insymbol();
			} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY && symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != SEMISY && symbol != RTSY);
			return;
		}
	}
	else if (symbol == SEMISY) {
		symbol = insymbol();
	}
	else {
		return; //暂时跳过
	}
	fprintf(output_file, "This is a statement!\n");
	//printf("This is a statement!\n");
}

/*
---条件语句处理
---先设置标签，并在{}之前设置当不成立时，跳到标签处执行；
---继续分析{}内语句，分析完后回填label；
---只有if无else。
---会遇到的错误：
---1.if后无(；
---2.缺失)；
---处理方式：继续读取，直至下一个语句state出现
*/
void if_state() {
	char label[100];
	char condition_result[100];
	generate_label(label);//label中存放LABEL_
	symbol = insymbol();
	if (symbol == LPARSY) {
		symbol = insymbol();
		condition();//条件处理，结果赋给now_result
		strcpy(condition_result, now_result);
		generate_mid_code(22, condition_result, label, CODE_EMPTY);//和0相等，跳到label处执行
																   //generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
		if (symbol == RPARSY) {
			symbol = insymbol();
			state();
			//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
			generate_mid_code(24, label, CODE_EMPTY, CODE_EMPTY);//设置上述label
			fprintf(output_file, "This is an if statement!\n");
			//printf("This is an if statement!\n");
			return;
		}
		else {
			error(RPAR_ERROR, line);
			to_next_state();
			return;
		}
	}
	else {
		error(LPAR_ERROR, line);
		to_next_state();
		return;
	}
}

/*
---条件处理
---只支持一个关系
---now_result=0 or now_result=1
*/
void condition() {
	char num1[100];
	char num2[100];
	int op_flag;//判断关系运算符是否合法
	strcpy(num1, CODE_EMPTY);
	strcpy(num2, CODE_EMPTY);
	expr();
	strcpy(num1, now_result);
	if (symbol == EQUSY || symbol == LTSY || symbol == LESY || symbol == GTRSY || symbol == GTESY || symbol == NEQSY) {
		op_flag=rela_op();//关系运算符处理，感觉没必要 就是保存symbol值
		if (op_flag == -1)
			return;
		expr();
		strcpy(num2, now_result);
		generate_temp_var(now_result);
		switch (rela_op_symbol) {
		case EQUSY: {
			generate_mid_code(20, num1, num2, now_result);
			break;
		}
		case LTSY: {
			generate_mid_code(17, num1, num2, now_result);
			break;
		}
		case LESY: {
			generate_mid_code(16, num1, num2, now_result);
			break;
		}
		case GTRSY: {
			generate_mid_code(19, num1, num2, now_result);
			break;
		}
		case GTESY: {
			generate_mid_code(18, num1, num2, now_result);
			break;
		}
		case NEQSY: {
			generate_mid_code(21, num1, num2, now_result);
			break;
		}
		}
	}
	else {
		generate_temp_var(now_result);
		generate_mid_code(21, num1, CODE_ZERO, now_result);//单值，判断是否为0，不为0 now_result=1
	}
	//address = address+1;
	//append_to_table(now_result,1,1,0,-1,address);   233
	fprintf(output_file, "This is a condition!\n");
	//printf("This is a condition!\n");
}

/*
---关系运算符处理
---rela_op_symbol中存放关系运算符类型
---会遇到的错误：非法关系运算符定义,
---错误处理：报错后继续读取,直至出现 ) 且返回-1通知condition此处有问题
*/
int rela_op() {
	if (symbol == EQUSY || symbol == GTRSY || symbol == GTESY || symbol == LTSY || symbol == LESY || symbol == NEQSY) {
		rela_op_symbol = symbol;
		symbol = insymbol();
	}
	else {
		error(WRONG_RELA_OP_ERROR, line);
		to_next_rpar();
		return -1;
	}
	fprintf(output_file, "This is a relational operation!\n");
	return 0;
	//printf("This is a relational operation!\n");
}

/*
---循环语句处理
---循环语句结构为 do 语句 while()
---循环开始时设置label，并设置该label，再判断循环条件后生成是否跳到label处执行
---会遇到的错误：
---1.while 缺失；
---2.while 后( 或 )缺失
---处理方式：继续读取，直至下一个state（不包括{})
*/
void loop_state() {
	char label[100];
	generate_label(label);//label中存放LABEL_
						  //generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
	generate_mid_code(24, label, CODE_EMPTY, CODE_EMPTY);//设置label，作为循环起点
	symbol = insymbol();
	state();
	if (symbol == WHISY) {
		symbol = insymbol();
		if (symbol == LPARSY) {
			symbol = insymbol();
			condition();
			if (symbol == RPARSY) {
				symbol = insymbol();
				generate_mid_code(23, now_result, label, CODE_EMPTY);//不等于0即条件成立时跳到label1处执行
																	 //generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
				fprintf(output_file, "This is a loop statement!\n");
				//printf("This is a loop statement!\n");
				return;
			}
			else {
				error(RPAR_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
		}
		else {
			error(LPAR_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
	}
	else {
		error(WHILESY_ERROR, line);
		to_next_state_semi_nolbras();
		return;
	}
}

/*
---情况语句处理
---其结构为：switch (expr) {情况表}
---情况表-->case 常量: 语句
---设置两个label，label指向switch语句的下一条语句，作为满足条件时跳出switch;
---my_label指向当前case语句的下一条语句，不满足条件时执行下一条case。
---会遇到的错误：
---1.switch 后( 或 )缺失；
---处理方式：继续读取，直至下一个state
---2.{ 或则 }缺失
---处理方式：继续读取，直至下一个state（不包含{}）
*/
void switch_state() {
	char label[100];
	char for_compare[100];//expr算出来的结果，用于比较
	generate_label(label);//label中存放LABEL_
	symbol = insymbol();
	if (symbol == LPARSY) {
		symbol = insymbol();
		expr();
		strcpy(for_compare, now_result);
		if (symbol == RPARSY) {
			symbol = insymbol();
			if (symbol == LBRASSY) {
				symbol = insymbol();
				situation_list(label, for_compare);//情况表处理
				if (symbol == RBRASSY) {
					symbol = insymbol();
					//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
					generate_mid_code(24, label, CODE_EMPTY, CODE_EMPTY);//设置label，指向swtich(){}后的语句
					fprintf(output_file, "This is a switch statement!\n");
					//printf("This is a switch statement!\n");
					return;
				}
				else {
					error(RBRAS_ERROR, line);
					to_next_state_semi_nolbras();
					return;
				}
			}
			else {
				error(LBRAS_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
		}
		else {
			error(RPAR_ERROR, line);
			to_next_state();
			return;
		}
	}
	else {
		error(LPAR_ERROR, line);
		to_next_state();
		return;
	}
}

/*
---情况表处理
*/
void situation_list(char label[], char for_compare[]) {
	do {
		case_state(label, for_compare);//情况子语句处理
	} while (symbol == CASESY);
	fprintf(output_file, "This is a situation list!\n");
	//printf("This is a situation list!\n");
}

/*
---情况子语句处理
---设置指向下一条语句的my_label，若符合条件，则直接跳出switch(){} 否则跳到my_label执行
---会遇到的错误：
---1.在整数中，+-号后面的不是无符号整数；
---2.应该是case的地方不是case
---3.case后不是常量
---处理方式：报错后继续读入直至读入:case|state|colon；
---4.case后缺失: 
---处理方式：报错后继续读入直至读入:case|state；
*/
void case_state(char label[], char for_compare[]) {
	char compare_result[100];
	char my_label[100];
	//generate_temp_var(compare_result);
	generate_new_temp_var(compare_result);//compare_result中存放$S_
										  //address = address+1;
										  //append_to_table(compare_result,1,1,0,-1,address);
	generate_label(my_label);//my_label中存放LABEL_
	if (symbol == CASESY) {
		symbol = insymbol();
		if (symbol == CHAR2SY) {
			const_value = TOKEN[1];
			sprintf(TEMP, "%d", const_value);
			generate_mid_code(20, for_compare, TEMP, compare_result);//相等时compare_result=1
			generate_mid_code(22, compare_result, my_label, CODE_EMPTY);//compare_result=0时，跳到my_label处执行
																		//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
			fprintf(output_file, "This is a char!\n");
			//printf("This is a char!\n");
			fprintf(output_file, "This is a constant!\n");
			//printf("This is a constant!\n");
		}
		else if (symbol == PLUSSY || symbol == MINUSSY) {
			symbol = insymbol();
			if (symbol != DIGITSY || trans_num == 0) {
				error(AFTER_OP_DIGIT_ERROR, line);
				to_next_state_case_colon();
				return;
			}
			generate_mid_code(20, for_compare, TOKEN, compare_result);
			generate_mid_code(22, compare_result, my_label, CODE_EMPTY);
			//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
			fprintf(output_file, "This is an integer!\n");
			//printf("This is an integer!\n");
			fprintf(output_file, "This is a constant!\n");
			//printf("This is a constant!\n");
		}
		else if (symbol == DIGITSY) {
			generate_mid_code(20, for_compare, TOKEN, compare_result);//相等时compare_result=1
			generate_mid_code(22, compare_result, my_label, CODE_EMPTY);//compare_result=0时，跳到my_label处执行
																		//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
			fprintf(output_file, "This is an integer!\n");
			//printf("This is an integer!\n");
			fprintf(output_file, "This is a constant!\n");
			//printf("This is a constant!\n");
		}
		else {
			error(CASE_CONST_ERROR, line);
			to_next_state_case_colon();
			return;
		}
		symbol = insymbol();
		if (symbol != COLONSY) {
			error(COLON_ERROR, line);
			to_next_state_case();
			return;
		}
		symbol = insymbol();
		state();
		generate_mid_code(33, label, CODE_EMPTY, CODE_EMPTY);//无条件跳到label处执行
															 //generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
		generate_mid_code(24, my_label, CODE_EMPTY, CODE_EMPTY);//设置my_label，my_label指向下一条case语句
		fprintf(output_file, "This is a case statement!\n");
		//printf("This is a case statement!\n");
		return;
	}
	else {
		error(CASE_ERROR, line);
		to_next_state_case_colon();
		return;
	}
}

void val_fun_call2() {
	int type;
	char my_func_name[100];
	strcpy(my_func_name, TOKEN);           //这个保险么？？？？？func_name 应该不会贝更改？
	symbol = insymbol(); //假设标识符和(已经检查过
	symbol = insymbol();
	val_para_list();
	type = get_type_by_name(my_func_name, 2);
	if (type == -1)
	{
		to_next_state_semi_nolbras();
		return;
	}
	if (type != 1 && type != 2) {
		error(FUNC_NO_DEF_ERROR, line);
		do {
			symbol = insymbol();
		} while (symbol != RPARSY);
		symbol = insymbol();
		return;
	}
	if (symbol != RPARSY) {
		error(RPAR_ERROR, line);
		do {
			symbol = insymbol();
		} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY && symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != SEMISY && symbol != RTSY);
		return;
	}
	generate_temp_var(now_result);
	//address = address+1;
	//append_to_table(now_result,1,1,0,-1,address);
	generate_mid_code(14, my_func_name, CODE_EMPTY, now_result);
	if (factor_type == 0) {
		if (type == 1) {
			factor_type = 1;
		}
		else {
			factor_type = 2;
		}
	}
	symbol = insymbol();
	fprintf(output_file, "This is a value function call!\n");
	//printf("This is a value function call!\n");
}

/*
---有返回值函数调用
---可能遇到的问题：
---1.函数形参与实参不匹配
---2.函数调用缺少 )
---处理方式：报错后继续读入直至下一个state（不包含{}）
*/
void val_fun_call() {
	int type;
	char my_func_name[100];
	int correct_para = 0;
	int temp_index = var_index;
	strcpy(my_func_name, TOKEN);
	symbol = insymbol();  // ( 已匹配
	symbol = insymbol();
	val_para_list();//进入函数实参处理
	correct_para = is_correct_para(temp_index);
	if (correct_para == -1)
	{
		to_next_state_semi_nolbras();
		return;
	}
	if (symbol != RPARSY) {
		error(RPAR_ERROR, line);
		to_next_state_semi_nolbras();
		return;
	}
	//此now_result会赋值给赋值语句的左边部分
	generate_temp_var(now_result);
	generate_mid_code(14, my_func_name, CODE_EMPTY, now_result);//生成调用有返回值函数的中间代码
	//生成返回值
	symbol = insymbol();
	fprintf(output_file, "This is a value function call!\n");
	//printf("This is a value function call!\n");
}

/*
---无返回值函数调用处理
---1.函数形参与实参不匹配
---2.函数调用缺少 )
---处理方式：报错后继续读入直至下一个state（不包含{}）
*/
void void_fun_call() {
	int type;
	char my_func_name[100];
	int correct_para;
	int temp_index = var_index;
	strcpy(my_func_name, TOKEN);
	symbol = insymbol(); // ( 已匹配
	symbol = insymbol();
	val_para_list();//进入函数实参处理
	correct_para = is_correct_para(temp_index);
	if (correct_para == -1)
	{
		to_next_state_semi_nolbras();
		return;
	}
	if (symbol != RPARSY) {
		error(RPAR_ERROR, line);
		to_next_state_semi_nolbras();
		return;
	}
	symbol = insymbol();
	generate_mid_code(15, my_func_name, CODE_EMPTY, CODE_EMPTY);//生成调用名字为my_func_name的函数的中间代码
	fprintf(output_file, "This is a void function call!\n");
	//printf("This is a void function call!\n");
}

/*
---函数实参处理
---生成实参压栈中间代码 从左向右压栈
---计算函数实参总数
*/
void val_para_list() {
	para_num = 0;//实参总数初始化为0
	do {
		if (symbol == COMMASY) {
			symbol = insymbol();
		}
		if (symbol == PLUSSY || symbol == MINUSSY || symbol == IDSY || symbol == CHAR2SY || symbol == DIGITSY || symbol == LPARSY) {
			expr();
			generate_mid_code(35, now_result, CODE_EMPTY, CODE_EMPTY);//将实参压栈，注意此为从左向右压栈
			para_num++;
		}
	} while (symbol == COMMASY);
	fprintf(output_file, "This is a value parameter list!\n");
	//printf("This is a value parameter list!\n");
	return;
}

/*
---赋值语句处理
---会遇到的错误：
---1.常量被赋值错误；
---2.数组整体被赋值；
---3.char被int赋值
---4.数组访问缺少]
---5.数组赋值缺失=
---处理方式：报错后继续读入直至下一个state（不包含{}）
*/
void assign_state() {
	char src1[100];
	char src2[100];
	char result[100];
	int var_type;
	//char temp[50];
	strcpy(result, TOKEN);
	symbol = insymbol();
	if (symbol == ASSIGNSY) {
		var_type = get_type_by_name(result, 0); 
		if (var_type == -1)
		{
			to_next_state_semi_nolbras();
			return;
		}
		//这里要判断：1.常量赋值错误  2.char被int赋值 3.数组整体赋值
		if (const_flag) {
			error(CONST_ASSIGNMENT_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		if (array_flag) {
			error(ARRAY_ASSIGNMENT_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		if (var_type == 2 && factor_type == 1) {
			error(CHAR_ASSIGNMENT_ERROR, line);
			to_next_state_semi_nolbras();
		}
		symbol = insymbol();
		expr();
		strcpy(src1, now_result);
		generate_mid_code(11, src1, CODE_EMPTY, result);//生成赋值语句
		fprintf(output_file, "This is an assignment statement!\n");
		//printf("This is an assignment statement!\n");
		return;
	}
	else {
		//数组元素赋值 数组下标访问属于一个项 注意这些是以标识符开始的语句
		symbol = insymbol();
		expr();
		strcpy(src2, now_result);
		if (symbol != RBRASY) {
			error(RBRA_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		symbol = insymbol();
		if (symbol != ASSIGNSY) {
			error(ASSIGN_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		symbol = insymbol();
		expr();
		strcpy(src1, now_result);
		generate_mid_code(12, src1, src2, result);//result[src2]=src1
		fprintf(output_file, "This is an assignment statement!\n");
		//printf("This is an assignment statement!\n");
		return;
	}
}

/*
---读语句处理
---读语句结构：scanf(ID);
---读入数据到ID
---可能遇到的错误：
---1.scanf后缺失(；
---2.(后不是标识符；
---3.读入的标识符是void类型；
---4.)缺失；
-------还要判断读入的标识符种类-------
---处理方式：报错后继续读入直至下一个state（不包含{}）
*/
void read_state() {
	char var_name[100];
	int var_type;
	symbol = insymbol();
	if (symbol != LPARSY) {
		error(LPAR_ERROR, line);
		to_next_state_semi_nolbras();
		return;
	}
	do {
		symbol = insymbol();
		if (symbol != IDSY) {
			error(ID_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		strcpy(var_name, TOKEN);
		var_type = get_type_by_name(var_name, 0);
		if (var_type == -1)
		{
			to_next_state_semi_nolbras();
			return;
		}
		if (var_flag == 0) {
			error(READ_ARRAY_ERROR, line);
		}
		if (var_type == 1) {
			generate_mid_code(27, CODE_INT, CODE_EMPTY, var_name);//将读入的值存入var_name中
		}
		else {
			generate_mid_code(27, CODE_CHAR, CODE_EMPTY, var_name);//将读入的值存入var_name中
		}
		symbol = insymbol();
	} while (symbol == COMMASY);
	if (symbol != RPARSY) {
		error(RPAR_ERROR, line);
		to_next_state_semi_nolbras();
		return;
	}
	symbol = insymbol();
	fprintf(output_file, "This is a read statement!\n");
	//printf("This is a read statement!\n");
}

/*
---写语句处理
---写语句结构：printf(字符串，expr)|printf(字符串)|printf(expr)
---可能遇到的错误：
---1.printf后缺失(；
---2.)缺失；
---3.读入的标识符是void类型；
---处理方式：报错后继续读入直至下一个state（不包含{}）
*/
void write_state() {
	char src1[100];
	char src2[100];
	strcpy(src1, CODE_EMPTY);
	strcpy(src2, CODE_EMPTY);
	symbol = insymbol();
	if (symbol != LPARSY) {
		error(LPAR_ERROR, line);
		to_next_state_semi_nolbras();
		return;
	}
	symbol = insymbol();
	if (symbol == STRINGSY) {
		strcpy(src1, TOKEN);
		fprintf(output_file, "This is a string!\n");
		//printf("This is a string!\n");
		symbol = insymbol();
		if (symbol == COMMASY) {
			symbol = insymbol();
			expr();
			if (symbol != RPARSY) {
				error(RPAR_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
			strcpy(src2, now_result);
			symbol = insymbol();
			if (factor_type == 1) {
				generate_mid_code(28, src1, src2, CODE_INT);
			}
			else {
				generate_mid_code(28, src1, src2, CODE_CHAR);
			}
			fprintf(output_file, "This is a write statement!\n");
			//printf("This is a write statement!\n");
			return;
		}
		else if (symbol == RPARSY) {
			symbol = insymbol();
			generate_mid_code(28, src1, src2, CODE_EMPTY);
			fprintf(output_file, "This is a write statement!\n");
			//printf("This is a write statement!\n");
			return;
		}
		else {
			error(RPAR_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
	}
	else {
		expr();
		if (symbol != RPARSY) {
			error(RPAR_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		strcpy(src2, now_result);
		if (factor_type == 1) {
			generate_mid_code(28, src1, src2, CODE_INT);
		}
		else {
			generate_mid_code(28, src1, src2, CODE_CHAR);
		}
		symbol = insymbol();
		fprintf(output_file, "This is a write statement!\n");
		//printf("This is a write statement!\n");
		return;
	}
}

/*
---返回语句处理
---可能遇到的错误：
---1.对于无返回值函数返回值；
---2.)缺失；
---3.对于有返回值函数无返回值；
---处理方式：报错后继续读入直至下一个state（不包含{}）
*/
void return_state() {
	char src[100];
	symbol = insymbol();
	if (symbol == LPARSY) {
		if (void_flag == 1) {
			error(VOID_RT_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		rt_flag = 1;
		symbol = insymbol();
		expr();
		strcpy(src, now_result);
		if (symbol != RPARSY) {
			error(RPAR_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		generate_mid_code(26, src, CODE_EMPTY, CODE_EMPTY);
		//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
		symbol = insymbol();
		fprintf(output_file, "This is a return statement!\n");
		//printf("This is a return statement!\n");
		return;
	}
	else {
		if (val_flag == 1) {
			error(VAL_RT_ERROR, line);
			to_next_state_semi_nolbras(); 
			return;
		}
		if (main_flag) {
			generate_mid_code(34, CODE_EMPTY, CODE_EMPTY, CODE_EMPTY);
			//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
		}
		else {
			generate_mid_code(25, CODE_EMPTY, CODE_EMPTY, CODE_EMPTY);
			//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
		}
		fprintf(output_file, "This is a return statement!\n");
		//printf("This is a return statement!\n");
		return;
	}
}

/*
---作为算术表达式，每次将加法因子作为一个项；
---每次加法运算均是先将result赋给src1，然后将项的结果赋给src2，新建临时变量$VAR_存放在result中
---运算结果返回给全局变量now_result
---now_result值表示如下：id_record、数值（包括常量、数字、char)、$VAR_
*/
void expr() {
	factor_type = 0;
	char src1[100];
	char src2[100];
	char result[100];
	if (symbol == PLUSSY || symbol == MINUSSY) {
		factor_type = 1;
		if (symbol == PLUSSY) {
			symbol = insymbol();
			term();
			strcpy(result, now_result);
		}
		else {
			symbol = insymbol();
			term();
			strcpy(src2, now_result);
			generate_temp_var(result);
			//address = address+1;
			//append_to_table(result,1,1,0,-1,address);//临时变量不加入符号表？？？
			generate_mid_code(30, CODE_ZERO, src2, result);
		}
		while (symbol == PLUSSY || symbol == MINUSSY) {
			factor_type = 1;
			strcpy(src1, result);
			generate_temp_var(result);
			//address = address+1;
			//append_to_table(result,1,1,0,-1,address);
			if (symbol == PLUSSY) {
				symbol = insymbol();
				term();
				strcpy(src2, now_result);
				generate_mid_code(29, src1, src2, result);
			}
			else {
				symbol = insymbol();
				term();
				strcpy(src2, now_result);
				generate_mid_code(30, src1, src2, result);
			}
		}
	}
	else {
		term();
		strcpy(result, now_result);
		while (symbol == PLUSSY || symbol == MINUSSY) {
			factor_type = 1;
			strcpy(src1, result);
			generate_temp_var(result);
			//address = address+1;
			//append_to_table(result,1,1,0,-1,address);
			if (symbol == PLUSSY) {
				symbol = insymbol();
				term();
				strcpy(src2, now_result);
				generate_mid_code(29, src1, src2, result);
			}
			else {
				symbol = insymbol();
				term();
				strcpy(src2, now_result);
				generate_mid_code(30, src1, src2, result);
			}
		}
	}
	strcpy(now_result, result);
	fprintf(output_file, "This is an expression!\n");
	//printf("This is an expression!\n");
}

/*
---作为加法因子，每次将乘法因子作为一个因子；
---乘法因子定义为(E)、ID、ID[E]、ID();
---每次乘法运算均是先将result赋给src1，然后将项的结果赋给src2，新建临时变量$VAR_存放在result中
---运算结果返回给全局变量now_result
*/
void term() {
	char src1[100];
	char src2[100];
	char result[100];
	factor();
	strcpy(result, now_result);
	while (symbol == STARSY || symbol == DIVSY) {
		strcpy(src1, result);
		generate_temp_var(result);
		//address = address+1;
		//append_to_table(result,1,1,0,-1,address);
		if (symbol == STARSY) {
			symbol = insymbol();
			factor();
			strcpy(src2, now_result);
			generate_mid_code(31, src1, src2, result);
		}
		else {
			symbol = insymbol();
			factor();
			strcpy(src2, now_result);
			generate_mid_code(32, src1, src2, result);
		}

	}
	strcpy(now_result, result);
	fprintf(output_file, "This is a term!\n");
	//printf("This is a term!\n");
}

/*
---作为乘法因子，即乘法运算的基本单元；
---每次需要乘法因子的类型；
---now_result中值对应如下：
---ID()-$VAR_
---ID[E]-存放该数组元素值的临时变量
---常量-常量值
---ID-id_record
---CHAR-char对应的的Ascii码
---DIGIT-digit对应的数值
---(E)-expr()中修改now_result
*/
void factor() {
	char result[100];
	char id_record[100];
	int temp_type;
	int sign;
	strcpy(result, CODE_EMPTY);
	if (symbol == IDSY) {
		//判断以标识符开头的词法单元的类型提前保存该标识符相关信息
		strcpy(id_record, TOKEN);
		pre_ch = CHAR;
		pre_line_index = index_in_line;
		pre_symbol = symbol;
		strcpy(PRETOKEN, TOKEN);
		symbol = insymbol();
		if (symbol == LBRASY) {//若是左中括号，则括号里内容同样为一表达式
			temp_type = get_type_by_name(id_record, 0);//检查变量是否存在的，并核对变量类型
			if (temp_type == -1)
			{
				error(FUNC_NO_DEF_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
			if (array_flag == 0) {
				error(ID_NO_DEF_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
			symbol = insymbol();
			temp_type = factor_type;//暂存factor类型 ？？？
			expr();
			factor_type = temp_type;
			if (symbol != RBRASY) {
				error(RBRA_ERROR, line);
				to_next_state_semi_nolbras();
				return;
			}
			if (factor_type == 0) {
				factor_type = temp_type;
			}
			strcpy(result, now_result);
			generate_temp_var(now_result);
			//address = address+1;
			//append_to_table(now_result,1,1,0,-1,address);
			generate_mid_code(36, id_record, result, now_result);//将数组元素的值赋给now_result
			symbol = insymbol();
			fprintf(output_file, "This is a factor!\n");
			//printf("This is a factor!\n");
			return;
		}
		else if (symbol == LPARSY) {
			temp_type = get_type_by_name(id_record, 2);//检查变量是否存在的，并核对变量类型
			if (temp_type == -1)
			{
				to_next_state_semi_nolbras();
				return;
			}
			strcpy(TOKEN, PRETOKEN);
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			val_fun_call();//函数调用，有无返回值？？？
			fprintf(output_file, "This is a factor!\n");
			//printf("This is a factor!\n");
			return;
		}
		else {
			if (factor_type == 0) {
				factor_type = get_type_by_name(id_record, 0);
			}
			temp_type = get_type_by_name(id_record, 0);
			if (temp_type == -1)
			{
				to_next_state_semi_nolbras();
				return;
			}
			if (const_flag == 1) {
				temp_type = get_const_value(id_record);
				sprintf(now_result, "%d", temp_type);//若是const类型，now_result中存放常量值
				const_flag = 0;
			}
			else {
				strcpy(now_result, id_record);
			}
			fprintf(output_file, "This is a factor!\n");
			//printf("This is a factor!\n");
			return;
		}
	}
	else if (symbol == CHAR2SY) {
		if (factor_type == 0) {
			factor_type = 2;
		}
		const_value = TOKEN[1];
		sprintf(now_result, "%d", const_value);
		symbol = insymbol();
		fprintf(output_file, "This is a char!\n");
		//printf("This is a char!\n");
		fprintf(output_file, "This is a factor!\n");
		//printf("This is a factor!\n");
		return;
	}
	else if (symbol == PLUSSY || symbol == MINUSSY) {
		if (symbol == PLUSSY) {
			sign = 1;
		}
		else {
			sign = -1;
		}
		symbol = insymbol();
		if (symbol != DIGITSY) {
			error(AFTER_OP_DIGIT_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		if (trans_num == 0) {
			error(AFTER_OP_DIGIT_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		trans_num = sign*trans_num;
		sprintf(now_result, "%ld", trans_num);
		factor_type = 1;
		symbol = insymbol();
		fprintf(output_file, "This is an integer!\n");
		//printf("This is an integer!\n");
		fprintf(output_file, "This is a factor!\n");
		//printf("This is a factor!\n");
		return;
	}
	else if (symbol == DIGITSY) {
		sprintf(now_result, "%ld", trans_num);
		factor_type = 1;
		symbol = insymbol();
		fprintf(output_file, "This is an integer!\n");
		//printf("This is an integer!\n");
		fprintf(output_file, "This is a factor!\n");
		//printf("This is a factor!\n");
		return;
	}
	else if (symbol == LPARSY) {
		symbol = insymbol();
		expr();
		if (symbol != RPARSY) {
			error(RPAR_ERROR, line);
			to_next_state_semi_nolbras();
			return;
		}
		symbol = insymbol();
		fprintf(output_file, "This is a factor!\n");
		//printf("This is a factor!\n");
		return;
	}
	else {
		error(FACTOR_ERROR, line);
		to_next_state_semi_nolbras();
		return;
	}
}