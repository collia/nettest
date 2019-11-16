
#include <stdint.h>

#define MAX_PACKET_SIZE 65535

typedef struct {
    int socket;
    int ifindex;
} if_handle;

typedef struct {
    uint8_t packet[MAX_PACKET_SIZE];
    int data_size;
} packet_t;

if_handle* init_input_interface(rule_t *cfg);
if_handle* init_output_interface(rule_t *cfg);
void close_interface(if_handle *cfg);

int receive_packet(if_handle* hdl, packet_t* buffer);
int transmit_packet(if_handle* hdl, packet_t* buffer);
