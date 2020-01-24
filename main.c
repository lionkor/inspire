#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>

typedef enum Result
{
    OK           = 0x000, // everything is okay
    NO_ARGS      = 0x001, // no arguments given
    UNKNOWN_ARGS = 0x002, // given argument(s) not known
    BAD_ARGC     = 0x003, // argument count invalid
    MKDIR_ERROR  = 0x004, // mkdir failed
    FOPEN_ERROR  = 0x005, // fopen failed
    FCLOSE_ERROR = 0x006, // fclose failed (yes that's a thing)
} Result;

#define ENDL "\n"
#define STR_BAD_ARGC_ADD "expected one or more arguments to `add`. run `inspire help` to show help." ENDL
#define STR_UNKNOWN_ERROR "an error occured." ENDL
#define STR_HELP                                                                               \
    "Usage:\n"                                                                                 \
    "  'inspire add <text>' - adds the text to the list of ideas\n"                            \
    "  'inspire give' - gives a random idea from the list\n"                                   \
    "  'inspire help' - displays this help\n"                                                  \
    "\nExamples:\n"                                                                            \
    "  'inspire add Finish my game' - adds the idea \"Finish my game\" to the list of ideas\n" \
    "  'inspire give' - prints a random idea from the list, for example \"Finish my game\"\n"  \
    "\nLicensed under GPL-2.0\nReport bugs to development@kortlepel.com\n"
#define STR_HELP_SHORT \
    "Unknown usage. Run 'inspire help' to show help.\n"


Result
command_add(int argc, char** argv) {
    // argv[0] -> program name
    // argv[1] -> command "add"
    // argv[2] => idea string(s) to add
    if (argc == 2) {
        // not enough args
        fprintf(stderr, STR_BAD_ARGC_ADD);
        return BAD_ARGC;
    }

    char* home_dir = getenv("HOME");
    char  dir[1024];
    memset(dir, 0, 1024);
    strcpy(dir, home_dir);
    strcat(dir, "/.inspire");

    int rc = mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (rc != 0 && errno != EEXIST) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("mkdir");
        return MKDIR_ERROR;
    }

    strcat(dir, "/data");

    // open the file in append mode
    // (we assume that it exists or can be created at this point)
    FILE* fp = fopen(dir, "a");
    if (!fp) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("fopen");
        return FOPEN_ERROR;
    }

    /* 
     * write all the args from including the third one.
     * we expect those to be the parts of the string to add.
     * its possible to instead enforce entering the string in quotes,
     * but that seems unnecessary since we won't have other args anyways. 
     */
    for (int i = 2; i < argc; ++i) {
        fprintf(fp, "%s ", argv[i]);
    }
    fprintf(fp, ENDL);

    rc = fclose(fp);
    if (rc != 0) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("fclose");
        return FCLOSE_ERROR;
    }

    return OK;
}

Result command_give() {

    return OK;
}

Result command_help() {
    printf("%s", STR_HELP);
    return OK;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "add") == 0) {
            return command_add(argc, argv);
        } else if (strcmp(argv[1], "give") == 0) {
            return command_give();
        } else if (strcmp(argv[1], "help") == 0) {
            return command_help();
        }
    }

    printf("%s", STR_HELP_SHORT);
    return UNKNOWN_ARGS;
}

