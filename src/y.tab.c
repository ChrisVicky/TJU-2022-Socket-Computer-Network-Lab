/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20140715

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 8 "src/parser.y"
#include "parse.h"

/* Define YACCDEBUG to enable debug messages for this lex file */
/*#define YACCDEBUG*/
#define YYERROR_VERBOSE
#ifdef YACCDEBUG
#include <stdio.h>
#define YPRINTF(...) printf(__VA_ARGS__)
#else
#define YPRINTF(...)
#endif

/* yyparse() calls yyerror() on error */
void yyerror (const char *s);

void set_parsing_options(char *buf, size_t siz, Request *parsing_request);

/* yyparse() calls yylex() to get tokens */
extern int yylex();


/*
** Global variables required for parsing from buffer
** instead of stdin:
*/

/* Pointer to the buffer that contains input */
char *parsing_buf;

/* Current position in the buffer */
int parsing_offset;

/* Buffer size */
size_t parsing_buf_siz;

/* Current parsing_request Header Struct */
Request *parsing_request;

#line 50 "src/parser.y"
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
#line 72 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

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
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    1,    1,    1,    4,    4,    2,    2,    2,    2,    5,
    5,    3,    3,    3,    6,    7,    0,
};
static const YYINT yylen[] = {                            2,
    1,    1,    1,    1,    2,    1,    1,    1,    1,    1,
    3,    0,    1,    1,    6,    7,    3,
};
static const YYINT yydefred[] = {                         0,
    2,    3,    1,    0,    4,    0,    0,    0,    5,    0,
    0,    9,    8,    7,    6,   10,    0,   13,   14,    0,
   17,    0,    0,    0,    0,   11,    0,   15,    0,    0,
   16,
};
static const YYINT yydgoto[] = {                          4,
   15,   16,   23,    6,   17,    7,   11,
};
static const YYINT yysindex[] = {                      -245,
    0,    0,    0,    0,    0, -256, -245, -215,    0, -224,
 -233,    0,    0,    0,    0,    0, -227,    0,    0, -223,
    0, -215, -215, -207, -254,    0, -215,    0, -207, -239,
    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -216,
    0,    0,    0,    0,    0,    0, -208,    0,    0,    0,
    0,    0,    0, -208, -208,    0,    0,    0, -230,    0,
    0,
};
static const YYINT yygindex[] = {                         0,
    1,  -21,  -10,   48,    6,    0,    0,
};
#define YYTABLESIZE 60
static const YYINT yytable[] = {                         20,
    5,   26,   28,    1,    2,    3,    9,    5,   26,    8,
    9,   18,   19,   27,    1,    2,    3,   31,   30,   12,
    1,    2,    3,   21,   13,   14,   12,   25,   12,   12,
   12,   12,   29,   12,   12,    1,    2,    3,   22,   19,
   24,   18,   19,   12,    1,    2,    3,   12,   13,   14,
   12,   12,   12,   12,   10,   12,   12,    0,   18,   19,
};
static const YYINT yycheck[] = {                         10,
    0,   23,  257,  260,  261,  262,    6,    7,   30,  266,
   10,  266,  267,   24,  260,  261,  262,  257,   29,  259,
  260,  261,  262,  257,  264,  265,  257,   22,  259,  260,
  261,  262,   27,  264,  265,  260,  261,  262,  266,  267,
  264,  266,  267,  259,  260,  261,  262,  264,  264,  265,
  259,  260,  261,  262,    7,  264,  265,   -1,  266,  267,
};
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 268
#define YYUNDFTOKEN 278
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"t_crlf","t_backslash","t_slash",
"t_digit","t_dot","t_token_char","t_lws","t_colon","t_separators","t_sp","t_ws",
"t_ctl",0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : request",
"allowed_char_for_token : t_token_char",
"allowed_char_for_token : t_digit",
"allowed_char_for_token : t_dot",
"token : allowed_char_for_token",
"token : token allowed_char_for_token",
"allowed_char_for_text : allowed_char_for_token",
"allowed_char_for_text : t_separators",
"allowed_char_for_text : t_colon",
"allowed_char_for_text : t_slash",
"text : allowed_char_for_text",
"text : text ows allowed_char_for_text",
"ows :",
"ows : t_sp",
"ows : t_ws",
"request_line : token t_sp text t_sp text t_crlf",
"request_header : token ows t_colon ows text ows t_crlf",
"request : request_line request_header t_crlf",

};
#endif

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 227 "src/parser.y"

