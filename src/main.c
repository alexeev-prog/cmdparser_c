#include "../lib/cmdparser.h"
#include <stdio.h>

int main(int argc, char **argv) {
    int help_flag = 0;
    int verbose_flag = 0;
    const char *output_file = NULL;
    const char *input_file = NULL;

    struct CommandOption options[4] = {
        {"Help info", "help",    'h', 0, NULL, &help_flag},     // Help flag
        {"Verbose flag", "verbose", 'v', 0, NULL, &verbose_flag},  // Verbose flag
        {"Output file", "output",  'o', 1, "test.c", &output_file},   // Option with argument
        {"Option sort", NULL,      'i', 1, NULL, &input_file}     // Option only with short name
    };

    struct CLIMetadata meta = {
        .prog_name = argv[0],
        .description = "File Processor - processes input files and generates output",
        .usage_args = "[FILE...]",
        .options = options,
        .options_count = sizeof(options) / sizeof(options[0])
    };

    int pos_index = parse_options(argc, argv, meta.options, meta.options_count);

    if (pos_index < 0) {
        return 1;
    }

    // Help flag
    if (help_flag) {
        print_help(&meta);
        return 0;
    }

    // Parsing result
    printf("Verbose mode: %s\n", verbose_flag ? "ON" : "OFF");

    if (output_file) {
        printf("Output file: %s\n", output_file);
    }

    // Print positional arguments
    printf("Positional arguments:\n");
    for (int i = pos_index; i < argc; i++) {
        printf("  %d: %s\n", i - pos_index + 1, argv[i]);
    }

    return 0;
}
