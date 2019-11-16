
typedef struct {
    int socket;
} if_handle;

if_handle* init_input_interface(rule_t *cfg);
if_handle* init_output_interface(rule_t *cfg);
void close_interface(if_handle *cfg);
