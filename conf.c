#include <libconfig.h>

int main(int argc, char **argv) {
	
	config_t cfg;
	
	config_init(&cfg);

	if (! config_read_file(&cfg, "example.cfg")) {
		
