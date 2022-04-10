#include "kv.h"


void append_KV(CGI_ARG* arg, char* key, char *value){
	if(value==NULL){
		return;
	}
	arg->ENVP[arg->cnt] = (char*) malloc(strlen(key)+strlen(value)+2);
	memset(arg->ENVP[arg->cnt], 0, strlen(key)+strlen(value)+2);
	sprintf(arg->ENVP[arg->cnt], "%s=%s", key, value);
	arg->cnt++;
//	print_CGI(arg);
}

void init_CGI_ARG(CGI_ARG* arg){
//	printf("Start INIT\n");
	memset(arg->argc, 0, sizeof(arg->argc));
//	*(arg->argc) = (char*) malloc(sizeof(char*));
	arg->argv = 0;
	memset(arg->ENVP, 0, sizeof(arg->ENVP));
	arg->cnt = 0;
//	LOG("END INIT\n");
}

void free_CGI_ARG(CGI_ARG* arg){
	if(arg==NULL)
		return;
	int i;
	for(i=0;i<arg->cnt;i++){
		free(arg->ENVP[i]);
		arg->ENVP[i] = NULL;
	}
	for(i=0;i<arg->argv;i++){
		free(arg->argc[i]);
		arg->argc[i] = NULL;
	}
	free(arg);
	arg = NULL;
	return;
}

void append_arg(CGI_ARG*arg, char *narg){
	// If narg is null, skip
	if(narg==NULL) return;
//	printf("EEEE %s\n" ,narg);
	arg->argc[arg->argv] = (char*)malloc(strlen(narg));
	strncpy(arg->argc[arg->argv++], narg, strlen(narg));
//	print_CGI(arg);
	//arg->argc = (char**)realloc(arg->argc,sizeof(char**)*(arg->argv+1));
//	printf("END OF APPEND_ARG\n");
	return;
}
/*
void kv_to_ENVP(CGI_ARG* arg, char **ENVP, int*cnt_tot){
	printf("Begin convert it to ENVP\n");
	int i,cnt=0;
	for(i=0;i<arg->cnt;i++){
		size_t lkey = strlen(arg->kv[i].key);
		size_t lvalue = strlen(arg->kv[i].value);
		size_t tot_l = lkey + lvalue + 1;
		ENVP[cnt] = (char*)malloc(sizeof(char*)*tot_l);
		strncpy(ENVP[cnt], arg->kv[i].key, lkey);
		strncpy(ENVP[cnt]+lkey,"=",1);
		strncpy(ENVP[cnt]+lkey+1,arg->kv[i].value,lvalue);
		ENVP = (char**)realloc(ENVP, sizeof(char**)*(++cnt+1));
	}
	*cnt_tot = cnt;
}
*/
void print_CGI(CGI_ARG*arg){
	int i;
	printf("ARGS:\n");
	for(i=0;i<arg->argv;i++){
		printf("Arg  >>%s\n" ,arg->argc[i]);
	}
	printf("ENVP:\n");
	for(i=0;i<arg->cnt;i++){
		printf("Envp >> %s\n" ,arg->ENVP[i]);
	}
	printf("---------------------\n");

}
/*
int main(){
	CGI_ARG *arg = (CGI_ARG*) malloc(sizeof(CGI_ARG));
	init_CGI_ARG(arg);
	printf("INIT_END\n");
	append_KV(arg, "Content-Length", "10");
	append_arg(arg, "ARG");
	append_arg(arg, "NEWN");
	append_KV(arg, "NEW", "TET");
	print_CGI(arg);

	char **ENVP = (char**)malloc(sizeof(char**));
	int cnt = 0;
	kv_to_ENVP(arg, ENVP, &cnt);
	int i;
	for(i=0;i<cnt;i++){
		printf("%s\n" ,ENVP[i]);
	}
	return 0;
}
*/
