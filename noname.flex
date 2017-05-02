%{
  #include "stdio.h"
  #include "stdlib.h"
  #include "lexer-utilities.h"
  #include "noname-parse.h"
  #include "noname-types.h"

  int num_lines = 0, num_chars = 0;
  extern YYSTYPE yylval;

  #define YY_NO_UNPUT   /* keep g++ happy */

  using namespace llvm;
  using namespace noname;
  
  extern void yyerror(char const *s);
  
  namespace noname {
    extern int noname_read(char *buf, int *result, int max_size);
  }

  /* Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
  * is returned in "result".
  */
  // #undef YY_INPUT
  // #define YY_INPUT(buf, result, max_size)                                 \
  //   if ((result = fread((char *)buf, sizeof(char), max_size, fin)) < 0) { \
  //     YY_FATAL_ERROR("read() in flex scanner failed");
  #undef YY_INPUT
  #define YY_INPUT(buf, result, max_size) noname_read(buf, &result, max_size)

  extern int curr_lineno;
  extern int verbose_flag;

  #define YY_NO_UNPUT   /* keep g++ happy */

  unsigned int comment = 0;
  extern FILE *fin; /* we read from this file */

  extern char string_buf[MAX_STR_CONST]; /* to assemble string constants */
  extern char *string_buf_ptr;
  extern unsigned int string_buf_left;
  extern bool string_error;
%}

%option noyywrap 
  // %option noyywrap nounput batch debug yylineno
  // %option warn noyywrap nodefault yylineno reentrant bison-bridge 

%x COMMENT
%x STRING

LINE_BREAK      \n
STMT_SEP        ;
LETTER          [a-zA-Z]
ALPHA           [a-zA-Z$_]
DIGIT           [0-9]
DIGITS          {DIGIT}+
LONG_TOK            {DIGIT}+
DOUBLE_TOK          {DIGIT}+(\.{DIGIT}+)?
IDENTIFIER              {ALPHA}({ALPHA}|{DIGIT})*