/* C code */

void set_parsing_options(char *buf, size_t siz, Request *request)
{
    parsing_buf = buf;
	parsing_offset = 0;
	parsing_buf_siz = siz;
    parsing_request = request;
}

void yyerror (const char *s) {fprintf (stderr, "%s\n", s);}
#line 257 "y.tab.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 2:
#line 114 "src/parser.y"
	{
	yyval.i = '0' + yystack.l_mark[0].i;
}
break;
case 4:
#line 123 "src/parser.y"
	{
	YPRINTF("token: Matched rule 1.\n");
	snprintf(yyval.str, 8192, "%c", yystack.l_mark[0].i);
}
break;
case 5:
#line 127 "src/parser.y"
	{
	YPRINTF("token: Matched rule 2.\n");
	memcpy(yyval.str, yystack.l_mark[-1].str, strlen(yystack.l_mark[-1].str));
	yyval.str[strlen(yystack.l_mark[-1].str)] = yystack.l_mark[0].i;
	yyval.str[strlen(yystack.l_mark[-1].str) + 1] = 0;
    /* snprintf($$, 8192, "%s%c", $1, $2);*/
}
break;
case 7:
#line 157 "src/parser.y"
	{
	yyval.i = yystack.l_mark[0].i;
}
break;
case 8:
#line 160 "src/parser.y"
	{
	yyval.i = yystack.l_mark[0].i;
}
break;
case 9:
#line 163 "src/parser.y"
	{
	yyval.i = yystack.l_mark[0].i;
}
break;
case 10:
#line 171 "src/parser.y"
	{
	YPRINTF("text: Matched rule 1.\n");
	snprintf(yyval.str, 8192, "%c", yystack.l_mark[0].i);
}
break;
case 11:
#line 175 "src/parser.y"
	{
	YPRINTF("text: Matched rule 2.\n");
	memcpy(yyval.str, yystack.l_mark[-2].str, strlen(yystack.l_mark[-2].str));
	memcpy(yyval.str + strlen(yystack.l_mark[-2].str), yystack.l_mark[-1].str, strlen(yystack.l_mark[-1].str));
	yyval.str[strlen(yystack.l_mark[-2].str) + strlen(yystack.l_mark[-1].str)] = yystack.l_mark[0].i;
	yyval.str[strlen(yystack.l_mark[-2].str) + strlen(yystack.l_mark[-1].str) + 1] = 0;
	/* snprintf($$, 8192, "%s%s%c", $1, $2, $3);*/
}
break;
case 12:
#line 187 "src/parser.y"
	{
	YPRINTF("OWS: Matched rule 1\n");
	yyval.str[0]=0;
}
break;
case 13:
#line 191 "src/parser.y"
	{
	YPRINTF("OWS: Matched rule 2\n");
	snprintf(yyval.str, 8192, "%c", yystack.l_mark[0].i);
}
break;
case 14:
#line 195 "src/parser.y"
	{
	YPRINTF("OWS: Matched rule 3\n");
	snprintf(yyval.str, 8192, "%s", yystack.l_mark[0].str);
}
break;
case 15:
#line 200 "src/parser.y"
	{
	YPRINTF("request_Line:\n%s\n%s\n%s\n",yystack.l_mark[-5].str, yystack.l_mark[-3].str,yystack.l_mark[-1].str);
    strcpy(parsing_request->http_method, yystack.l_mark[-5].str);
	strcpy(parsing_request->http_uri, yystack.l_mark[-3].str);
	strcpy(parsing_request->http_version, yystack.l_mark[-1].str);
}
break;
case 16:
#line 207 "src/parser.y"
	{
	YPRINTF("request_Header:\n%s\n%s\n",yystack.l_mark[-6].str,yystack.l_mark[-2].str);
    strcpy(parsing_request->headers[parsing_request->header_count].header_name, yystack.l_mark[-6].str);
	strcpy(parsing_request->headers[parsing_request->header_count].header_value, yystack.l_mark[-2].str);
	parsing_request->header_count++;
}
break;
case 17:
#line 221 "src/parser.y"
	{
	YPRINTF("parsing_request: Matched Success.\n");
	return SUCCESS;
}
break;
#line 564 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                yys = yyname[YYTRANSLATE(yychar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
