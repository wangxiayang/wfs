#include <stdarg.h>
#include <stdio.h>

int red_printf(const char *format, ...) {
	
	printf("\33[0;31m");

	va_list args;
	va_start(args, format);
	
	int res = vprintf(format, args);

	printf("\33[m");
	return res;
}

int green_printf(const char *format, ...) {

	printf("\33[0;32m");

	va_list args;
	va_start(args, format);

	int res = vprintf(format, args);

	printf("\33[m");
	return res;
}

int yellow_printf(const char *format, ...) {

	printf("\33[0;33m");

	va_list args;
	va_start(args, format);

	int res = vprintf(format, args);

	printf("\33[m");
	return res;
}

int blue_printf(const char *format, ...) {

	printf("\33[0;34m");

	va_list args;
	va_start(args, format);

	int res = vprintf(format, args);

	printf("\33[m");
	return res;
}
