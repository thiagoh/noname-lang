%{
  #include "stdio.h"
  #include "stdlib.h"
  #include "lexer-utilities.h"
  #include "noname-parse.h"
  #include "noname-types.h"

  int num_lines = 0, num_chars = 0;
  extern YYSTYPE yylval;
  extern void yyerror(char const *s);
  
  extern int curr_lineno;
  extern int verbose_flag;

  #define YY_NO_UNPUT   /* keep g++ happy */

  unsigned int comment = 0;

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
LONG            {DIGIT}+
DOUBLE          {DIGIT}+(\.{DIGIT}+)?
ID              {ALPHA}({ALPHA}|{DIGIT})*

ELSE            else
FALSE           false
IF              if
IN              in
LET             let
DEF             def
RETURN          return
LOOP            loop
THEN            then
WHILE           while
BREAK           break
CASE            case
NEW             new
NOT             not
TRUE            true
NEWLINE         [\n]
NOTNEWLINE      [^\n]
NOTSTRING       [^\n\0\\\"]
WHITESPACE      [ \t\r\f\v]+
ASSIGN          =
LE              <=
DARROW          =>
NULLCH          [\0]
BACKSLASH       [\\]
STAR            [*]
NOTSTAR         [^*]
LEFTPAREN       [(]
NOTLEFTPAREN    [^(]
RIGHTPAREN      [)]
NOTRIGHTPAREN   [^)]

LINE_COMMENT    "--"
START_COMMENT   "/*"
END_COMMENT     "*/"

QUOTES          \"


%%

{LINE_BREAK}                    {
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
  return (ERROR);
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
  return (ERROR);
}



<INITIAL>{QUOTES} {
  BEGIN(STRING);
  string_buf_ptr = string_buf;
  string_buf_left = MAX_STR_CONST;
  string_error = false;
}

<STRING><<EOF>> {
  yylval.error_msg = "EOF in string constant";
  BEGIN(INITIAL);
  return ERROR;
}

<STRING>{NOTSTRING}* {
  int rc = str_write(yytext, strlen(yytext));
  if (rc != 0) {
    return (ERROR);
  }
}
<STRING>{NULLCH} {
  null_character_err();
  return (ERROR);
}

<STRING>{NEWLINE} {
  BEGIN(INITIAL);
  curr_lineno++;
  if (!string_error) {
    yylval.error_msg = "Unterminated string constant";
    return (ERROR);
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
    return (ERROR);
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
<INITIAL>{ASSIGN}                { return (ASSIGN); }
<INITIAL>{ELSE}                  { return (ELSE); }
<INITIAL>{IF}                    { return (IF); }
<INITIAL>{IN}                    { return (IN); }
<INITIAL>{LET}                   { return (LET); }
<INITIAL>{RETURN}                { return (RETURN); }
<INITIAL>{DEF}                   { return (DEF); }
<INITIAL>{THEN}                  { return (THEN); }
<INITIAL>{WHILE}                 { return (WHILE); }
<INITIAL>{CASE}                  { return (CASE); }
<INITIAL>{NEW}                   { return (NEW); }
<INITIAL>{NOT}                   { return (NOT); }
<INITIAL>{ID}      {
  yylval.id_v = strdup(yytext);
  return (ID); }
<INITIAL>{LONG}     {
  yylval.long_v = atoi(strdup(yytext));
  return (LONG); }
<INITIAL>{DOUBLE}  {
  yylval.double_v = atof(strdup(yytext));
  return (DOUBLE); }

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