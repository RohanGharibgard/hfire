#ifndef CIO_H
#define CIO_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
    extern "C" {
#endif

#define CIO_TRUE    (1)
#define CIO_FALSE   (0)

typedef struct
{
    const char * key;
    char * val;
}
cio_file_head_t;

int cio_read_line(
    FILE * stream,
    char ** line, size_t max_char_line);

int cio_copy_line(
    char ** dest, char * src, size_t max_char_dest);

int cio_is_cmt_line(
    const char * line, const char * cmtchr);

int cio_parse_line(
    char * line, const char * sepchr,
    char ** token, size_t max_token_line);

int cio_parse_cmt_line(
    char * line, const char * hdr_sepchr,
    char ** key, char ** val);

int cio_read_file(
    FILE * stream,
    const char * sepchr, const char * cmtchr,
    size_t max_char_line, size_t max_token_line,
    int (*parse_rec)(char ** token, size_t max_token_line, void * rec),
    void * rec);

size_t cio_get_num_tokens(char ** token, size_t max_token_line);

int cio_fprintf_tokens(
    FILE * stream, 
    char ** token, size_t max_token_line,
    const char * sepchr, int print_trailing_newline);

int cio_set_file_head_val(
    cio_file_head_t ** file_head, size_t num_file_head,
    const char * key, char * val);

int cio_fprintf_file_head(
    FILE * stream,
    cio_file_head_t ** file_head, size_t num_file_head,
    const char * sepchr, const char * cmtchr);

const char * cio_get_file_head_val(
    cio_file_head_t ** file_head, size_t num_file_head,
    const char * key, const char * default_val);

#ifdef __cplusplus
    }
#endif

#endif

/* end of cio.h */
