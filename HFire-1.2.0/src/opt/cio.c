#include "cio.h"

#include <string.h>
#include <ctype.h>

int 
cio_read_line(
    FILE * stream,
    char ** line, size_t max_char_line)
{
    size_t idx = 0;
    int c;

    if ( stream == NULL )
    {
        return EXIT_FAILURE;
    }

    --max_char_line; /* save one character for nul */
    while ( (c = fgetc(stream)) != EOF && idx < max_char_line )
    {
        if ( (char)c == '\n' )
        {
            break;
        }
        if ( (char)c == '\r' )
        {
            /* read and discard newline following linefeed */
            fgetc(stream);
            break;
        }
        (*line)[idx++] = (char)c;
    }
    (*line)[idx] = '\0';

    if ( ferror(stream) || idx >= max_char_line )
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int
cio_copy_line(
    char ** dest, char * src, size_t max_char_dest)
{
    size_t idx = 0;

    if ( dest == NULL || src == NULL )
    {
        return EXIT_FAILURE;
    }

    while ( src[idx] != '\0' && idx < max_char_dest )
    {
        (*dest)[idx] = src[idx];
        ++idx;
    }

    return EXIT_SUCCESS;
}

int
cio_is_cmt_line(
    const char * line, const char * cmtchr)
{
    size_t idx = 0;

    if ( line == NULL )
    {
        return CIO_FALSE;
    }

    while ( cmtchr != NULL && cmtchr[idx] != '\0' )
    {
        if ( line[0] == cmtchr[idx] )
        {
            return CIO_TRUE;
        }
        ++idx;
    }

    return CIO_FALSE;
}

int
cio_parse_line(
    char * line, const char * sepchr,
    char ** token, size_t max_token_line)
{
    size_t idx = 0;

    if ( line == NULL )
    {
        return EXIT_FAILURE;
    }

    token[idx] = strtok(line, sepchr);
    while ( token[idx] != NULL && idx < max_token_line )
    {
        /* fprintf(stdout, "%s<---->", token[idx]); */
        token[++idx] = strtok(NULL, sepchr);
    }
 
    if ( idx >= max_token_line )
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int 
cio_parse_cmt_line(
    char * line, const char * hdr_sepchr,
    char ** key, char ** val)
{
    char * psepchr = NULL;

    if ( line == NULL || strlen(line) < 2 )
    {
        return EXIT_FAILURE;
    }

    /* ignore the leading comment character */
    if ( (psepchr = strstr(++line, hdr_sepchr)) == NULL )
    {
        return EXIT_FAILURE;
    }

    /* trim whitespace surrounding key */
    *psepchr = '\0';
    while ( *line != '\0' && isspace(*line) )
    {
        ++line;
    }
    *key = line;
    line = psepchr;
    do
    {
        *line = '\0';
        --line;
    } while ( line > *key && isspace(*line) );

    /* value is string following header separator */
    line = ++psepchr;

    /* trim whitespace or quotes surrounding value */
    while ( *line != '\0' && ( isspace(*line) || *line == '"' ) )
    {
        ++line;
    }
    *val = line;
    while ( *line != '\0' && *line != '"' ) 
    {
        ++line;
    }
    *line = '\0';

    if ( key == NULL || val == NULL )
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int
cio_read_file(
    FILE * stream,
    const char * sepchr, const char * cmtchr,
    size_t max_char_line, size_t max_token_line,
    int (*parse_rec)(char ** token, size_t max_token_line, void * rec),
    void * rec)
{
    char * line;
    char ** token;

    if ( stream == NULL )
    {
        return EXIT_FAILURE;
    }
    if ( (line = (char *) malloc(sizeof(char) * max_char_line)) == NULL )
    {
        return EXIT_FAILURE;
    }
    if ( (token = (char **) malloc(sizeof(char *) * max_token_line)) == NULL )
    {
        return EXIT_FAILURE;
    }

    while ( !feof(stream) )
    {
        if ( cio_read_line(stream, &line, max_char_line) )
        {
            return EXIT_FAILURE;
        }
        if ( line == NULL || line[0] == '\0' )
        {
            continue;
        }
        if ( cio_is_cmt_line(line, cmtchr) == CIO_TRUE )
        {
            continue;
        } 
        if ( cio_parse_line(line, sepchr, token, max_token_line) )
        {
            return EXIT_FAILURE;
        }
        if ( parse_rec(token, max_token_line, &rec) )
        {
            return EXIT_FAILURE;
        }
    }

    if ( token != NULL )
    {
        free(token);
    }
    if ( line != NULL )
    {
        free(line);
    }

    return EXIT_SUCCESS;
}

size_t
cio_get_num_tokens(
    char ** token, size_t max_token_line)
{
    size_t idx;
    size_t num_tokens = 0;

    for(idx = 0; token[idx] != NULL && idx < max_token_line; ++idx)
    {
        ++num_tokens;
    }

    return num_tokens;
}

int
cio_fprintf_tokens(
    FILE * stream, 
    char ** token, size_t max_token_line,
    const char * sepchr, int print_trailing_newline)
{
    size_t idx;

    if ( stream == NULL )
    {
        return EXIT_FAILURE;
    }

    for(idx = 0; token[idx] != NULL && idx < max_token_line; ++idx)
    {
        if ( idx > 0 )
        {
            fprintf(stream, "%s", sepchr);
        }
        fprintf(stream, "%s", token[idx]);
    }
    if ( print_trailing_newline )
    {
        fprintf(stream, "\n");
    }

    return ferror(stream);
}

int
cio_set_file_head_val(
    cio_file_head_t ** file_head, size_t num_file_head,
    const char * key, char * val)
{
    size_t h;
    size_t val_strlen;

    for(h = 0; h < num_file_head; ++h)
    {
        if ( file_head[h]->key != NULL && val != NULL &&
            strcmp(file_head[h]->key, key) == 0 )
        {
            val_strlen = strlen(val);
            if ( (file_head[h]->val = 
                malloc((val_strlen + 1) * sizeof(char))) == NULL )
            {
                return EXIT_FAILURE;
            }
            strcpy(file_head[h]->val, val);
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

int
cio_fprintf_file_head(
    FILE * stream,
    cio_file_head_t ** file_head, size_t num_file_head,
    const char * sepchr, const char * cmtchr)
{
    size_t h;

    if ( stream == NULL )
    {
        return EXIT_FAILURE;
    }

    for(h = 0; h < num_file_head; ++h)
    {
        if ( file_head[h]->key == NULL )
        {
            continue;
        }
        fprintf(stream, "%s %s %s %s\n",
            cmtchr,
            file_head[h]->key,
            sepchr,
            (file_head[h]->val == NULL ? "n/a" : file_head[h]->val));
    }

    return ferror(stream);
}

const char * 
cio_get_file_head_val(
    cio_file_head_t ** file_head, size_t num_file_head,
    const char * key, const char * default_val)
{
    size_t h;

    for(h = 0; h < num_file_head; ++h)
    {
        if ( file_head[h]->key != NULL && file_head[h]->val != NULL && 
            strcmp(file_head[h]->key, key) == 0 )
        {
            return file_head[h]->val;
        }
    }

    return default_val;
}

/* end of cio.c */
