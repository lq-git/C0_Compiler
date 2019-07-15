#define MAX_FUNC_BLOCK 128
#define MAX_BASIC_BLOCK 256
#define MAX_NODE_NUM 100
#define MAX_NUM_ONE_NODE 10

//����ṹ
struct func_block {
	int start_num;
	int basic_blocks[MAX_BASIC_BLOCK];
	int b_block_index;
};

struct func_blocks {
	struct func_block FUNC_LIST[MAX_FUNC_BLOCK];
	int func_index;
};

struct node {
	char name[100];
	int node_num;  //�ڵ����
};

struct node_table {
	struct node NODE_LIST[MAX_NODE_NUM];
	int var_num;
};

struct node_for_dag {
	int var_num;  //һ���ڵ��Ӧ�ı����ж���
	int op;
	int left_son_num;
	int right_son_num;
	char name[MAX_NUM_ONE_NODE][100];
};

struct like_dag {
	struct node_for_dag NODE_LIST[MAX_NODE_NUM];
	int node_num;
};

extern struct func_blocks FUNC_BLOCKS;
extern void  init_func_list();