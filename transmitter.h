

typedef struct {
    if_handle* in;
    if_handle* out;
    pthread_t thread;
} rule_handle_t;

rule_handle_t* init_rule(rule_t *rule);
void start_rule(rule_handle_t *rule);
