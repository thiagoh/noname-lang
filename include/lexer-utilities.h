
#ifndef _LEXER_UTILITIES_H
#define _LEXER_UTILITIES_H

// extern int str_write(char *str, unsigned int len);
// extern int null_character_err();
// extern char *backslash_common();

/* Max size of string constants */
#define MAX_STR_CONST 1025
extern char string_buf[MAX_STR_CONST]; /* to assemble string constants */
extern char *string_buf_ptr;
extern unsigned int string_buf_left;
extern bool string_error;

extern char* copy_string(char *s, int len);
extern int str_write(char *str, unsigned int len);
extern int null_character_err();
extern char * backslash_common();

#endif
