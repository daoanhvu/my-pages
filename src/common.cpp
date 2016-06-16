#include "../include/common.h"
#include <stdlib.h>
#include <string.h>

/**
 * parst c-string to int using atoi
 */
int parseInt(const char *str, int len) {
	char strV[10];
	strV[9] = '\0';
	memcpy(strV, str, len);
	return atoi(strV);
}

/**
 * @param: end - exclusive end
 */
double parseDouble(const char *str, int start, int end, int *error) {
	int i;
	double val = 0;
	char isFloatingPoint = 0;
	long floating = 1;
	char negative = 1;

	*error = -1;
	if(str == NULL)
		return 0;

	if(str[start] == '-'){
		negative = -1;
		start++;
	}

	for(i=start; i<end; i++){

		if(str[i]=='\0')
			return 0;

		if((str[i]<48) || (str[i]>57)){
			if( str[i] == 46 && isFloatingPoint==0)
				isFloatingPoint = 1;
			else{
				*error = ERROR_PARSE_DOUBLE;
				/*printf(" Floating point ERROR F\n");*/
				return 0;
			}
		}else{
			if(isFloatingPoint){
				floating *= 10;
				val = val + (double)(str[i] - 48)/floating;
			}else
				val = val * 10 + (str[i] - 48);
		}
	}
	(*error) = 0;
	return val*negative;
}

int isTrimmedChar(char ch) {
	int res = 0;

	if( (ch==' ') || (ch=='\n') || (ch==0x0A) )
		res = 1;

	return res;
}
