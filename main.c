#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "lex.h"
#include "grammer.h"
#include "mid_code.h"
#include "error.h"

//ȫ�ֱ�������
FILE* compile_file;                                        //��������ļ�
FILE* output_file;                                         //�ʷ��������������ļ�

int main()
{
	/*char data_path[100];
	printf("������������ļ�·����\n");
	scanf("%s", data_path);*/
	if ((compile_file = fopen("D:\\Projects\\Compiler\\C0_Compiler\\C0_Compiler\\compile_file.txt", "r")) == NULL) {
		printf("Can not find the file.\n");
		return 0;
	}
	output_file = fopen("result.txt", "w");
	initial_table();
	init_code_list();
	CHAR = get_ch();
	symbol = insymbol();
	program();
	fprintf(output_file, "finish compiling\n");
	print_mid_code();
	/*generate_all_final_code();
	whole_process();
	print_mid_code2();
	generate_all_final_code2();*/
	fclose(compile_file);
	fclose(output_file);
	return 0;
}