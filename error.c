#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "global.h"

void error(int error_type, int line_index) {
	switch (error_type) {
	case 1: {
		fprintf(output_file, "line %d : non-zero integer shouldn't start with 0'\n", line_index);
		printf("line %d : non-zero integer shouldn't start with 0'\n", line_index);
		break;
	}
	case 2: {
		fprintf(output_file, "line %d : invalid char being declared\n", line_index);
		printf("line %d : invalid char being declared\n", line_index);
		break;
	}
	case 3: {
		fprintf(output_file, "line %d : single quatation don't match\n", line_index);
		printf("line %d : single quatation don't match\n", line_index);
		break;
	}
	case 4: {
		fprintf(output_file, "line %d : double quatation don't match\n", line_index);
		printf("line %d : double quatation don't match\n", line_index);
		break;
	}
	case 5: {
		fprintf(output_file, "line %d : invalid char appears\n", line_index);
		printf("line %d : invalid char appears\n", line_index);
		break;
	}
	case 6: {
		fprintf(output_file, "line %d : integer out of the range\n", line_index);
		printf("line %d : integer out of the range\n", line_index);
		break;
	}
	case 7: {
		fprintf(output_file, "line %d : table overflow!\n", line_index);
		printf("line %d : table overflow!\n", line_index);
		break;
	}
	case 8: {
		fprintf(output_file, "line %d: the parameter number doesn't match!\n", line_index);
		printf("line %d: the parameter number doesn't match!\n", line_index);
		break;
	}
	case 9: {
		fprintf(output_file, "line %d: this function name already exists!\n", line_index);
		printf("line %d: this function name already exists!\n", line_index);
		break;
	}
	case 10: {
		fprintf(output_file, "line %d: this variable name already exists!\n", line_index);
		printf("line %d: this variable name already exists!\n", line_index);
		break;
	}
	case 11: {
		fprintf(output_file, "line %d: this place should be an Identifier after type identifier!\n", line_index);
		printf("line %d: this place should be an Identifier after type identifier!\n", line_index);
		break;
	}
	case 12: {
		fprintf(output_file, "line %d: this place should be an equal in constant defination!\n", line_index);
		printf("line %d: this place should be an equal in constant defination!\n", line_index);
		break;
	}
	case 13: {
		fprintf(output_file, "line %d: the type doesn't match in cosntant defination!\n", line_index);
		printf("line %d: the type doesn't match in cosntant defination!\n", line_index);
		break;
	}
	case 14: {
		fprintf(output_file, "line %d: the type identifier is wrong in cosntant defination!\n", line_index);
		printf("line %d: the type identifier is wrong in cosntant defination!\n", line_index);
		break;
	}
	case 15: {
		fprintf(output_file, "line %d: this place should be a semicolon!\n", line_index);
		printf("line %d: this place should be a semicolon!\n", line_index);
		break;
	}
	case 16: {
		fprintf(output_file, "line %d: the index of array is not unsigned integer in variable defination!\n", line_index);
		printf("line %d: the index of array is not unsigned integer in variable defination!\n", line_index);
		break;
	}
	case 17: {
		fprintf(output_file, "line %d: this place should be a right parenthese!\n", line_index);
		printf("line %d: this place should be a right parenthese!\n", line_index);
		break;
	}
	case 18: {
		fprintf(output_file, "line %d: this place should be a right bracket!\n", line_index);
		printf("line %d: this place should be a right bracket!\n", line_index);
		break;
	}
	case 19: {
		fprintf(output_file, "line %d: this place should be a right brace!\n", line_index);
		printf("line %d: this place should be a right brace!\n", line_index);
		break;
	}
	case 20: {
		fprintf(output_file, "line %d: the identifier is not a variable in read statement!\n", line_index);
		printf("line %d: the identifier is not a variable in read statement!\n", line_index);
		break;
	}
	case 21: {
		fprintf(output_file, "line %d: this place should be the key word main!\n", line_index);
		printf("line %d: this place should be the key word main!\n", line_index);
		break;
	}
	case 22: {
		fprintf(output_file, "line %d: this place should be a left parenthese!\n", line_index);
		printf("line %d: this place should be a left parenthese!\n", line_index);
		break;
	}
	case 23: {
		fprintf(output_file, "line %d: this place should be a left brace!\n", line_index);
		printf("line %d: this place should be a left brace!\n", line_index);
		break;
	}
	case 24: {
		fprintf(output_file, "line %d: there should be an unsigned integer after + or -!\n", line_index);
		printf("line %d: there should be an unsigned integer after + or -!\n", line_index);
		break;
	}
	case 25: {
		fprintf(output_file, "line %d: there should be a factor here!\n", line_index);
		printf("line %d: there should be a factor here!\n", line_index);
		break;
	}
	case 26: {
		fprintf(output_file, "line %d: there should be a left parenthese or a left bracket or a left brace after identifier!\n", line_index);
		printf("line %d: there should be a left parenthese or a left bracket or a left brace after identifier!\n", line_index);
		break;
	}
	case 27: {
		fprintf(output_file, "line %d: wrong relational operater!\n", line_index);
		printf("line %d: wrong relational operater!\n", line_index);
		break;
	}
	case 28: {
		fprintf(output_file, "line %d: this place should be the key word while!\n", line_index);
		printf("line %d: this place should be the key word while!\n", line_index);
		break;
	}
	case 29: {
		fprintf(output_file, "line %d: array or const is not allowed to appear in read statement!\n", line_index);
		printf("line %d: array or const is not allowed to appear in read statement!\n", line_index);
		break;
	}
	case 30: {
		fprintf(output_file, "line %d: in void function, should not return value!\n", line_index);
		printf("line %d: in void function, should not return value!\n", line_index);
		break;
	}
	case 31: {
		fprintf(output_file, "line %d: in int or char function, should return value!\n", line_index);
		printf("line %d: in int or char function, should return value!\n", line_index);
		break;
	}
	case 32: {
		fprintf(output_file, "line %d: there should be void before the key word main!\n", line_index);
		printf("line %d: there should be void before the key word main!\n", line_index);
		break;
	}
	case 33: {
		fprintf(output_file, "line %d: mistake after identifier in identifier declararion!\n", line_index);
		printf("line %d: mistake after identifier in identifier declararion!\n", line_index);
		break;
	}
	case 34: {
		fprintf(output_file, "line %d: in int or char function, should have return statement!\n", line_index);
		printf("line %d: in int or char function, should have return statement!\n", line_index);
		break;
	}
	case 35: {
		fprintf(output_file, "line %d: this place should be the key word case!\n", line_index);
		printf("line %d: this place should be the key word case!\n", line_index);
		break;
	}
	case 36: {
		fprintf(output_file, "line %d: this place should be a constant after case!\n", line_index);
		printf("line %d: this place should be a constant after case!\n", line_index);
		break;
	}
	case 37: {
		fprintf(output_file, "line %d: this place should be a colon!\n", line_index);
		printf("line %d: this place should be a colon!\n", line_index);
		break;
	}
	case 38: {
		fprintf(output_file, "line %d: this place should be an equal!\n", line_index);
		printf("line %d: this place should be an equal!\n", line_index);
		break;
	}
	case 39: {
		fprintf(output_file, "line %d: undefined function call or the num of value para is not equal to defined para or the type is wrong!\n", line_index);
		printf("line %d: undefined function call or the num of value para is not equal to defined para or the type is wrong!\n", line_index);
		break;
	}
	case 40: {
		fprintf(output_file, "line %d: undefined identifier or wrong type!\n", line_index);
		printf("line %d: undefined identifier or wrong type!\n", line_index);
		break;
	}
	case 41: {
		fprintf(output_file, "line %d: mid_code out of index!\n", line_index);
		printf("line %d: mid_code out of index!\n", line_index);
		break;
	}
	case 42: {
		fprintf(output_file, "line %d: constant can't be assigned!\n", line_index);
		printf("line %d: constant can't be assigned!\n", line_index);
		break;
	}
	case 43: {
		fprintf(output_file, "line %d: array's name can't be assigned!\n", line_index);
		printf("line %d: array's name can't be assigned!\n", line_index);
		break;
	}
	case 44: {
		fprintf(output_file, "line %d: char shouldn't be assigned by int!(warning)\n", line_index);
		printf("line %d: char shouldn't be assigned by int!(warning)\n", line_index);
		break;
	}
	case 45: {
		fprintf(output_file, "line %d: function block out of range(optimizer)\n", line_index);
		printf("line %d: function block out of range(optimizer)\n", line_index);
		break;
	}
	case 46: {
		fprintf(output_file, "line %d: basic block out of range(optimizer)\n", line_index);
		printf("line %d: basic block out of range(optimizer)\n", line_index);
		break;
	}
	case 47: {
		fprintf(output_file, "line %d: num of node out of range(optimizer)\n", line_index);
		printf("line %d: num of node out of range(optimizer)\n", line_index);
		break;
	}
	case 48: {
		fprintf(output_file, "line %d: num of var one node out of range(optimizer)\n", line_index);
		printf("line %d: num of var one node out of range(optimizer)\n", line_index);
		break;
	}
	}
	//fclose(output_file);
	return;
}

