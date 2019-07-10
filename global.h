//ȫ�ֱ���
#pragma once

//���뵥�����Ͷ���
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
#define RTSY 13                   //return          ǰ13���Ǳ�����
#define IDSY 14                   //��ͨ��ʶ��
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
#define DIGITSY 34                //�޷�������
#define CHAR2SY 35                //�ַ�
#define STRINGSY 36               //�ַ���
#define SEMISY 37                 //;

#define FILE_OPEN_FAIL 0                                  //δ�ܴ򿪴������ļ�
//�����Ǵʷ������еı���
#define ZERO_START_ERROR 1                                //�����������0��ͷ
#define INVALID_CHAR_ERROR 2                              //�ַ��е�char�ǷǷ���
#define NO_MATCH_SQ_ERROR 3                               //�����Ų�ƥ��
#define NO_MATCH_DQ_ERROR 4                               //˫���Ų�ƥ��
#define INVALID_CHARACTER_ERROR 5                         //�ļ��г����﷨�����ܵĳɷ�
#define OUT_OF_BOUND_ERROR 6                              //���ͳ�����ֵ����int
//�������﷨����������еı���
#define OUT_OF_TABLE_ERROR 7                              //���ű��������޷�����µı�ʶ��
#define PARA_NUM_ERROR 8                                  //���ú���ʱ��ֵ�κ�ԭ����������βε���Ŀ��ͬ
#define FUNC_REDEF_ERROR 9                                //���庯��ʱ�����ַ��ű��Ѿ���ͬ���ĺ���
#define VAR_REDEF_ERROR 10                                //��ͬ�����Ѿ���ͬ����������
#define ID_ERROR 11                                       //�����ͱ�ʶ�����治�ǵ�sym���Ǳ�ʶ��
#define CONST_DEF_ASSIGN_ERROR 12                         //�ڳ��������У�Ӧ���ǵȺŵ�λ�ò��ǵȺ�
#define CONST_DEF_TYPE_ERROR 13                           //���������У��Ⱥź����sym�����ͺͳ�����������Ͳ�һ�»���sym�������������ַ�
#define CONST_DEC_TYPE_ERROR 14                           //���������У����������ͱ�ʶ������int��char
#define SEMI_ERROR 15                                     //Ӧ���Ƿֺŵĵط����ɱ��sym����
#define VAR_DEF_ARRAYINDEX_ERROR 16                       //���������У���������������޷�������
#define RPAR_ERROR 17                                     //Ӧ��ʱ��С���ŵĵط�sym���Ͳ���ȷ
#define RBRA_ERROR 18                                     //Ӧ��ʱ�������ŵĵط�sym���Ͳ���ȷ
#define RBRAS_ERROR 19                                    //Ӧ��ʱ�Ҵ����ŵĵط�sym���Ͳ���ȷ
#define VAR_KIND_ERROR 20                                 //������еı�ʶ�����Ǳ���
#define MAINSY_ERROR 21                                   //Ӧ����main�����ֵĵط�sym���ʹ���
#define LPAR_ERROR 22                                     //Ӧ��ʱ��С���ŵĵط�sym���Ͳ���ȷ
#define LBRAS_ERROR 23                                    //Ӧ��ʱ������ŵĵط�sym���Ͳ���ȷ
#define AFTER_OP_DIGIT_ERROR 24                           //�������У�+-�ź���Ĳ����޷�������
#define FACTOR_ERROR 25                                   //��factor��������Ϊ������Ĳ��ֲ���factor,��ʶ���ʶ��ĵ�һ��sym������factor����һ����֧
#define STATE_AFTER_ID_ERROR 26                           //�����ɷַ����У������˱�ʶ����Ϊ��ʼ��������治��=,(,[�е�һ�֣�����Ҫ����
#define WRONG_RELA_OP_ERROR 27                            //����Ĺ�ϵ�����
#define WHILESY_ERROR 28                                  //ѭ������У�������do��state��Ӧ����while�����֣�������Ϊ����
#define READ_ARRAY_ERROR 29                               //������У�ֻ�ܽ������ֵ�浽��ͨ������
#define VOID_RT_ERROR 30                                  //��������У�void������return������ֵ
#define VAL_RT_ERROR 31                                   //��������У���void������return����û��ֵ
#define NOT_VOID_MAIN_ERROR 32                            //main��������ʱǰ�治��void
#define ID_DEC_ERROR 33                                   //��ʶ������ʱ������Ӧ���Ƕ��š���С���š��������Ż��߷ֺţ���������ı�ʶ������Ϊ�����ʽ(��)
#define NO_RT_FUNDEF_ERROR 34                             //�з���ֵ��������ʱû�з������
#define CASE_ERROR 35                                     //Ӧ����case�ĵط���������symbol
#define CASE_CONST_ERROR 36                               //case����Ӧ���ǳ����������򱨴�
#define COLON_ERROR 37                                    //Ӧ����ð�ŵĵط�������symbol
#define ASSIGN_ERROR 38                                   //Ӧ����=�ĵط�������symbol
#define FUNC_NO_DEF_ERROR 39                              //���õĺ���δ������߲��������Ͳ���
#define ID_NO_DEF_ERROR 40                                //��ʶ��δ����������Ͳ�ƥ��
//�м���������еı���
#define OUT_OF_CODE_ERROR 41                              //�м����ṹ�������������ٴ�Ŵ���
//�﷨�����������ӵ�����
#define CONST_ASSIGNMENT_ERROR 42                         //��������ֵ
#define ARRAY_ASSIGNMENT_ERROR 43                         //�������ֱ�ֱ�Ӹ�ֵ
#define CHAR_ASSIGNMENT_ERROR 44                          //char���ܱ�int��ֵ
//�Ż��еı���������ʾ�Լ�
#define OUT_OF_FUNC_BLOCK_ERROR 45                        //������̫��
#define OUT_OF_BASIC_BLOCK_ERROR 46                       //������̫��
#define OUT_OF_NODE_NUM 47                                //�ڵ����ߵ��������dag�Ľڵ���Ŀ̫��
#define OUT_OF_ONE_NODE_VAR 48                            //һ���ڵ��ϵı���������

extern FILE* compile_file;
extern FILE* output_file;     //�ʷ��������������ļ�
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
