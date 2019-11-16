
#define MAX_INTERFACE_NAME_LEN 64
#define MAX_RULES_NUM 4

typedef struct {
    char in[MAX_INTERFACE_NAME_LEN];
    char out[MAX_INTERFACE_NAME_LEN];
} rule_t;

typedef struct {
    rule_t rules[MAX_RULES_NUM];
    int rules_num;
} rules_t;



