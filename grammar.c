#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "global.h"
#include "grammar.h"
#include "lex.h"
#include "mid_code.h"
#include "error.h"

int symbol;                   //����ĵ������ͺ�
              /*---������Ƶ�ȫ�ֱ���---*/

/*---1.�����ֳ��õ�---*/
int pre_symbol;               //��ʱ��¼symbol
char pre_ch;                  //��ʱ��¼CHAR
int pre_line_index;           //��ʱ��¼index_in_line
char PRETOKEN[100];           //����������ʱ����ʱ��¼TOKEN��ֵ

/*---2.����ű��õ�ȫ�ֱ���---*/
/*---(0)���õ�---*/
int address = 0;              //����ڵ�ǰAR��ƫ�Ƶ�ַ
char TEMP[100];
int array_flag = 0;           //��ʶ���ǲ�������
int var_flag = 0;
int const_flag = 0;
int para_flag = 0;
int global_flag = 0;          //��ǰ�ı������߳����ǲ���ȫ�ֵ�

/*---(1)���������͵�������Ҫ��---*/
int type_symbol;              //�ں��������м�¼������type
char func_name[100];          //��¼�������ƣ������Ͳ�����Ӧ��
int para_num;                 //��¼�����Ĳ��������������
int rt_flag;                  //����з���ֵ�����ǲ�����return���
int main_flag = 0;            //�ڴ���RT���ʱ�������main��ֱ���˳�
int void_flag = 0;
int val_flag = 0;

/*---(2)������������Ҫ��---*/
char name_of_id[100];         //���ڼ�¼��ʶ��������(���������ͱ���������)
int var_type;                 //���������м�¼���������� ���� ����������
int var_array_size;           //���������м�¼��������Ĵ�С ������������Ҫ��
int const_type;               //���������м�¼����������
int const_value;              //���������м�¼������ֵ ���ʽ����Ҫ��
int rela_op_symbol;           //��ϵ�������symbolֵ
int factor_type = 0;          //��¼��ǰfactor������

char now_result[100];
int var_index;                //��ѯ��������ʱ��¼�����ڷ��ű��е�λ�ã������´β�ѯ����ֵʱʹ��

struct table Table;
struct table new_Table;
struct table new_Table2;

/*---��ʼ�����ű�---*/
void initial_table() {
	Table.num_of_func = 0;
	Table.list_index = -1;
}

