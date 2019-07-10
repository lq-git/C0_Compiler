//全局变量
#pragma once

//读入单词类型定义
#define CONSY 1                   //const
#define INTSY 2                   //int
#define CHARSY 3                  //char
#define VOIDSY 4                  //void
#define MAINSY 5                  //main
#define DOSY 6                    //do
#define WHISY 7                   //while
#define SWISY 8                   //switch
#define CASESY 9                  //case
#define IFSY 10                   //if
#define SCANSY 11                 //scanf
#define PRISY 12                  //printf
#define RTSY 13                   //return          前13个是保留字
#define IDSY 14                   //普通标识符
#define PLUSSY 15                 //+
#define MINUSSY 16                //-
#define STARSY 17                 //*
#define DIVSY 18                  //'/'
#define COMMASY 19                //,
#define COLONSY 20                //:
#define LPARSY 21                 //(
#define RPARSY 22                 //)
#define LBRASY 23                 //[
#define RBRASY 24                 //]
#define LBRASSY 25                //{
#define RBRASSY 26                //}
#define ASSIGNSY 27               //=
#define EQUSY 28                  //==
#define GTRSY 29                  //>
#define GTESY 30                  //>=
#define LTSY 31                   //<
#define LESY 32                   //<=
#define NEQSY 33                  // !=
#define DIGITSY 34                //无符号整数
#define CHAR2SY 35                //字符
#define STRINGSY 36               //字符串
#define SEMISY 37                 //;

#define FILE_OPEN_FAIL 0                                  //未能打开待编译文件
//下面是词法分析中的报错
#define ZERO_START_ERROR 1                                //非零的整数以0开头
#define INVALID_CHAR_ERROR 2                              //字符中的char是非法的
#define NO_MATCH_SQ_ERROR 3                               //单引号不匹配
#define NO_MATCH_DQ_ERROR 4                               //双引号不匹配
#define INVALID_CHARACTER_ERROR 5                         //文件中出现语法不接受的成分
#define OUT_OF_BOUND_ERROR 6                              //整型常量的值超过int
//下面是语法和语义分析中的报错
#define OUT_OF_TABLE_ERROR 7                              //符号表已满，无法添加新的标识符
#define PARA_NUM_ERROR 8                                  //引用函数时，值参和原函数定义的形参的数目不同
#define FUNC_REDEF_ERROR 9                                //定义函数时，发现符号表已经有同名的函数
#define VAR_REDEF_ERROR 10                                //在同层里已经有同名变量定义
#define ID_ERROR 11                                       //在类型标识符后面不是的sym不是标识符
#define CONST_DEF_ASSIGN_ERROR 12                         //在常量定义中，应该是等号的位置不是等号
#define CONST_DEF_TYPE_ERROR 13                           //常量定义中，等号后面的sym的类型和常量定义的类型不一致或者sym不是整数或者字符
#define CONST_DEC_TYPE_ERROR 14                           //常量声明中，常量的类型标识符不是int或char
#define SEMI_ERROR 15                                     //应该是分号的地方打错成别的sym类型
#define VAR_DEF_ARRAYINDEX_ERROR 16                       //变量定义中，数组的索引不是无符号整数
#define RPAR_ERROR 17                                     //应该时右小括号的地方sym类型不正确
#define RBRA_ERROR 18                                     //应该时右中括号的地方sym类型不正确
#define RBRAS_ERROR 19                                    //应该时右大括号的地方sym类型不正确
#define VAR_KIND_ERROR 20                                 //读语句中的标识符不是变量
#define MAINSY_ERROR 21                                   //应该是main保留字的地方sym类型错误
#define LPAR_ERROR 22                                     //应该时左小括号的地方sym类型不正确
#define LBRAS_ERROR 23                                    //应该时左大括号的地方sym类型不正确
#define AFTER_OP_DIGIT_ERROR 24                           //在整数中，+-号后面的不是无符号整数
#define FACTOR_ERROR 25                                   //在factor处理中认为所处理的部分不是factor,即识别符识别的第一个sym不符合factor任意一个分支
#define STATE_AFTER_ID_ERROR 26                           //在语句成分分析中，遇到了标识符作为开始，如果后面不是=,(,[中的一种，则需要报错
#define WRONG_RELA_OP_ERROR 27                            //错误的关系运算符
#define WHILESY_ERROR 28                                  //循环语句中，分析完do和state后，应该是while保留字，不是则为错误
#define READ_ARRAY_ERROR 29                               //读语句中，只能将读入的值存到普通变量中
#define VOID_RT_ERROR 30                                  //返回语句中，void函数的return后面有值
#define VAL_RT_ERROR 31                                   //返回语句中，非void函数的return后面没有值
#define NOT_VOID_MAIN_ERROR 32                            //main函数定义时前面不是void
#define ID_DEC_ERROR 33                                   //标识符声明时，后面应该是逗号、左小括号、左中括号或者分号，除此以外的标识符声明为错误格式(新)
#define NO_RT_FUNDEF_ERROR 34                             //有返回值函数定义时没有返回语句
#define CASE_ERROR 35                                     //应该是case的地方是其他的symbol
#define CASE_CONST_ERROR 36                               //case后面应该是常量，不是则报错
#define COLON_ERROR 37                                    //应该是冒号的地方是其他symbol
#define ASSIGN_ERROR 38                                   //应该是=的地方是其他symbol
#define FUNC_NO_DEF_ERROR 39                              //调用的函数未定义或者参数、类型不对
#define ID_NO_DEF_ERROR 40                                //标识符未定义或者类型不匹配
//中间代码生成中的报错
#define OUT_OF_CODE_ERROR 41                              //中间代码结构体已满，不能再存放代码
//语法分析中新增加的问题
#define CONST_ASSIGNMENT_ERROR 42                         //常量被赋值
#define ARRAY_ASSIGNMENT_ERROR 43                         //数组名字被直接赋值
#define CHAR_ASSIGNMENT_ERROR 44                          //char不能被int赋值
//优化中的报错，用于提示自己
#define OUT_OF_FUNC_BLOCK_ERROR 45                        //函数块太多
#define OUT_OF_BASIC_BLOCK_ERROR 46                       //基本快太多
#define OUT_OF_NODE_NUM 47                                //节点表或者导出表或者dag的节点数目太多
#define OUT_OF_ONE_NODE_VAR 48                            //一个节点上的变量数过多

extern FILE* compile_file;
extern FILE* output_file;     //词法分析结果输出的文件
extern int symbol;
extern int line;
extern char CHAR;
extern char TOKEN[100];
extern int index_in_line;
extern long trans_num;

extern const char CON[];
extern const char INT[];
extern const char _CHAR[];
extern const char VOID[];
extern const char MAIN[];
extern const char DO[];
extern const char WHILE[];
extern const char SWITCH[];
extern const char CASE[];
extern const char IF[];
extern const char SCANF[];
extern const char PRINTF[];
extern const char RETURN[];
extern const int imax;
extern const char CODE_EMPTY[];
extern const char CODE_INT[];
extern const char CODE_CHAR[];
extern const char CODE_CONST[];
extern const char CODE_ARRAY[];
extern const char CODE_FUNC[];
extern const char CODE_PARA[];
extern const char CODE_CALL[];
extern const char CODE_RT[];
extern const char CODE_EQU[];
extern const char CODE_NEQ[];
extern const char CODE_LABEL[];
extern const char CODE_SCAN[];
extern const char CODE_PRINT[];
extern const char CODE_GOTO[];
extern const char CODE_END[];
extern const char CODE_ZERO[];
