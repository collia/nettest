

typedef struct {
    if_handle* in;
    if_handle* out;
    pthread_t thread;
} rule_handle;

rule_handle* init_rule(rule_t *rule, if_handle* in, if_handle* out);
void start_rule(rule_handle *rule);
