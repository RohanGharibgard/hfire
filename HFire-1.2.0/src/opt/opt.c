#include "opt.h"

#include <string.h>

#ifdef USE_POSIX_GETOPT
#   include "unistd.h"
#endif

#include "cio.h"

static
int
opt_is_param_type_flag(
    const opt_param_meta_t * param_meta)
{
    if ( param_meta->arg != NULL &&
        (strcmp(param_meta->arg, OPT_ARG_TRUE) == 0 ||
         strcmp(param_meta->arg, OPT_ARG_FALSE) == 0) )
    {
        return OPT_TRUE;
    }

    return OPT_FALSE;
}

#ifdef USE_POSIX_GETOPT
static
char *
opt_alloc_getopt_string(
    opt_param_meta_t ** param_meta, size_t num_param_meta)
{
    char * getopt_string = NULL;
    size_t p, getopt_string_size = 0;

    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->code != NULL )
        {
            getopt_string_size += strlen(param_meta[p]->code);
        }
    }

    getopt_string = (char *) malloc(sizeof(char) * (getopt_string_size + 1));
    if ( getopt_string == NULL )
    {
        return getopt_string;
    }

    getopt_string[0] = '\0';
    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->code != NULL )
        {
            strcat(getopt_string, param_meta[p]->code);
        }
    }

    return getopt_string;
}
#endif

int
opt_get_param_code_idx(
    opt_param_meta_t ** param_meta, size_t num_param_meta,
    char code, size_t * param_meta_idx)
{
    size_t p;

    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->code != NULL && param_meta[p]->code[0] == code )
        {
            *param_meta_idx = p;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

int
opt_get_param_name_idx(
    opt_param_meta_t ** param_meta, size_t num_param_meta,
    const char * name, size_t * param_meta_idx)
{
    size_t p;

    if ( name == NULL )
    {
        return EXIT_FAILURE;
    }

    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->name != NULL &&
            strcmp(param_meta[p]->name, name) == 0 )
        {
            *param_meta_idx = p;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

int
opt_is_param_name_set(
    opt_param_meta_t ** param_meta, size_t num_param_meta,
    const char * name)
{
    size_t idx;

    if ( !opt_get_param_name_idx(param_meta, num_param_meta, name, &idx) )
    {
        if ( opt_is_param_type_flag(param_meta[idx]) )
        {
            if ( strcmp(param_meta[idx]->arg, OPT_ARG_TRUE) == 0 )
            {
                return OPT_TRUE;
            }
        }
        else
        {
            if ( param_meta[idx]->arg != NULL &&
                strcmp(param_meta[idx]->arg, OPT_ARG_IS_REQUIRED) != 0 )
            {
                return OPT_TRUE;
            }
        }
    }

    return OPT_FALSE;
}

int
opt_is_missing_req_arg(
    opt_param_meta_t ** param_meta, size_t num_param_meta)
{
    size_t p;

    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->arg != NULL &&
            strcmp(param_meta[p]->arg, OPT_ARG_IS_REQUIRED) == 0 )
        {
            return OPT_TRUE;
        }
    }

    return OPT_FALSE;
}

int
opt_get_arg_as_list_of_string(
    const opt_param_meta_t * param_meta, const char * sepchr,
    char ** token, size_t * num_token, size_t max_arg)
{
    if ( cio_parse_line(param_meta->arg, sepchr, token, max_arg) )
    {
        return EXIT_FAILURE;
    }
    *num_token = cio_get_num_tokens(token, max_arg);
    return EXIT_SUCCESS;
}