/*
---������ű��ڼ���ʱ����
---���ű����� ����ʽ�������ֱ���˳�����
---�����ظ��������� ����ʽ��������������ű�
*/
void append_to_table(char name[], int kind, int type, int value, int size, int in_address) {
	int index, func_index;
	if (Table.list_index >= (MAXLIST - 1)) {
		//�����ű������������ֱ���˳�����
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
				error(FUNC_REDEF_ERROR, line);//���������ظ�������������ű�
				return;
			}
		}
		//����µĺ�����list_indexָ��ǰ�����ӵ�����
		Table.index_of_func[Table.num_of_func] = Table.list_index + 1;
		Table.num_of_func++;
	}
	else {
		func_index = Table.index_of_func[Table.num_of_func - 1];
		for (index = func_index; index <= Table.list_index; index++) {
			if (strcmp(Table.List[index].name, name) == 0) {
				error(VAR_REDEF_ERROR, line);//��������ͬ��������������󲻼�����ű�
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

/*---���ú�����������---*/
void append_para(int num) {
	int index;
	index = Table.index_of_func[Table.num_of_func - 1];
	Table.List[index].size = num;
}

/*
---��ѯID���ͣ�������ȫ��flag
---���ݱ�ʶ�������ر�ʶ������
---��鲻���ñ�ʶ�� ����ʽ������󷵻�-1 ������-1��Ĵ���ʽ��
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
				break;//�ڱ����ҵ���ͬ����ʱ�˳�ѭ��
		}
		if (i == Table.num_of_func) {
			error(FUNC_NO_DEF_ERROR, line);
			return -1;
		}
		else {
			var_index = index;
			return Table.List[index].type;//���غ�������
		}
	}
	else { //��ͨ��ʶ��
		index = Table.index_of_func[Table.num_of_func - 1];
		for (; index <= Table.list_index; index++) {
			if (strcmp(Table.List[index].name, name) == 0) {
				break;//�ڱ����ҵ���ͬ����ʱ�˳�ѭ��
			}
		}
		if (index > Table.list_index) { //�����Ҳ�����ȥȫ�ֱ�����
			first_func_index = Table.index_of_func[0];
			for (index = 0; index < first_func_index; index++) {
				if (strcmp(name, Table.List[index].name) == 0)
					break;
			}
			//ȫ�ֱ�����Ҳ�Ҳ���
			if (index == first_func_index) {
				error(ID_NO_DEF_ERROR, line);
				printf("wrong: %s\n", name);
				return -1;
			}
			global_flag = 1;//ȫ�ֱ������ҵ�����Ϊ1
		}
		//������ȫ�ֱ������ֲ��������ҵ���
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

//�ڵ��ú���ʱ�����Һ����Ƿ��ڷ��ű��У����˶�ʵ�����β���Ŀ�Ƿ���ͬ
//ʵ����Ŀ��Ϊpara_num����ִ�к������ô���ʱ��ʼ��Ϊ0

/*---�Ժ�����Ϊ�����ڷ��ű��в�ѯ�����Ͳ�����---*/
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
---�жϺ���ʵ�����β��Ƿ���ͬ
---������ͬ ����ʽ������󷵻�
*/
int is_correct_para(int index) {
	if (Table.List[index].size != para_num) {
		error(PARA_NUM_ERROR, line);
		return -1;
	}
	return 1;
}

/*
---��ѯ����ֵ
---ע��char���ͳ����洢��ASCII��
---��ʱ��ȷ���ñ���Ϊ����
*/
int get_const_value(char name[]) {
	int index, first_func_index;
	index = Table.index_of_func[Table.num_of_func - 1];
	for (; index <= Table.list_index; index++) {
		if (strcmp(Table.List[index].name, name) == 0) {
			break;//�ڱ����ҵ���ͬ����ʱ�˳�ѭ��
		}
	}
	if (index > Table.list_index) { //�����Ҳ�����ȥȫ�ֱ�����
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
	else {//�����ҵ�
		return Table.List[index].value;
	}
}

/*
---��ѯ����ֵ
---ע��char���ͳ����洢��ASCII��
---��ʱ��ȷ���ñ���Ϊ����
*/
int get_const_value1() {
	return Table.List[var_index].value;
}

/*
---���������
---�������������壬�ڳ�������ģ��ѭ��
---��������������
---���������ޱ�������
---����/����˵��ʱ�Ĵ���
---1.��ʶ��������󣺲�������ű�������ȡ����һ���������߱�������
---2.����˵������;��β
---3.��ʶ�����ǷǷ��ַ�
---����ʽ������������ȡ����һ���������߱�������
---������ѭ˳�򣺳�������-->������������������������
*/
void program() {
	//����˵������
	if (symbol == CONSY) {
		const_dec();
	}

	//����˵������
	while (symbol == INTSY || symbol == CHARSY) {
		//��Ϊint����char��ͷ�Ŀ����Ǳ����������ߺ������������������Ҫ�ָ��ֳ�
		pre_ch = CHAR;
		pre_line_index = index_in_line;
		pre_symbol = symbol;
		strcpy(PRETOKEN, TOKEN);
		symbol = insymbol();
		if (symbol != IDSY) {
			error(ID_ERROR, line);
			to_next_int_char_void();//��ʶ��������� ��ȡ����һ���������߱�������
			continue;
		}
		//�����һ�����ʣ��Ӷ��б��Ǳ������Ǻ���(������Ӧ����С����)
		symbol = insymbol();
		if (symbol == LPARSY) {//��⵽С���ţ�ȷ���Ǻ���˵��
			strcpy(TOKEN, PRETOKEN);//�ָ��ֳ�������ѭ�������뺯��˵������
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			break;
		}
		else if (symbol == SEMISY || symbol == COMMASY || symbol == LBRASY) {//ȷ���Ǳ���˵��
			strcpy(TOKEN, PRETOKEN);//�ָ��ֳ��������������ģ��
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			var_def();//��������
			if (symbol != SEMISY) {
				error(SEMI_ERROR, line);
				to_next_int_char_void();//������һ���������߱�������
				continue;
			}
			else {
				symbol = insymbol();
			}
		}
		else {
			//����Ӧ�����������Ϣ����ʶ��������ַ�����(�Ƿ�����)����skip����һ��int��char��void
			error(ID_DEC_ERROR, line);
			to_next_int_char_void();//������һ���������߱�������
			continue;
		}
	}

	//����˵������
	while (symbol == INTSY || symbol == CHARSY || symbol == VOIDSY) {
		if (symbol == INTSY || symbol == CHARSY) {

			val_func_def();//�з���ֵ��������
		}
		else {
			pre_ch = CHAR;//��¼�ֳ�
			pre_line_index = index_in_line;
			pre_symbol = symbol;
			strcpy(PRETOKEN, TOKEN);
			symbol = insymbol();
			if (symbol == MAINSY) {
				strcpy(TOKEN, PRETOKEN); //�ָ��ֳ���������������������ģ��
				CHAR = pre_ch;
				index_in_line = pre_line_index;
				symbol = pre_symbol;
				break;
			}
			else {
				strcpy(TOKEN, PRETOKEN);//�ָ��ֳ��������޷���ֵ��������ģ��(�˴����Ż���)
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
---����˵�� const type ID=_;
---����������������
---�������Ĵ��󣺲���';'����
---����ʽ������������ȡ������const��int��char�����������
*/
void const_dec() {
	while (symbol == CONSY) {
		symbol = insymbol();
		const_def();//��������
		if (symbol != SEMISY) {
			error(SEMI_ERROR, line);
			//������һ�䳣���������߱��������������
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
---�������岿��
---char ���ͼ�¼��ֵΪ��ASCII��
---�������Ĵ���
---1.�����������Ͳ�ƥ�����
---2.�������У�+-�ź���Ĳ����޷���������
---3.�ڳ��������У�Ӧ���ǵȺŵ�λ�ò��ǵȺţ�
---4.��ʶ���������;
---����ʽ����������ű�������ȡ��ֱ������','|';' Ҳ������һ�������������ӽ���
---5.���������У����������ͱ�ʶ������int��char
---����ʽ����������ű�������ȡ��ֱ������;' Ҳ���Ǿ��ӽ���
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
					if (symbol == PLUSSY || symbol == MINUSSY) {//�ж�����
						sign = (symbol == PLUSSY) ? 1 : 0;
						symbol = insymbol();
						if (symbol == DIGITSY && const_type == INTSY) {
							if (trans_num == 0) {
								error(AFTER_OP_DIGIT_ERROR, line);//�������У�+-�ź���Ĳ����޷�������
								to_next_comma_semi();
								continue;
							}
							if (sign == 1)
								const_value = trans_num;
							else
								const_value = 0 - trans_num;
							//���������Ԫʽ
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
						//���������Ԫʽ
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
						const_value = TOKEN[1];//char ��¼����ASCII��
						//���������Ԫʽ
						address = address + 1;
						append_to_table(name_of_id, 0, 2, const_value, -1, address);
						sprintf(TEMP, "%d", const_value);
						generate_mid_code(2, name_of_id, TEMP, CODE_EMPTY);
						symbol = insymbol();
						fprintf(output_file, "This is a char!\n");
						//printf("This is a char!\n");
					}
					else {
						error(CONST_DEF_TYPE_ERROR, line);//�����������ʹ���
						to_next_comma_semi();
						continue;
					}
				}
				else {
					error(CONST_DEF_ASSIGN_ERROR, line);//�ڳ��������У�Ӧ���ǵȺŵ�λ�ò��ǵȺ�
					to_next_comma_semi();
					continue;
				}
			}
			else {
				error(ID_ERROR, line);//��ʶ���������
				to_next_comma_semi();
				continue;//������Ǳ�ʶ�����;ͼ������� ֱ��,����;
			}
		} while (symbol == COMMASY);//������һ��������ֵ���
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
---�з���ֵ��������
---�������Ĵ���
---�������Ĵ���
---1.��ʶ���Ƿ����壻
---2.��ʶ������(��
---3.)ȱʧ��
---4.)����{;
---����ʽ�������������룬ֱ��},Ҳ����ֱ�������˺���������һ����
---5.�����βα�ʶ���Ƿ����壻
---����ʽ�������������룬ֱ��)��Ҳ����ֱ�������βζ���
---6.}ȱʧ��
---����ʽ�������������룬ֱ��int|char|void��Ҳ���Ƕ�����һ����
---7.ȱʧreturn ����ʽ:����
---*/
void val_func_def() {
	type_symbol = symbol;
	rt_flag = 0;
	dec_head();//������������+���ƴ���
	if (symbol == LPARSY) {//�ٴμ���ԭ�򣺽��뺯������ģ��󣬻��ж����������
		//�����з���ֵ����
		val_flag = 1;
		void_flag = 0;
		if (type_symbol == CHARSY) {
			//char�ͺ��� ��������+���������м����
			generate_mid_code(8, func_name, CODE_EMPTY, CODE_EMPTY);
			add_func_block();
		}
		else {
			//int�ͺ��� ��������+���������м����
			generate_mid_code(7, func_name, CODE_EMPTY, CODE_EMPTY);
			add_func_block();
		}
		symbol = insymbol();
		para_list();//�����βδ���
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
					//������Ԫʽ�������Ľ������?
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
				to_next_rbras();//һ������ֱ�������˺���
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
---�޷���ֵ��������
---�������Ĵ���
---1.��ʶ���Ƿ����壻
---2.��ʶ������(��
---3.)ȱʧ��
---4.)����{;
---����ʽ�������������룬ֱ��},Ҳ����ֱ�������˺���������һ����
---5.�����βα�ʶ���Ƿ����壻
---����ʽ�������������룬ֱ��)��Ҳ����ֱ�������βζ���
---6.}ȱʧ��
---����ʽ�������������룬ֱ��int|char|void��Ҳ���Ƕ�����һ����
--------------------�����Ľ�������������----------------------------
*/
void void_func_def() {
	type_symbol = VOIDSY;
	symbol = insymbol();
	if (symbol == IDSY) {
		strcpy(func_name, TOKEN);
		//��ʶ�޷���ֵ��������
		void_flag = 1;
		val_flag = 0;
		generate_mid_code(9, func_name, CODE_EMPTY, CODE_EMPTY);
		add_func_block();
		address = 0;
		append_to_table(func_name, 2, 0, 0, 0, address);//��������������para_list���޸�
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
						//������Ԫʽ�������Ľ�����䣿����
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
---���������岿��
---ֻ֧��void main()
---������������Ԫʽʱ������������������
---�������Ĵ���
---1.ȱʧ(��)��
---����ʽ����ȡ��{��Ҳ����������������ʼִ�У�
---2.ȱʧ{��
---����ʽ���������������е㲻�ף�
------------------��������Ҫ�Ľ�----------------------
*/
void main_func() {
	/*
	*�д��Ľ�
	*������int main()��ʾ��ʶ������
	*���������NOT_VOID_MAIN_ERROR����
	*/
	if (symbol != VOIDSY) {
		error(NOT_VOID_MAIN_ERROR, line);
		return;
	}
	symbol = insymbol();
	if (symbol == MAINSY) {
		//���+������Ԫʽ
		strcpy(func_name, TOKEN);
		//��ʶ���޷���ֵ������
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
			to_next_lbras();//������������ʼ��ִ��
		}
		else {
			symbol = insymbol();
		}
		if (symbol != RPARSY) {
			error(RPAR_ERROR, line);
			to_next_lbras();/*---------������������ʼ��ִ�У���һû��{��---------*/
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
		return;//��������������ִ���
	}
}

/*
---������������+���ƴ���
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
		address = 0;//ÿһ��������ʱaddress��0
		//������������+�������
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
---�����βδ���
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
				//�����βμ������
				address = address + 1;
				append_to_table(name_of_id, 3, real_type, 0, -1, address);
				//�м��������
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
	append_para(para_num);//�Ѻ������β�����������
	fprintf(output_file, "This is a parameter list!\n");
	//printf("This is a parameter list!\n");
}

/*
---����˵��ģ�飨���ھֲ�����˵����
---����var_dec()ѭ�������������
---ֻ�б��������޶���
---�������������ű�ʱ���洢��ƫ�Ƶ�ַΪ�����ַ+���鳤��
---���ӽ���û��; 
---����ʽ���������룬ֱ������int char ����if�Լ�������䣨ע�ⲻ����{}��
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
---����˵��ģ�飨����ȫ�ֱ���˵���;ֲ�����˵����
---ֻ�б��������޶���
---�������������ű�ʱ���洢��ƫ�Ƶ�ַΪ�����ַ+���鳤��
---����˵��ģ���ڼ�Ĵ���
---1.��ʶ��������󣨰��������Ǳ�ʶ���ͱ�ʶ������ӵģ�
---2.�������Ų�ƥ�����(����������
---3.����ĳ��Ȳ����޷�������������������
---����ʽ������󣬶�ȡ����һ���������������
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
		if (symbol == IDSY) {//����������һ���ʷ���Ԫ������һ�������������ֵ��ǷǱ�ʶ��
			strcpy(name_of_id, TOKEN);
			symbol = insymbol();
			var_array_size = 0;
			if (symbol == COMMASY || symbol == SEMISY) {
				//����ͨ�������ñ������
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
			else if (symbol == LBRASY) {         //����
				symbol = insymbol();
				if (symbol == DIGITSY) {
					var_array_size = trans_num;
					if (var_array_size != 0) {
						symbol = insymbol();
						if (symbol == RBRASY) {
							//����������֡���С�����͵����
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
							error(RBRA_ERROR, line);//������������
							to_next_comma_semi();
							continue;
						}
					}
					else {
						error(VAR_DEF_ARRAYINDEX_ERROR, line); //����ĳ��Ȳ����޷�������
						to_next_comma_semi();
						continue;
					}
				}
				else {
					error(VAR_DEF_ARRAYINDEX_ERROR, line);//����ĳ��Ȳ����޷�������
					to_next_comma_semi();
					continue;
				}
			}
			else {
				error(ID_DEC_ERROR, line);//������������
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
---������䴦��
---�������������壬����ģ��ѭ����������ͣ�
---���������޶��壬����ģ��ѭ����������ͣ�
---������ı���������ѭ��������ֵ
---��ѭ��������-->��������-->����д��� ��˳��
---����д���
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
---����д���
---������䣬ѭ��������䴦��
*/
void state_list() {
	while (symbol == IFSY || symbol == DOSY || symbol == SWISY || symbol == LBRASSY || symbol == IDSY || symbol == SCANSY || symbol == PRISY || symbol == SEMISY || symbol == RTSY) {
		state();
	}
	fprintf(output_file, "This is a statement list!\n");
	//printf("This is a statement list!\n");
}

/*
---��䴦��
---ʶ�������䲢������Ӧ��䴦��
---�������|ѭ�����|������|{�����}|�з���ֵ��������;|�޷���ֵ��������;|��ֵ���;|�����;|д���;|��;|�������;
---{}�������Ĵ���} ��ʧ ����ʽ����������ֱ��state��������{}��
---�������û�ֵ�������Ĵ���
---1.��ʶ����ǺϷ�������ַ���
---2.���ӽ�����;ȱʧ��
---3.����δ���壻
---����ʽ��������������ֱ����һ��state��������{}����

*/
void state() {
	int func_type;
	init_var_num(); //��ʼ���������ò�������Ϊ0
	if (symbol == IFSY) {
		if_state();//������䴦��
	}
	else if (symbol == DOSY) {
		loop_state();//ѭ����䴦��
	}
	else if (symbol == SWISY) {
		switch_state();//�����䴦��
	}
	else if (symbol == LBRASSY) {
		symbol = insymbol();
		state_list();//����д���
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
		//�������û��߸�ֵ���
		pre_ch = CHAR;//��¼�ֳ�
		pre_line_index = index_in_line;
		pre_symbol = symbol;
		strcpy(PRETOKEN, TOKEN);
		symbol = insymbol();
		if (symbol == LPARSY) {
			//��������
			strcpy(TOKEN, PRETOKEN);//�ָ��ֳ������뺯�����ô���
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;//��һ��������
			func_type = get_type_by_name(TOKEN, 2);
			//���ȷ�������ֺ�������ʱ��д������
			if (func_type == 0) {
				void_fun_call();//�޷���ֵ�������ô���
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
			strcpy(TOKEN, PRETOKEN);//�ָ��ֳ������븳ֵ��䴦�� �Ա�ʶ����ʼ�����
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			assign_state();//��ֵ��䴦��
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
		read_state();//����䴦��
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
		write_state();//д��䴦��
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
		return_state();//������䴦��
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
		return; //��ʱ����
	}
	fprintf(output_file, "This is a statement!\n");
	//printf("This is a statement!\n");
}

/*
---������䴦��
---�����ñ�ǩ������{}֮ǰ���õ�������ʱ��������ǩ��ִ�У�
---��������{}����䣬����������label��
---ֻ��if��else��
---�������Ĵ���
---1.if����(��
---2.ȱʧ)��
---����ʽ��������ȡ��ֱ����һ�����state����
*/
void if_state() {
	char label[100];
	char condition_result[100];
	generate_label(label);//label�д��LABEL_
	symbol = insymbol();
	if (symbol == LPARSY) {
		symbol = insymbol();
		condition();//���������������now_result
		strcpy(condition_result, now_result);
		generate_mid_code(22, condition_result, label, CODE_EMPTY);//��0��ȣ�����label��ִ��
																   //generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
		if (symbol == RPARSY) {
			symbol = insymbol();
			state();
			//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
			generate_mid_code(24, label, CODE_EMPTY, CODE_EMPTY);//��������label
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
---��������
---ֻ֧��һ����ϵ
---now_result=0 or now_result=1
*/
void condition() {
	char num1[100];
	char num2[100];
	int op_flag;//�жϹ�ϵ������Ƿ�Ϸ�
	strcpy(num1, CODE_EMPTY);
	strcpy(num2, CODE_EMPTY);
	expr();
	strcpy(num1, now_result);
	if (symbol == EQUSY || symbol == LTSY || symbol == LESY || symbol == GTRSY || symbol == GTESY || symbol == NEQSY) {
		op_flag=rela_op();//��ϵ����������о�û��Ҫ ���Ǳ���symbolֵ
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
		generate_mid_code(21, num1, CODE_ZERO, now_result);//��ֵ���ж��Ƿ�Ϊ0����Ϊ0 now_result=1
	}
	//address = address+1;
	//append_to_table(now_result,1,1,0,-1,address);   233
	fprintf(output_file, "This is a condition!\n");
	//printf("This is a condition!\n");
}

/*
---��ϵ���������
---rela_op_symbol�д�Ź�ϵ���������
---�������Ĵ��󣺷Ƿ���ϵ���������,
---����������������ȡ,ֱ������ ) �ҷ���-1֪ͨcondition�˴�������
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
---ѭ����䴦��
---ѭ�����ṹΪ do ��� while()
---ѭ����ʼʱ����label�������ø�label�����ж�ѭ�������������Ƿ�����label��ִ��
---�������Ĵ���
---1.while ȱʧ��
---2.while ��( �� )ȱʧ
---����ʽ��������ȡ��ֱ����һ��state��������{})
*/
void loop_state() {
	char label[100];
	generate_label(label);//label�д��LABEL_
						  //generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
	generate_mid_code(24, label, CODE_EMPTY, CODE_EMPTY);//����label����Ϊѭ�����
	symbol = insymbol();
	state();
	if (symbol == WHISY) {
		symbol = insymbol();
		if (symbol == LPARSY) {
			symbol = insymbol();
			condition();
			if (symbol == RPARSY) {
				symbol = insymbol();
				generate_mid_code(23, now_result, label, CODE_EMPTY);//������0����������ʱ����label1��ִ��
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
---�����䴦��
---��ṹΪ��switch (expr) {�����}
---�����-->case ����: ���
---��������label��labelָ��switch������һ����䣬��Ϊ��������ʱ����switch;
---my_labelָ��ǰcase������һ����䣬����������ʱִ����һ��case��
---�������Ĵ���
---1.switch ��( �� )ȱʧ��
---����ʽ��������ȡ��ֱ����һ��state
---2.{ ���� }ȱʧ
---����ʽ��������ȡ��ֱ����һ��state��������{}��
*/
void switch_state() {
	char label[100];
	char for_compare[100];//expr������Ľ�������ڱȽ�
	generate_label(label);//label�д��LABEL_
	symbol = insymbol();
	if (symbol == LPARSY) {
		symbol = insymbol();
		expr();
		strcpy(for_compare, now_result);
		if (symbol == RPARSY) {
			symbol = insymbol();
			if (symbol == LBRASSY) {
				symbol = insymbol();
				situation_list(label, for_compare);//�������
				if (symbol == RBRASSY) {
					symbol = insymbol();
					//generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
					generate_mid_code(24, label, CODE_EMPTY, CODE_EMPTY);//����label��ָ��swtich(){}������
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
---�������
*/
void situation_list(char label[], char for_compare[]) {
	do {
		case_state(label, for_compare);//�������䴦��
	} while (symbol == CASESY);
	fprintf(output_file, "This is a situation list!\n");
	//printf("This is a situation list!\n");
}

/*
---�������䴦��
---����ָ����һ������my_label����������������ֱ������switch(){} ��������my_labelִ��
---�������Ĵ���
---1.�������У�+-�ź���Ĳ����޷���������
---2.Ӧ����case�ĵط�����case
---3.case���ǳ���
---����ʽ��������������ֱ������:case|state|colon��
---4.case��ȱʧ: 
---����ʽ��������������ֱ������:case|state��
*/
void case_state(char label[], char for_compare[]) {
	char compare_result[100];
	char my_label[100];
	//generate_temp_var(compare_result);
	generate_new_temp_var(compare_result);//compare_result�д��$S_
										  //address = address+1;
										  //append_to_table(compare_result,1,1,0,-1,address);
	generate_label(my_label);//my_label�д��LABEL_
	if (symbol == CASESY) {
		symbol = insymbol();
		if (symbol == CHAR2SY) {
			const_value = TOKEN[1];
			sprintf(TEMP, "%d", const_value);
			generate_mid_code(20, for_compare, TEMP, compare_result);//���ʱcompare_result=1
			generate_mid_code(22, compare_result, my_label, CODE_EMPTY);//compare_result=0ʱ������my_label��ִ��
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
			generate_mid_code(20, for_compare, TOKEN, compare_result);//���ʱcompare_result=1
			generate_mid_code(22, compare_result, my_label, CODE_EMPTY);//compare_result=0ʱ������my_label��ִ��
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
		generate_mid_code(33, label, CODE_EMPTY, CODE_EMPTY);//����������label��ִ��
															 //generate_mid_code(37,CODE_EMPTY,CODE_EMPTY,CODE_EMPTY);
		generate_mid_code(24, my_label, CODE_EMPTY, CODE_EMPTY);//����my_label��my_labelָ����һ��case���
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
	strcpy(my_func_name, TOKEN);           //�������ô����������func_name Ӧ�ò��ᱴ���ģ�
	symbol = insymbol(); //�����ʶ����(�Ѿ�����
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
---�з���ֵ��������
---�������������⣺
---1.�����β���ʵ�β�ƥ��
---2.��������ȱ�� )
---����ʽ��������������ֱ����һ��state��������{}��
*/
void val_fun_call() {
	int type;
	char my_func_name[100];
	int correct_para = 0;
	int temp_index = var_index;
	strcpy(my_func_name, TOKEN);
	symbol = insymbol();  // ( ��ƥ��
	symbol = insymbol();
	val_para_list();//���뺯��ʵ�δ���
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
	//��now_result�ḳֵ����ֵ������߲���
	generate_temp_var(now_result);
	generate_mid_code(14, my_func_name, CODE_EMPTY, now_result);//���ɵ����з���ֵ�������м����
	//���ɷ���ֵ
	symbol = insymbol();
	fprintf(output_file, "This is a value function call!\n");
	//printf("This is a value function call!\n");
}

/*
---�޷���ֵ�������ô���
---1.�����β���ʵ�β�ƥ��
---2.��������ȱ�� )
---����ʽ��������������ֱ����һ��state��������{}��
*/
void void_fun_call() {
	int type;
	char my_func_name[100];
	int correct_para;
	int temp_index = var_index;
	strcpy(my_func_name, TOKEN);
	symbol = insymbol(); // ( ��ƥ��
	symbol = insymbol();
	val_para_list();//���뺯��ʵ�δ���
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
	generate_mid_code(15, my_func_name, CODE_EMPTY, CODE_EMPTY);//���ɵ�������Ϊmy_func_name�ĺ������м����
	fprintf(output_file, "This is a void function call!\n");
	//printf("This is a void function call!\n");
}

/*
---����ʵ�δ���
---����ʵ��ѹջ�м���� ��������ѹջ
---���㺯��ʵ������
*/
void val_para_list() {
	para_num = 0;//ʵ��������ʼ��Ϊ0
	do {
		if (symbol == COMMASY) {
			symbol = insymbol();
		}
		if (symbol == PLUSSY || symbol == MINUSSY || symbol == IDSY || symbol == CHAR2SY || symbol == DIGITSY || symbol == LPARSY) {
			expr();
			generate_mid_code(35, now_result, CODE_EMPTY, CODE_EMPTY);//��ʵ��ѹջ��ע���Ϊ��������ѹջ
			para_num++;
		}
	} while (symbol == COMMASY);
	fprintf(output_file, "This is a value parameter list!\n");
	//printf("This is a value parameter list!\n");
	return;
}

/*
---��ֵ��䴦��
---�������Ĵ���
---1.��������ֵ����
---2.�������屻��ֵ��
---3.char��int��ֵ
---4.�������ȱ��]
---5.���鸳ֵȱʧ=
---����ʽ��������������ֱ����һ��state��������{}��
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
		//����Ҫ�жϣ�1.������ֵ����  2.char��int��ֵ 3.�������帳ֵ
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
		generate_mid_code(11, src1, CODE_EMPTY, result);//���ɸ�ֵ���
		fprintf(output_file, "This is an assignment statement!\n");
		//printf("This is an assignment statement!\n");
		return;
	}
	else {
		//����Ԫ�ظ�ֵ �����±��������һ���� ע����Щ���Ա�ʶ����ʼ�����
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
---����䴦��
---�����ṹ��scanf(ID);
---�������ݵ�ID
---���������Ĵ���
---1.scanf��ȱʧ(��
---2.(���Ǳ�ʶ����
---3.����ı�ʶ����void���ͣ�
---4.)ȱʧ��
-------��Ҫ�ж϶���ı�ʶ������-------
---����ʽ��������������ֱ����һ��state��������{}��
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
			generate_mid_code(27, CODE_INT, CODE_EMPTY, var_name);//�������ֵ����var_name��
		}
		else {
			generate_mid_code(27, CODE_CHAR, CODE_EMPTY, var_name);//�������ֵ����var_name��
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
---д��䴦��
---д���ṹ��printf(�ַ�����expr)|printf(�ַ���)|printf(expr)
---���������Ĵ���
---1.printf��ȱʧ(��
---2.)ȱʧ��
---3.����ı�ʶ����void���ͣ�
---����ʽ��������������ֱ����һ��state��������{}��
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
---������䴦��
---���������Ĵ���
---1.�����޷���ֵ��������ֵ��
---2.)ȱʧ��
---3.�����з���ֵ�����޷���ֵ��
---����ʽ��������������ֱ����һ��state��������{}��
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
---��Ϊ�������ʽ��ÿ�ν��ӷ�������Ϊһ���
---ÿ�μӷ���������Ƚ�result����src1��Ȼ����Ľ������src2���½���ʱ����$VAR_�����result��
---���������ظ�ȫ�ֱ���now_result
---now_resultֵ��ʾ���£�id_record����ֵ���������������֡�char)��$VAR_
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
			//append_to_table(result,1,1,0,-1,address);//��ʱ������������ű�����
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
---��Ϊ�ӷ����ӣ�ÿ�ν��˷�������Ϊһ�����ӣ�
---�˷����Ӷ���Ϊ(E)��ID��ID[E]��ID();
---ÿ�γ˷���������Ƚ�result����src1��Ȼ����Ľ������src2���½���ʱ����$VAR_�����result��
---���������ظ�ȫ�ֱ���now_result
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
---��Ϊ�˷����ӣ����˷�����Ļ�����Ԫ��
---ÿ����Ҫ�˷����ӵ����ͣ�
---now_result��ֵ��Ӧ���£�
---ID()-$VAR_
---ID[E]-��Ÿ�����Ԫ��ֵ����ʱ����
---����-����ֵ
---ID-id_record
---CHAR-char��Ӧ�ĵ�Ascii��
---DIGIT-digit��Ӧ����ֵ
---(E)-expr()���޸�now_result
*/
void factor() {
	char result[100];
	char id_record[100];
	int temp_type;
	int sign;
	strcpy(result, CODE_EMPTY);
	if (symbol == IDSY) {
		//�ж��Ա�ʶ����ͷ�Ĵʷ���Ԫ��������ǰ����ñ�ʶ�������Ϣ
		strcpy(id_record, TOKEN);
		pre_ch = CHAR;
		pre_line_index = index_in_line;
		pre_symbol = symbol;
		strcpy(PRETOKEN, TOKEN);
		symbol = insymbol();
		if (symbol == LBRASY) {//�����������ţ�������������ͬ��Ϊһ���ʽ
			temp_type = get_type_by_name(id_record, 0);//�������Ƿ���ڵģ����˶Ա�������
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
			temp_type = factor_type;//�ݴ�factor���� ������
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
			generate_mid_code(36, id_record, result, now_result);//������Ԫ�ص�ֵ����now_result
			symbol = insymbol();
			fprintf(output_file, "This is a factor!\n");
			//printf("This is a factor!\n");
			return;
		}
		else if (symbol == LPARSY) {
			temp_type = get_type_by_name(id_record, 2);//�������Ƿ���ڵģ����˶Ա�������
			if (temp_type == -1)
			{
				to_next_state_semi_nolbras();
				return;
			}
			strcpy(TOKEN, PRETOKEN);
			CHAR = pre_ch;
			index_in_line = pre_line_index;
			symbol = pre_symbol;
			val_fun_call();//�������ã����޷���ֵ������
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
				sprintf(now_result, "%d", temp_type);//����const���ͣ�now_result�д�ų���ֵ
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