IMPORT          #import
ELSE_TOK            else
FALSE           false
IF_TOK              if
IN_TOK              in
LET_TOK             let
DEF_TOK             def
RETURN          return
LOOP_TOK            loop
THEN_TOK            then
WHILE           while
BREAK_TOK           break
CASE_TOK            case
NEW_TOK             new
NOT_TOK             not
TRUE            true
NEWLINE         [\n]
NOTNEWLINE      [^\n]
NOTSTRING       [^\n\0\\\"]
WHITESPACE      [ \t\r\f\v]+
ASSIGN          =
LE_TOK              <=
DARROW          =>
NULLCH          [\0]
BACKSLASH       [\\]
STAR_TOK            [*]
NOTSTAR         [^*]
LEFTPAREN_TOK       [(]
NOTLEFTPAREN_TOK    [^(]
RIGHTPAREN      [)]
NOTRIGHTPAREN   [^)]

LINE_COMMENT    "//"
START_COMMENT   "/*"
END_COMMENT     "*/"

QUOTES          \"


%%

{LINE_BREAK}       {
                        ++num_chars;
                        ++num_lines;
                      }

{START_COMMENT} {
  comment++;
  BEGIN(COMMENT);
}

<COMMENT><<EOF>> {
  yylval.error_msg = "EOF in comment";
  BEGIN(INITIAL);
  return (ERROR_TOK);
}

<COMMENT>{BACKSLASH}(.|{NEWLINE}) {
  backslash_common();
};

<COMMENT>{BACKSLASH}               ;

<COMMENT>{START_COMMENT} {
  comment++;
}

<COMMENT>{END_COMMENT} {
  comment--;
  if (comment == 0) {
    BEGIN(INITIAL);
  }
}

<COMMENT>.                      { ++num_chars; }

<INITIAL>{END_COMMENT} {
  yylval.error_msg = "Unmatched */";
  return (ERROR_TOK);
}

<INITIAL>{LINE_COMMENT}{NOTNEWLINE}*  ;

<INITIAL>{QUOTES} {
  BEGIN(STRING);
  string_buf_ptr = string_buf;
  string_buf_left = MAX_STR_CONST;
  string_error = false;
}

<STRING><<EOF>> {
  yylval.error_msg = "EOF in string constant";
  BEGIN(INITIAL);
  return ERROR_TOK;
}

<STRING>{NOTSTRING}* {
  int rc = str_write(yytext, strlen(yytext));
  if (rc != 0) {
    return (ERROR_TOK);
  }
}
<STRING>{NULLCH} {
  null_character_err();
  return (ERROR_TOK);
}

<STRING>{NEWLINE} {
  BEGIN(INITIAL);
  curr_lineno++;
  if (!string_error) {
    yylval.error_msg = "Unterminated string constant";
    return (ERROR_TOK);
  }
}
<STRING>{BACKSLASH}(.|{NEWLINE}) {
  char *c = backslash_common();
  int rc;

  switch (*c) {
    case 'n':
      rc = str_write("\n", 1);
      break;
    case 'b':
      rc = str_write("\b", 1);
      break;
    case 't':
      rc = str_write("\t", 1);
      break;
    case 'f':
      rc = str_write("\f", 1);
      break;
    case '\0':
      rc = null_character_err();
      break;
    default:
      rc = str_write(c, 1);
  }
  if (rc != 0) {
    return (ERROR_TOK);
  }
}
<STRING>{BACKSLASH}             ;

<STRING>{QUOTES} {
  BEGIN(INITIAL);
  if (!string_error) {
    yylval.id_v = copy_string(string_buf, string_buf_ptr - string_buf);
    return (STR_CONST);
  }
}


<*>{WHITESPACE}                  { ++num_chars; }
<INITIAL>{IMPORT}                { return (IMPORT); }
<INITIAL>{ASSIGN}                { return (ASSIGN); }
<INITIAL>{ELSE_TOK}                  { return (ELSE_TOK); }
<INITIAL>{IF_TOK}                    { return (IF_TOK); }
<INITIAL>{IN_TOK}                    { return (IN_TOK); }
<INITIAL>{LET_TOK}                   { return (LET_TOK); }
<INITIAL>{RETURN}                { return (RETURN); }
<INITIAL>{DEF_TOK}                   { return (DEF_TOK); }
<INITIAL>{THEN_TOK}                  { return (THEN_TOK); }
<INITIAL>{WHILE}                 { return (WHILE); }
<INITIAL>{CASE_TOK}                  { return (CASE_TOK); }
<INITIAL>{NEW_TOK}                   { return (NEW_TOK); }
<INITIAL>{NOT_TOK}                   { return (NOT_TOK); }
<INITIAL>{IDENTIFIER}      {
  yylval.id_v = strdup(yytext);
  return (IDENTIFIER); }
<INITIAL>{LONG_TOK}     {
  yylval.long_v = atoi(strdup(yytext));
  return (LONG_TOK); }
<INITIAL>{DOUBLE_TOK}  {
  yylval.double_v = atof(strdup(yytext));
  return (DOUBLE_TOK); }

<INITIAL>","                     { return int(','); }
<INITIAL>":"                     { return int(':'); }
<INITIAL>"{"                     { return int('{'); }
<INITIAL>"}"                     { return int('}'); }
<INITIAL>"+"                     { return int('+'); }
<INITIAL>"-"                     { return int('-'); }
<INITIAL>"*"                     { return int('*'); }
<INITIAL>"/"                     { return int('/'); }
<INITIAL>"<"                     { return int('<'); }
<INITIAL>"~"                     { return int('~'); }
<INITIAL>"."                     { return int('.'); }
<INITIAL>"@"                     { return int('@'); }
<INITIAL>"("                     { return int('('); }
<INITIAL>")"                     { return int(')'); }
<INITIAL>"&"                     { return int('&'); }
<INITIAL>";"                     { return int(STMT_SEP); }

<INITIAL>. {
    printf("lexer error '%s'", yytext);
    yylval.error_msg = yytext; return 0; 
  }

%%