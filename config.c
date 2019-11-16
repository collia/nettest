#include <stdio.h>
#include <stdbool.h>
#include <libconfig.h>
#include <syslog.h>
#include <string.h>
#include "nettest.h"
#include "config.h"

int parse_config(rules_t *config, char * filename) {
    config_t cfg;
    config_setting_t *setting;
    const char *str;

    syslog(LOG_INFO,"Parsing %s\n", filename);
    
    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    if(! config_read_file(&cfg, filename))
    {
        syslog(LOG_ERR, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(-1);
    }

    setting = config_lookup(&cfg, "rules");
    
    if(setting) {
        int count = config_setting_length(setting);
        if(count >= MAX_RULES_NUM) {
            syslog(LOG_ERR, "Too many rules %d, supported only %d\n", count, MAX_RULES_NUM );
            config_destroy(&cfg);
            return(-1);
        }
        config->rules_num = count;
        for(int i = 0; i < count; i++) {
            config_setting_t *rule = config_setting_get_elem(setting, i);

            const char *input;
            const char *output;
            if(!(config_setting_lookup_string(rule, "input", &input) &&
                 config_setting_lookup_string(rule, "output", &output))) {
                   syslog(LOG_ERR, "No required fields in rule %d\n", i);
                   continue;
               } else {
                   syslog(LOG_INFO,"Redirect from: %s to %s\n", input, output);
                   strncpy(config->rules[i].in, input, MAX_INTERFACE_NAME_LEN);
                   strncpy(config->rules[i].out, output, MAX_INTERFACE_NAME_LEN);
               }
        }
    }
    else {
        fprintf(stderr, "No 'rules' setting in configuration file.\n");
    }

    config_destroy(&cfg);
    return 0;
    
}

