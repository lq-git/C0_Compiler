//�ڲ��ṹ����ƣ���Ÿ����Ĵ�����Ӧ�ı�������
struct reg_var {
	char reg_name[20];
	char var_name[100];
	int is_var;//0: const 1: just var -1: no data
};

//��������
extern void get_new_Table();
extern void data_area_generate();
extern void generate_all_final_code();
extern void generate_text();
extern int new_index_in_tab(char name[]);
extern int get_func_para_num(char name[]);
extern void load_data(char reg_name[], char var_name[]);
extern void store_data(char reg_name[], char var_name[]);
extern int get_func_total_size(char name[]);