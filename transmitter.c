#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "nettest.h"
#include "interface.h"
#include "transmitter.h"

static void dump_packet(packet_t *packet) {
#ifdef DEBUG

    #define PRINT_LEN 64
    char buffer[PRINT_LEN*3+PRINT_LEN/16];
    char* buffer_ptr = buffer;
    for(int i = 0; i< PRINT_LEN && i < packet->data_size; i++)
    {
        if(i%16 == 0 && i != 0) {
            *buffer_ptr = '\n';
            buffer_ptr++;
        }
        buffer_ptr += sprintf(buffer_ptr, "%02hhx ", packet->packet[i]);
    }

    syslog(LOG_DEBUG,"Pkt:\n%s", buffer);
#endif
}

static void *transmit_function(void *ptr )
{
    int rc;
    packet_t *packet = malloc(sizeof(packet_t));
    rule_handle_t *rule = (rule_handle_t*)ptr;
    syslog(LOG_INFO, "Started retransmitter");
    while(1) {
        rc = receive_packet(rule->in,
                            packet);
        if(rc < 0) {
            syslog(LOG_ERR, "Failed receive packet");
            break;
        }
        dump_packet(packet);
        syslog(LOG_DEBUG, "transfered %d bytes", packet->data_size);
        rc = transmit_packet(rule->out,
                             packet);
        if(rc < 0) {
            syslog(LOG_ERR, "Failed transmit packet");
            break;
        }
    }
    free(packet);
}

rule_handle_t* init_rule(rule_t *rule) {
    pthread_t thread;
    int ret;
    rule_handle_t *hdl;
    if_handle* in;
    if_handle* out;

    hdl = malloc(sizeof(rule_handle_t));
    if(!hdl) {
        return NULL;
    }

    in = init_input_interface(rule);
    if(!in) {
        syslog(LOG_ERR, "Error opening interface %s\n", rule->in);
        return NULL;
    }
    out = init_output_interface(rule);
    if(!out) {

        syslog(LOG_ERR, "Error opening interface %s\n", rule->out);
        return NULL;
    }

    hdl->in = in;
    hdl->out = out;
    ret = pthread_create( &thread, NULL, transmit_function, (void*) hdl);
    if(ret)
    {
        syslog(LOG_ERR, "Error - pthread_create() return code: %d\n", ret);
        return NULL;
    }
    hdl->thread = thread;
    return hdl;
}


void start_rule(rule_handle_t *rule) {
    pthread_join(rule->thread, NULL);
}
