#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <syslog.h>
#include "nettest.h"
#include "config.h"
#include "interface.h"
#include "transmitter.h"

static rules_t rules_config;
    
void print_help() {
    printf("Net test utility. \n");
    printf("\t-c <config> config files \n");
    printf("\t-v verbose\n");
    printf("\t-l log level 0-7\n");
    printf("\t-h show this help \n");
}

int main(int argc, char** argv ) {
    int c;
    bool verbose = false;
    int log_lvl = LOG_UPTO(LOG_WARNING);
    char *config_file = NULL;
    printf("Nettest utility\n");

    while ((c = getopt (argc, argv, "hvl:c:")) != -1) {
        switch (c)
        {
        case 'c':
            config_file = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case 'l':
            log_lvl = LOG_UPTO(atoi(optarg));
            break;
        case 'h':
            print_help();
            return 0;
            break;
        case '?':
            if (optopt == 'c')
                syslog(LOG_ERR, "Option -%c requires an argument - config file.\n", optopt);
            else if (optopt == 'l')
                syslog(LOG_ERR, "Option -%c requires an argument - log level.\n", optopt);
            else if (isprint (optopt))
                syslog(LOG_ERR, "Unknown option `-%c'.\n", optopt);
            else
                syslog(LOG_ERR,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            goto error;
        default:
            abort ();
        }
    }
    if(verbose) {
        openlog("nettest", LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
    } else {
        openlog("nettest", LOG_PID, LOG_USER);
    }
    setlogmask(log_lvl);
    if(config_file == NULL) {
        syslog(LOG_ERR, "config file option required\n");
        goto error;
    }
    if(parse_config(&rules_config, config_file)) {
        syslog(LOG_ERR, "Error parsing config %s\n", config_file);
        goto error;
    }

    for(int i=0; i< rules_config.rules_num; i++)
    {
        if_handle *in;
        if_handle *out;
        rule_handle *rule;

        in = init_input_interface(&rules_config.rules[i]);
        out = init_output_interface(&rules_config.rules[i]);
        if(!in) {
            syslog(LOG_ERR, "Error opening interface %s\n", rules_config.rules[i].in);
            goto error;
        }
        if(!out) {
            syslog(LOG_ERR, "Error opening interface %s\n", rules_config.rules[i].out);
            goto error;
        }
        rule = init_rule(&rules_config.rules[i], in, out);
        if(!rule) {
            syslog(LOG_ERR, "Error creating thread\n");
            goto error;
        }
        start_rule(rule);
        //close_interface(in);
        //close_interface(out);
    }
    
    closelog();
    return 0;
error:
    closelog();
    return 1;
}
