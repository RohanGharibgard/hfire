#ifndef OPT_UTIL_H
#define OPT_UTIL_H

#include <stdlib.h>
#include <errno.h>

#include "opt.h"

#ifdef __cplusplus
    extern "C" {
#endif

/* extern int errno; */

/* macros for extracting primitive types from arguments */

#define GET_BOOL_ARG(optarg, param, num_param, name, idx) \
    do { \
        optarg = 0; \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            optarg = 1; \
        } \
    } while ( 0 )

#define GET_STRING_ARG(optarg, param, num_param, name, idx) \
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            optarg = param[idx]->arg; \
        } \
    } while ( 0 )

#define GET_LONG_ARG(optarg, param, num_param, name, idx) \
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            optarg = strtol(param[idx]->arg, (char **) NULL, 10); \
            if ( errno == ERANGE || errno == EINVAL ) \
            { \
                fprintf(stderr, \
                    "unable to extract signed integer from argument"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while ( 0 )

#define GET_UNSIGNED_LONG_ARG(optarg, param, num_param, name, idx) \
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            optarg = strtoul(param[idx]->arg, (char **) NULL, 10); \
            if ( errno == ERANGE || errno == EINVAL ) \
            { \
                fprintf(stderr, \
                    "unable to extract unsigned integer from argument"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while ( 0 )

#define GET_DOUBLE_ARG(optarg, param, num_param, name, idx) \
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            optarg = strtod(param[idx]->arg, (char **) NULL); \
            if ( errno == ERANGE || errno == EINVAL ) \
            { \
                fprintf(stderr, \
                    "unable to extract real number value from argument"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while ( 0 )

/* macros for extracting list of primitve types from arguments */

#define GET_LIST_OF_STRING_ARG(optarg, num, param, num_param, name, idx) \
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            if ( opt_get_arg_as_list_of_string( \
                param[idx], \
                OPT_DEFAULT_LIST_SEPCHR, \
                optarg, &num, \
                OPT_DEFAULT_LIST_SIZE) ) \
            { \
                fprintf(stderr, "unable to extract list from argument"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while ( 0 )

#define GET_LIST_OF_LONG_ARG(optarg, num, param, num_param, name, idx) \
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            if ( opt_get_arg_as_list_of_long( \
                param[idx], \
                OPT_DEFAULT_LIST_SEPCHR, \
                optarg, &num, \
                OPT_DEFAULT_LIST_SIZE) ) \
            { \
                fprintf(stderr, "unable to extract list from argument"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while ( 0 )

#define GET_LIST_OF_UNSIGNED_LONG_ARG(optarg, num, param, num_param, name, idx)\
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            if ( opt_get_arg_as_list_of_unsigned_long( \
                param[idx], \
                OPT_DEFAULT_LIST_SEPCHR, \
                optarg, &num, \
                OPT_DEFAULT_LIST_SIZE) ) \
            { \
                fprintf(stderr, "unable to extract list from argument"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while ( 0 )

#define GET_LIST_OF_DOUBLE_ARG(optarg, num, param, num_param, name, idx) \
    do { \
        if ( opt_is_param_name_set(param, num_param, name) ) \
        { \
            opt_get_param_name_idx(param, num_param, name, &idx); \
            if ( opt_get_arg_as_list_of_double( \
                param[idx], \
                OPT_DEFAULT_LIST_SEPCHR, \
                optarg, &num, \
                OPT_DEFAULT_LIST_SIZE) ) \
            { \
                fprintf(stderr, "unable to extract list from argument"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while ( 0 )

#ifdef __cplusplus
    }
#endif

#endif

/* endof opt_util.h */
