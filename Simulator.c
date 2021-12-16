#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int Hex2Digit(char hex) {
	int dig = 0;
	if ('0' <= hex && hex <= '9')
		dig += hex - '0';
	else if ('A' <= hex && hex <= 'F')
		dig += hex - 'A' + 10;
	return dig;
}

int Hex2Num(char* hex) {
	int L = strlen(hex);
	int num = 0;
	char* reverse_hex = _strrev(hex);
	for (int i = 0;i<L;i++) //run backwards and sum over the digits with corresponding powers.
	{
		//printf("%d\n", num);
		num += Pow(16, i)*Hex2Digit(reverse_hex[i]);
	}
	return num;
}

int Pow(int base, int power) {
	int result = 1;
	for (int i = 0;i < power;i++) {
		result *= base;
	}
	return result;
}



void main()
{
	char a[]= "F4367C";
	//char* bla = _strrev(a);
	//printf("%s\n", bla);
	printf("%d\n", Hex2Num(a));
	//printf("%d", Pow(16,2));


}
