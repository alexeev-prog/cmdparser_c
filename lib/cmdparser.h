/**
 * @file cmdparser.h
 * @author Alexeev Bronislav
 * @brief Command line arguments parser
 * @version 0.1
 * @date 2025-06-26
 *
 * @copyright Copyright Alexeev Bronislav (c) 2025

 ██████ ███    ███ ██████      ██████   █████  ██████  ███████ ███████ ██████       ██████
██      ████  ████ ██   ██     ██   ██ ██   ██ ██   ██ ██      ██      ██   ██     ██
██      ██ ████ ██ ██   ██     ██████  ███████ ██████  ███████ █████   ██████      ██
██      ██  ██  ██ ██   ██     ██      ██   ██ ██   ██      ██ ██      ██   ██     ██
 ██████ ██      ██ ██████      ██      ██   ██ ██   ██ ███████ ███████ ██   ██      ██████

 **/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Command Option struct
 *
 **/
struct CommandOption {
    const char* help;
    const char* long_name;
    char short_name;
    int has_arg;
    const char* default_value;
    void* handler;
};

/**
 * @brief CLI Program Metadata
 *
 **/
struct CLIMetadata {
    const char *prog_name;
    const char *description;
    const char *usage_args;
    struct CommandOption *options;
    size_t options_count;
};

typedef struct CommandOption CMDOption[];
typedef struct CLIMetadata ProgramInfo[];

/**
 * @brief
 *
 * @param meta
 **/
void print_help(struct CLIMetadata* meta) {
    printf("%s\n", meta->description);

    printf("Usage: %s [OPTIONS] %s\n\n", meta->prog_name, meta->usage_args ? meta->usage_args : "");

    printf("Options:\n");

    for (size_t i = 0; i < meta->options_count; i++) {
        struct CommandOption* opt = &meta->options[i];

        char left_col[128] = {0};
        char short_buf[4] = {0};

        if (opt->short_name) {
            snprintf(short_buf, sizeof(short_buf), "-%c", opt->short_name);
        }

        if (opt->long_name) {
            if (opt->short_name) {
                snprintf(left_col, sizeof(left_col), "%s, --%s", short_buf, opt->long_name);
            } else {
                snprintf(left_col, sizeof(left_col), "    --%s", opt->long_name);
            }
        } else if (opt->short_name) {
            snprintf(left_col, sizeof(left_col), "%s", short_buf);
        }

        if (opt->has_arg) {
            if (opt->long_name) {
                strncat(left_col, "=ARG", sizeof(left_col) - strlen(left_col) - 1);
            } else {
                strncat(left_col, " ARG", sizeof(left_col) - strlen(left_col) - 1);
            }
        }

        if (opt->default_value == NULL) {
            printf("  %-30s %s\n", left_col, opt->help ? opt->help : "");
        } else {
            printf("  %-30s %s (default: %s)\n", left_col, opt->help ? opt->help : "", opt->default_value);
        }
    }

    printf("\n");
}

/**
 * @brief Find option
 *
 * @param options Options list
 * @param options_count Options count
 * @param short_name Option short name
 * @param long_name Option long name
 * @return const CommandOption*
 **/
struct CommandOption* find_option(struct CommandOption* options,
                                  size_t options_count,
                                  char short_name,
                                  const char* long_name) {
    for (size_t i = 0; i < options_count; ++i) {
        if (short_name && options[i].short_name == short_name) {
            return &options[i];
        }

        if (long_name && options[i].long_name && strcmp(options[i].long_name, long_name) == 0) {
            return &options[i];
        }
    }
    return NULL;
}

/**
 * @brief Parse options
 *
 * @param argc arguments count
 * @param argv arguments array
 * @param options CommandOption array
 * @param options_count count of options
 * @return int
 **/
int parse_options(int argc, char** argv, struct CommandOption* options, size_t options_count) {
    int i = 1;

    while (i < argc) {
        const char* arg = argv[i];

        if (strcmp(arg, "--") == 0) {
            i++;
            break;
        }

        if (strncmp(arg, "--", 2) == 0) {
            const char* name = arg + 2;
            const char* value = strchr(name, '=');
            size_t name_len = value ? (size_t)(value - name) : strlen(name);

            char long_name_buf[64];
            if (name_len >= sizeof(long_name_buf)) {
                fprintf(stderr, "Option too long: %s\n", name);
                return -1;
            }
            strncpy(long_name_buf, name, name_len);
            long_name_buf[name_len] = '\0';

            struct CommandOption* opt = find_option(options, options_count, '\0', long_name_buf);
            if (!opt) {
                fprintf(stderr, "Unknown option: --%s\n", long_name_buf);
                return -1;
            }

            if (opt->has_arg) {
                if (value) {
                    *(const char**)opt->handler = value + 1;
                } else {
                    if (opt->default_value != NULL) {
                        *(const char**)opt->handler = opt->default_value + 1;
                    } else {
                        if (i + 1 >= argc) {
                            fprintf(stderr, "Missing value for: --%s\n", long_name_buf);
                            return -1;
                        }
                        *(const char**)opt->handler = argv[++i];
                    }
                }
            } else {
                if (value) {
                    fprintf(stderr, "Unexpected value for: --%s\n", long_name_buf);
                    return -1;
                }
                *(int*)opt->handler = 1;
            }
            i++;
            continue;
        }

        if (arg[0] == '-' && arg[1] != '\0') {
            const char* chars = arg + 1;
            while (*chars) {
                char c = *chars++;
                struct CommandOption* opt = find_option(options, options_count, c, NULL);

                if (!opt) {
                    fprintf(stderr, "Unknown option: -%c\n", c);
                    return -1;
                }

                if (opt->has_arg) {
                    if (*chars != '\0') {
                        *(const char**)opt->handler = chars;
                        break;
                    } else {
                        if (opt->default_value != NULL) {
                            *(const char**)opt->handler = opt->default_value;
                            break;
                        } else {
                            if (i + 1 >= argc) {
                                fprintf(stderr, "Missing value for: -%c\n", c);
                                return -1;
                            }
                            *(const char**)opt->handler = argv[++i];
                        }
                    }
                } else {
                    *(int*)opt->handler = 1;
                }
            }
            i++;
            continue;
        }

        break;
    }

    return i;
}
