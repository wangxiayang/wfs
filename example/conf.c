#include <stdlib.h>
#include <libconfig.h>
#include "common.h"

int main(int argc, char **argv) {
	
	config_t cfg;
	
	config_init(&cfg);

	if (! config_read_file(&cfg, "example.cfg")) {
		red_printf("conf read file failure\n");
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}

	const char *str;
	if (config_lookup_string(&cfg, "name", &str)) {
		green_printf("[name] %s\n", str);
	} else {
		red_printf("no entry for name\n");
	}

	config_destroy(&cfg);
	return 0;
}
