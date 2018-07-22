#include <stdio.h>
#include <locale.h>
#include <nnpack.h>
/* Import user configuration: */
#include <uk/config.h>

int main(int argc, char *argv[])
{
	printf("Hello world!\n");
	const enum nnp_status status = nnp_initialize();
}
