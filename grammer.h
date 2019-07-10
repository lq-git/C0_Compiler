#define MAXLIST 1024
#define MAXNAME 50
#define MAXFUNC 100

//���ű����
struct one_symbol {
	char name[MAXNAME];            //��ʶ������
	int kind;                 /*����
							  0������
							  1������
							  2������
							  3����������
							  */
	int type;                 /*����
							  0��void(�������޷���ֵ����)
							  1��int
							  2: char
							  */
	int value;                /*������ֵ��ascii��
							  ������0
							  */
	int size;                 /*��������������
							  ���飺Ԫ�ظ���
							  ������-1
							  */
	int in_address;              //���������AR�׵�ַ��ƫ����(�����¼�׵�ַ)
};
struct table {
	struct one_symbol List[MAXLIST];     //���ű�
	int list_index;                      //����
	int num_of_func;                     //�Ѿ������ĺ�������
	int index_of_func[MAXFUNC];          //��ʼ��ȫ-1����Ŷ�Ӧ˳��ĺ�����table����
};


/*
�Ը����ɷֵĶ���(����&����˼Ϊ��û�е�����Ƴ���)
void program();               ����
void const_dec();             ��������
void var_dec();               ��������                  (ʵ����������ڶ��ֻ������жϲ�ͬ����������������Щʱ�򲻻�ʹ��)
void val_func_def();          �з���ֵ��������
void void_func_def();         �޷���ֵ��������
void main_func();             ������
void const_def();             ��������
void var_def();               ��������
void dec_head();              ����ͷ��
void para_list();             ������
void comp_state();            �������
void integer();               &����
void type_id();               &����ʶ���
void state_list();            �����
void if_state();              �������
void state();                 ���
void condition();             ����
void rela_op();               ��ϵ�����
void loop_state();            ѭ�����
void switch_state();          ������
void situation_list();        �����
void case_state();            ��������
void constant();              &����
void val_fun_call();          �з���ֵ�����������
void void_fun_call();         �޷���ֵ�����������
void val_para_list();         ֵ������
void assign_state();          ��ֵ���
void read_state();            �����
void write_state();           д���
void return_sate();           �������
void expr();                  ���ʽ
void plus_op();               �ӷ������
void mult_op();               �˷������
void term();                  ��
void factor();                ����
*/
//���к�������
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