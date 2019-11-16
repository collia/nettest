#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "nettest.h"
#include "interface.h"
#include "transmitter.h"

static void *transmit_function(void *ptr )
{
    syslog(LOG_INFO, "Thread");
}

rule_handle* init_rule(rule_t *rule, if_handle* in, if_handle* out) {
    pthread_t thread;
    int ret;
    rule_handle *hdl = malloc(sizeof(rule_handle));
    if(!hdl) {
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


void start_rule(rule_handle *rule) {
    pthread_join(rule->thread, NULL);
}

