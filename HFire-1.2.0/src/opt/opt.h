#ifndef OPT_H
#define OPT_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
    extern "C" {
#endif

#define OPT_TRUE                    (1)
#define OPT_FALSE                   (0)

#define OPT_ARG_IS_REQUIRED         "$required"
#define OPT_ARG_TRUE                "$true"
#define OPT_ARG_FALSE               "$false"

#define OPT_DEFAULT_LIST_SIZE       (255)
#define OPT_DEFAULT_LIST_SEPCHR     ","

/* application metadata */

typedef struct
{
    /* application name */
    char * name;
    /* multi-string title */
    char * title;
    /* multi-line description */
    char * desc;
    /* version designator */
    char * version;
    /* copyright attribution */
    char * copyright;
}
opt_app_meta_t;

/* option metadata */

typedef struct
{
    /* (optional) a getopt format character code */
    char * code;
    /* longname associated with parameter */
    char * name;
    /* short one-line description of parameter */
    char * desc;
    /*
     *  argument associatd with parameter read from command line or
     *  { NULL, OPT_ARG_IS_REQUIRED, OPT_ARG_TRUE, OPT_ARG_FALSE }
     */
    char * arg;
}
opt_param_meta_t;

int opt_get_param_code_idx(
    opt_param_meta_t ** param_meta, size_t num_param_meta,
    char code, size_t * param_meta_idx);

int opt_get_param_name_idx(
    opt_param_meta_t ** param_meta, size_t num_param_meta,
    const char * name, size_t * param_meta_idx);

int opt_is_param_name_set(
    opt_param_meta_t ** param_meta, size_t num_param_meta,
    const char * name);

int opt_is_missing_req_arg(
    opt_param_meta_t ** param_meta, size_t num_param_meta);

int opt_get_arg_as_list_of_string(
    const opt_param_meta_t * param_meta, const char * sepchr,
    char ** token, size_t * num_token, size_t max_arg);

int opt_get_arg_as_list_of_long(
    const opt_param_meta_t * param_meta, const char * sepchr,
    long * token, size_t * num_token, size_t max_arg);

int opt_get_arg_as_list_of_unsigned_long(
    const opt_param_meta_t * param_meta, const char * sepchr,
    unsigned long * token, size_t * num_token, size_t max_arg);

int opt_get_arg_as_list_of_double(
    const opt_param_meta_t * param_meta, const char * sepchr,
    double * token, size_t * num_token, size_t max_arg);

int opt_fprintf_help(
    FILE * stream,
    const opt_app_meta_t * app_meta,
    opt_param_meta_t ** param_meta, size_t num_param_meta);

int opt_fprintf_missing_req_arg(
    FILE * stream,
    const opt_app_meta_t * app_meta,
    opt_param_meta_t ** param_meta, size_t num_param_meta);

int opt_fprintf_debug(
    FILE * stream,
    opt_param_meta_t ** param_meta, size_t num_param_meta);

int opt_getopt(
    int argc, char * const argv[],
    opt_param_meta_t ** param_meta, size_t num_param_meta);

#ifdef __cplusplus
    }
#endif

#endif

/* end of opt.h */