int
opt_get_arg_as_list_of_long(
    const opt_param_meta_t * param_meta, const char * sepchr,
    long * token, size_t * num_token, size_t max_arg)
{
    char * arg = NULL;
    char ** str_token = NULL;
    size_t t;

    /* need to make copy of arg since we use strtok to parse line */
    if ( (arg = malloc(sizeof(char) * (strlen(param_meta->arg) + 1))) == NULL )
    {
        return EXIT_FAILURE;
    }
    strcpy(arg, param_meta->arg);
    if ( (str_token = malloc(sizeof(char *) * max_arg)) == NULL )
    {
        if ( arg != NULL )
        {
            free(arg);
        }
        return EXIT_FAILURE;
    }
    if ( cio_parse_line(arg, sepchr, str_token, max_arg) )
    {
        if ( str_token != NULL )
        {
            free(str_token);
        }
        if ( arg != NULL )
        {
            free(arg);
        }
        return EXIT_FAILURE;
    }
    *num_token = cio_get_num_tokens(str_token, max_arg);
    for(t = 0; t < *num_token; ++t)
    {
        token[t] = strtol(str_token[t], (char **) NULL, 10);
    }
    if ( str_token != NULL )
    {
        free(str_token);
    }
    if ( arg != NULL )
    {
        free(arg);
    }
    return EXIT_SUCCESS;
}

int
opt_get_arg_as_list_of_unsigned_long(
    const opt_param_meta_t * param_meta, const char * sepchr,
    unsigned long * token, size_t * num_token, size_t max_arg)
{
    char * arg = NULL;
    char ** str_token = NULL;
    size_t t;

    /* need to make copy of arg since we use strtok to parse line */
    if ( (arg = malloc(sizeof(char) * (strlen(param_meta->arg) + 1))) == NULL )
    {
        return EXIT_FAILURE;
    }
    strcpy(arg, param_meta->arg);
    if ( (str_token = malloc(sizeof(char *) * max_arg)) == NULL )
    {
        if ( arg != NULL )
        {
            free(arg);
        }
        return EXIT_FAILURE;
    }
    if ( cio_parse_line(arg, sepchr, str_token, max_arg) )
    {
        if ( str_token != NULL )
        {
            free(str_token);
        }
        if ( arg != NULL )
        {
            free(arg);
        }
        return EXIT_FAILURE;
    }
    *num_token = cio_get_num_tokens(str_token, max_arg);
    for(t = 0; t < *num_token; ++t)
    {
        token[t] = strtoul(str_token[t], (char **) NULL, 10);
    }
    if ( str_token != NULL )
    {
        free(str_token);
    }
    if ( arg != NULL )
    {
        free(arg);
    }
    return EXIT_SUCCESS;
}

int
opt_get_arg_as_list_of_double(
    const opt_param_meta_t * param_meta, const char * sepchr,
    double * token, size_t * num_token, size_t max_arg)
{
    char * arg = NULL;
    char ** str_token = NULL;
    size_t t;

    /* need to make copy of arg since we use strtok to parse line */
    if ( (arg = malloc(sizeof(char) * (strlen(param_meta->arg) + 1))) == NULL )
    {
        return EXIT_FAILURE;
    }
    strcpy(arg, param_meta->arg);
    if ( (str_token = malloc(sizeof(char *) * max_arg)) == NULL )
    {
        if ( arg != NULL )
        {
            free(arg);
        }
        return EXIT_FAILURE;
    }
    if ( cio_parse_line(arg, sepchr, str_token, max_arg) )
    {
        if ( str_token != NULL )
        {
            free(str_token);
        }
        if ( arg != NULL )
        {
            free(arg);
        }
        return EXIT_FAILURE;
    }
    *num_token = cio_get_num_tokens(str_token, max_arg);
    for(t = 0; t < *num_token; ++t)
    {
        token[t] = strtod(str_token[t], (char **) NULL);
    }
    if ( str_token != NULL )
    {
        free(str_token);
    }
    if ( arg != NULL )
    {
        free(arg);
    }
    return EXIT_SUCCESS;
}

static void
opt_fprintf_help_insert_padding(
    FILE * stream,
    size_t pad_size)
{
    while ( pad_size > 0 )
    {
        fprintf(stream, " ");
        --pad_size;
    }
}

static
void
opt_fprintf_help_name(
    FILE * stream,
    size_t longest_param_name_size,
    const char * code, const char * name)
{
    size_t pad_size;

    /* leading padding */
    pad_size = 2;
    opt_fprintf_help_insert_padding(stream, pad_size);

    if ( code != NULL )
    {
        fprintf(stream, "-%c,--%s", code[0], name);
    }
    else
    {
        fprintf(stream, "--%s   ", name);
    }

    /* trailing padding */
    pad_size = longest_param_name_size - strlen(name);
    opt_fprintf_help_insert_padding(stream, pad_size);
}

