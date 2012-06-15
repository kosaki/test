#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

my_printf(const char *format, ...)
{
	va_list ap;
	int val;

	va_start(ap, format);
	val = va_arg(ap, int);
	printf(format, val);
	va_end(ap);
}

int main(void)
{
	my_printf("%d\n", 42);

	return 0;
}
