#include<util.h>
#include<string.h>
#include<stdio.h>
void helper(int t, char c){
	int i=0;
	for(i=0;i<t;i++) printf("%c" ,c);
}

void PRINTHEAD(int port){
	char out[1000] = {0};
	sprintf(out, "START LISTENING AT 127.0.0.1 : %4d",port);
	int pre = 15;
	int len = strlen(out);
	int tot = 1 + pre + 1 + len + 1 + pre + 1;
	printf("+");
	helper(tot-2, '-');
	printf("+\n");
	printf("|");
	helper(pre + 1, ' ');
	PRINT4("%s" ,out);
	helper(pre+1, ' ');
	printf("|\n");
	printf("+");
	helper(tot-2,'-');
	printf("+\n");
}