static
void
opt_fprintf_help_req(
    FILE * stream,
    const char * arg)
{
    size_t pad_size;

    /* leading padding */
    pad_size = 2;
    opt_fprintf_help_insert_padding(stream, pad_size);

    if ( arg != NULL && strcmp(arg, OPT_ARG_IS_REQUIRED) == 0 )
    {
        fprintf(stream, "%s", OPT_ARG_IS_REQUIRED);
    }
    else
    {
        pad_size = strlen(OPT_ARG_IS_REQUIRED);
        opt_fprintf_help_insert_padding(stream, pad_size);
    }
}

static
void
opt_fprintf_help_desc(
    FILE * stream,
    const char * desc)
{
    size_t pad_size;

    /* leading padding */
    pad_size = 2;
    opt_fprintf_help_insert_padding(stream, pad_size);

    fprintf(stream, "%s\n", desc);
}

static
void
opt_fprintf_help_default(
    FILE * stream,
    size_t longest_param_name_size,
    const char * name, const char * arg)
{
    size_t pad_size;

    /* leading padding */
    pad_size = longest_param_name_size + 22;
    opt_fprintf_help_insert_padding(stream, pad_size);

    fprintf(stream, "(default --%s '%s')\n", name, arg);
}

int
opt_fprintf_help(
    FILE * stream,
    const opt_app_meta_t * app_meta,
    opt_param_meta_t ** param_meta, size_t num_param_meta)
{
    size_t p, size, longest_param_name_size = 0;

    if ( stream == NULL )
    {
        return EXIT_FAILURE;
    }

    /* app metadata */
    fprintf(stream, "%s -- %s\n", app_meta->title, app_meta->version);
    fprintf(stream, "\n");
    if ( app_meta->desc != NULL )
    {
        fprintf(stream, "%s\n", app_meta->desc);
    }
    fprintf(stream, "Usage: %s {OPTIONS}\n", app_meta->name);
    fprintf(stream, "\n");

    /* find size of longest parameter name */
    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->name == NULL )
        {
            continue;
        }
        size = strlen(param_meta[p]->name);
        if ( size > longest_param_name_size )
        {
            longest_param_name_size = size;
        }
    }

    /* print help for each parameter */
    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->name == NULL )
        {
            continue;
        }
        opt_fprintf_help_name(stream, longest_param_name_size,
            param_meta[p]->code, param_meta[p]->name);
        opt_fprintf_help_req(stream, param_meta[p]->arg);
        if ( param_meta[p]->desc == NULL )
        {
            continue;
        }
        opt_fprintf_help_desc(stream, param_meta[p]->desc);
        if ( param_meta[p]->arg == NULL ||
            strcmp(param_meta[p]->arg, OPT_ARG_IS_REQUIRED) == 0 ||
            strcmp(param_meta[p]->arg, OPT_ARG_TRUE) == 0 ||
            strcmp(param_meta[p]->arg, OPT_ARG_FALSE) == 0 )
        {
            continue;
        }
        opt_fprintf_help_default(stream, longest_param_name_size,
            param_meta[p]->name, param_meta[p]->arg);
    }
    fprintf(stream, "\n");

    return ferror(stream);
}

int
opt_fprintf_missing_req_arg(
    FILE * stream,
    const opt_app_meta_t * app_meta,
    opt_param_meta_t ** param_meta, size_t num_param_meta)
{
    size_t p, is_missing = 0;

    if ( stream == NULL )
    {
        return EXIT_FAILURE;
    }

    for(p = 0; p < num_param_meta; ++p)
    {
        if ( param_meta[p]->arg != NULL &&
            strcmp(param_meta[p]->arg, OPT_ARG_IS_REQUIRED) == 0 )
        {
            fprintf(stream, "missing required argument for --%s\n",
                param_meta[p]->name);
            is_missing = 1;
        }
    }

    if ( is_missing )
    {
        fprintf(stream, "try `%s --help` for help running this program\n",
            app_meta->name);
    }

    return ferror(stream);
}

