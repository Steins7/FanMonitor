#include "utils.h"

uint32_t num2str(char *s, int number, uint8_t base) {

	static char hexChars[] = "0123456789ABCDEF";
	char *p = s;

	// manage sign
	uint32_t nb = (number < 0 ? -number : number);

	// get digits
	do {
		*s++ = hexChars[nb % base];
	} while (nb /= base);

	// finalize string
	if(number < 0) *s++ = '-';
	*s='\0';

	// reverse string
	uint32_t cnt = s - p;
	char tmp;
	for(int i=0; i<cnt/2; ++i) {
		tmp = p[i];
		p[i] = p[cnt-i-1];
		p[cnt-i-1] = tmp;
	}

	return cnt; //number of caracters (excluding '\0')
}
