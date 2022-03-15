#define t_crlf 257
#define t_backslash 258
#define t_slash 259
#define t_digit 260
#define t_dot 261
#define t_token_char 262
#define t_lws 263
#define t_colon 264
#define t_separators 265
#define t_sp 266
#define t_ws 267
#define t_ctl 268
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	char str[8192];
	int i;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;