int
opt_fprintf_debug(
    FILE * stream,
    opt_param_meta_t ** param_meta, size_t num_param_meta)
{
    size_t p;

    if ( stream == NULL )
    {
        return EXIT_FAILURE;
    }

    for(p = 0; p < num_param_meta; ++p)
    {
        fprintf(stream, "code:   %s\n",
            (param_meta[p]->code == NULL ? "n/a" : param_meta[p]->code));
        fprintf(stream, "name:   %s\n",
            (param_meta[p]->name == NULL ? "n/a" : param_meta[p]->name));
        fprintf(stream, "desc:   %s\n",
            (param_meta[p]->desc == NULL ? "n/a" : param_meta[p]->desc));
        fprintf(stream, "arg:    %s\n",
            (param_meta[p]->arg == NULL ? "n/a" : param_meta[p]->arg));
        fprintf(stream, "\n");
    }

    return ferror(stream);
}

#ifdef USE_POSIX_GETOPT
int
opt_getopt(
    int argc, char * const argv[],
    opt_param_meta_t ** param_meta, size_t num_param_meta)
{
    extern char * optarg;
    char * getopt_string = NULL;
    int c, status = EXIT_SUCCESS;
    size_t param_meta_idx;

    getopt_string = opt_alloc_getopt_string(param_meta, num_param_meta);
    if ( getopt_string == NULL )
    {
        return EXIT_FAILURE;
    }

    while ( (c = getopt(argc, argv, getopt_string)) != -1 )
    {
        switch(c)
        {
            case '?':
                status = EXIT_FAILURE;
                break;
            default:
                if ( opt_get_param_code_idx(param_meta, num_param_meta,
                    (char)c, &param_meta_idx) != EXIT_SUCCESS )
                {
                    status = EXIT_FAILURE;
                }
                else
                {
                    if ( optarg != NULL )
                    {
                        param_meta[param_meta_idx]->arg = optarg;
                    }
                    else
                    {
                        param_meta[param_meta_idx]->arg = OPT_ARG_TRUE;
                    }
                }
                break;
        }
    }

    if ( getopt_string != NULL )
    {
        free(getopt_string);
    }

    return status;
}
#else
int
opt_getopt(
    int argc, char * const argv[],
    opt_param_meta_t ** param_meta, size_t num_param_meta)
{
    size_t p;
    char * arg;
    size_t arglen;
    int a, arg_match_index, opt_match_index;

    for(p = 0; p < num_param_meta; ++p)
    {
        /* search for an argument matching parameter code or name */
        arg_match_index = 0, opt_match_index = 0;
        for(a = 0; a < argc; ++a)
        {
            arg = argv[a];
            arglen = strlen(arg);
            /* found match for single dash param code */
            if ( arglen == 2 && arg[0] == '-' )
            {
                if ( param_meta[p]->code != NULL &&
                    arg[1] == param_meta[p]->code[0] )
                {
                    arg_match_index = a;
                }
            }
            /* found match for double dash param name */
            else if ( arglen > 2 && arg[0] == '-' && arg[1] == '-' )
            {
                if ( strcmp(&(arg[2]), param_meta[p]->name) == 0 )
                {
                    arg_match_index = a;
                }
            }
            /* found option for a matching argument */
            else if ( arg_match_index == (a-1) )
            {
                opt_match_index = a;
            }
        }
        /* if a matching argument was found, assign option to parameter */
        if ( arg_match_index > 0 )
        {
            /* parameter is of type flag */
            if ( opt_is_param_type_flag(param_meta[p]) )
            {
                if ( opt_match_index == 0 )
                {
                    param_meta[p]->arg = OPT_ARG_TRUE;
                }
                else if ( strcmp(argv[opt_match_index], OPT_ARG_TRUE) == 0 )
                {
                    param_meta[p]->arg = OPT_ARG_TRUE;
                }
                else if ( strcmp(argv[opt_match_index], OPT_ARG_FALSE) == 0 )
                {
                    param_meta[p]->arg = OPT_ARG_FALSE;
                }
            }
            /* parameter is of type '-key value' */
            else if ( opt_match_index > 0 )
            {
                param_meta[p]->arg = argv[opt_match_index];
            }
        }
    }

    return EXIT_SUCCESS;
}
#endif

/* end of opt.c */
