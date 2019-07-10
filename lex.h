extern int get_ch();                   //获取下一次字符，每次读取512个字符
void translate(int sym_num);    //???
void reset_token();             //清空词法单元
extern int trans_to_num();             //将TOKEN字符串转化为数字
void cat(char c);               //将字符c加入到TOKEN字符串中
int is_letter();                //判断CHAR为字母或_
int is_digit();                 //判断CHAR为数字
int is_colon();                 //判断CHAR为 :
int is_semi();                  //判断CHAR为 ;
int is_comma();					//判断CHAR为 ,
int is_plus();					//判断CHAR为 +
int is_minus();					//判断CHAR为 -
int is_star();					//判断CHAR为 *
int is_div();					//判断CHAR为 /
int is_squo();					//判断CHAR为 '
int is_dquo();					//判断CHAR为 "
int is_lparent();				//判断CHAR为 (
int is_rparent();				//判断CHAR为 )
int is_lbracket();				//判断CHAR为 [
int is_rbracket();				//判断CHAR为 ]
int is_lbrace();				//判断CHAR为 {
int is_rbrace();				//判断CHAR为 }
int is_gtr();					//判断CHAR为 >
int is_lt();					//判断CHAR为 <
int is_equ();					//判断CHAR为 =
int is_exc();					//判断CHAR为 !
int is_zero();					//判断CHAR为 0
int is_valid_character();		//判断CHAR为 合法字符
int is_key();					//判断TOKEN为 关键字
extern int insymbol();					//判断下一词法单元类型，并返回其类型