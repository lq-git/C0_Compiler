extern int get_ch();                   //��ȡ��һ���ַ���ÿ�ζ�ȡ512���ַ�
void translate(int sym_num);    //???
void reset_token();             //��մʷ���Ԫ
extern int trans_to_num();             //��TOKEN�ַ���ת��Ϊ����
void cat(char c);               //���ַ�c���뵽TOKEN�ַ�����
int is_letter();                //�ж�CHARΪ��ĸ��_
int is_digit();                 //�ж�CHARΪ����
int is_colon();                 //�ж�CHARΪ :
int is_semi();                  //�ж�CHARΪ ;
int is_comma();					//�ж�CHARΪ ,
int is_plus();					//�ж�CHARΪ +
int is_minus();					//�ж�CHARΪ -
int is_star();					//�ж�CHARΪ *
int is_div();					//�ж�CHARΪ /
int is_squo();					//�ж�CHARΪ '
int is_dquo();					//�ж�CHARΪ "
int is_lparent();				//�ж�CHARΪ (
int is_rparent();				//�ж�CHARΪ )
int is_lbracket();				//�ж�CHARΪ [
int is_rbracket();				//�ж�CHARΪ ]
int is_lbrace();				//�ж�CHARΪ {
int is_rbrace();				//�ж�CHARΪ }
int is_gtr();					//�ж�CHARΪ >
int is_lt();					//�ж�CHARΪ <
int is_equ();					//�ж�CHARΪ =
int is_exc();					//�ж�CHARΪ !
int is_zero();					//�ж�CHARΪ 0
int is_valid_character();		//�ж�CHARΪ �Ϸ��ַ�
int is_key();					//�ж�TOKENΪ �ؼ���
extern int insymbol();					//�ж���һ�ʷ���Ԫ���ͣ�������������