void to_next_comma_semi()
{
	do {
		symbol = insymbol();
	} while (symbol != COMMASY && symbol != SEMISY);
}

void to_next_semi()
{
	do {
		symbol = insymbol();
	} while (symbol != SEMISY);
}

void to_next_int_char_void()
{
	do {
		symbol = insymbol();
	} while (symbol != INTSY && symbol != CHARSY && symbol != VOIDSY);
}

void to_next_rpar()
{
	do {
		symbol = insymbol();
	} while (symbol != RPARSY);
}

void to_next_rbras()
{
	do {
		symbol = insymbol();
	} while (symbol != RBRASSY);
}

void to_next_lbras()
{
	do {
		symbol = insymbol();
	} while (symbol != LBRASSY);
}

void to_next_int_char_state_nolbras() 
{
	do {
		symbol = insymbol();
	} while (symbol != INTSY && symbol != CHARSY && symbol != IFSY && symbol != DOSY && symbol != SWISY&& symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != RTSY);
}

void to_next_state()
{
	do {
		symbol = insymbol();
	} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY && symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != RTSY);
}

void to_next_state_semi_nolbras()
{
	do {
		symbol = insymbol();
	} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != IDSY && symbol != SCANSY && symbol != PRISY && symbol != RTSY&&symbol != SEMISY);
}

void to_next_state_case_colon()
{
	do {
		symbol = insymbol();
	} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY &&symbol != IDSY &&
		symbol != SCANSY && symbol != PRISY && symbol != SEMISY && symbol != RTSY&&symbol != COLONSY&&symbol != CASESY);
}

void to_next_state_case()
{
	do {
		symbol = insymbol();
	} while (symbol != IFSY && symbol != DOSY && symbol != SWISY && symbol != LBRASSY &&symbol != IDSY &&
		symbol != SCANSY && symbol != PRISY && symbol != SEMISY && symbol != RTSY && symbol != CASESY);
}