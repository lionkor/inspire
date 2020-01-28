#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>

#define ENDL "\n"
#define STR_BAD_ARGC_ADD "expected one or more arguments to `add`. run `inspire help` to show help." ENDL
#define STR_UNKNOWN_ERROR "an error occured." ENDL
#define STR_NO_ENV_HOME "the environment variable 'HOME' was not found. please supply it if it's not defined." ENDL
#define STR_FOPEN_ERROR "an error occured and is displayed below. it is likely that this happened because no ideas have been added yet. run 'inspire help' to find out how to add some!" ENDL

#define STR_HELP                                                                                    \
    "Usage:\n"                                                                                      \
    "  'inspire add [text]' - adds the text to the list of ideas.\n"                                \
    "                         will read stdin instead if no text is given.\n"                       \
    "  'inspire remove' - starts a prompt to remove specific ideas from the list\n"                 \
    "  'inspire give' - gives a random idea from the list\n"                                        \
    "  'inspire help' - displays this help\n"                                                       \
    "  'inspire show' - shows all ideas currently stored\n"                                         \
    "\nExamples :\n"                                                                                \
    "  '$ inspire add Finish my game' - adds the idea 'Finish my game' to the list of ideas\n"      \
    "  '$ inspire give' - gives \"Finish my game\" or another random idea from the list of ideas\n" \
    "\nLicensed under GPL-2.0\nReport bugs to development@kortlepel.com\n"
#define STR_HELP_SHORT \
    "Unknown usage. Run 'inspire help' to show help.\n"

typedef struct vector {
    void*  data;
    size_t size;
    size_t capacity;
    size_t elem_size;
} vector_t;

vector_t* vector_create(size_t elem_size, size_t reserve) {
    vector_t* v  = (vector_t*)malloc(sizeof(vector_t));
    v->data      = calloc(reserve, elem_size);
    v->capacity  = reserve;
    v->size      = 0;
    v->elem_size = elem_size;
    return v;
}

void vector_free(vector_t* v) {
    if (v)
        free(v->data);
    free(v);
}

void vector_push_back(vector_t* v, void* elem) {
    assert(v); // we just want to crash, since this is not a public api
    assert(elem);
    if (v->size + 1 > v->capacity) {
        v->data = realloc(v->data, v->elem_size * (v->size + 1));
        v->capacity++;
    }
    memcpy(v->data + v->size * v->elem_size, elem, v->elem_size);
    v->size++;
}

void* vector_at(vector_t* v, size_t index) {
    return v->data + index * v->elem_size;
}

FILE* open_inspire_data_file(char* openmode) {
    char* home_dir = getenv("HOME");
    if (!home_dir) {
        fprintf(stderr, STR_NO_ENV_HOME);
        return NULL;
    }
    char dir[1024];
    memset(dir, 0, 1024);
    strcpy(dir, home_dir);
    strcat(dir, "/.inspire");

    int rc = mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (rc != 0 && errno != EEXIST) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("mkdir");
        return NULL;
    }

    strcat(dir, "/data");

    // we assume that the file exists or can be created at this point
    FILE* fp = fopen(dir, openmode);
    if (!fp) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("fopen");
        return NULL;
    }

    return fp;
}

int command_add(int argc, char** argv) {
    // argv[0] -> program name
    // argv[1] -> command "add"
    // argv[2] => idea string(s) to add

    FILE* fp = open_inspire_data_file("a");
    if (!fp)
        return -1; // open_inspire_data_file already prints errors

    /* 
     * write all the args from including the third one.
     * we expect those to be the parts of the string to add.
     * its possible to instead enforce entering the string in quotes,
     * but that seems unnecessary since we won't have other args anyways. 
     */
    if (argc > 2) {
        for (int i = 2; i < argc; ++i) {
            fprintf(fp, "%s ", argv[i]);
        }
        fprintf(fp, ENDL);
    } else {
        // no extra args, we ask for input (thanks to Inferno_geek for this idea)
        size_t maxlen;
        char*  idea = NULL;
        int    read = getline(&idea, &maxlen, stdin);
        if (!idea) {
            fprintf(stderr, STR_UNKNOWN_ERROR);
            perror("getline");
            return -1;
        }
        if (strlen(idea) == 0 || read == 0 || (strlen(idea) == 1 && idea[0] == '\n')) {
            fprintf(stderr, "empty idea, ignoring\n");
            return -1;
        }
        fprintf(fp, "%s", idea);
    }

    int rc = fclose(fp);
    if (rc != 0) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("fclose");
        return -1;
    }

    return 0;
}

int command_give() {

    FILE* fp = open_inspire_data_file("r");
    if (!fp)
        return -1; // open_inspire_data_file already prints errors

    // 1 KB max line length
    size_t max_len = 1024;
    char   line[max_len];
    // reserve space for 10 lines
    // after 10 lines reallocs will happen with vector_push_back
    vector_t* lines = vector_create(sizeof(char) * max_len, 10);
    while (fgets(line, max_len, fp) != NULL) {
        // copy max_len chars from line into the vector
        vector_push_back(lines, line);
    }

    if (lines->size == 0) {
        printf("no ideas stored!\n");
    } else {
        // no need for newline since the lines end with \n anyways
        printf("%s", vector_at(lines, rand() % lines->size));
    }

    vector_free(lines);
    int rc = fclose(fp);
    if (rc != 0) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("fclose");
        return -1;
    }

    return 0;
}

int command_help() {
    printf("%s", STR_HELP);
    return 0;
}

int command_show() {
    FILE* fp = open_inspire_data_file("r");
    if (!fp)
        return -1; // open_inspire_data_file already prints errors

    // 1 KB max line length
    size_t max_len = 1024;
    char   line[max_len];
    while (fgets(line, max_len, fp) != NULL) {
        printf("%s", line);
    }

    int rc = fclose(fp);
    if (rc != 0) {
        fprintf(stderr, STR_UNKNOWN_ERROR);
        perror("fclose");
        return -1;
    }
    return 0;
}

int command_remove() {
    // interactive command, lots of stdin/out
    FILE*  fp      = open_inspire_data_file("rw");
    size_t max_len = 1024;
    char   line[max_len];

    vector_t* lines = vector_create(sizeof(char) * max_len, 10);
    while (fgets(line, max_len, fp) != NULL) {
        // copy max_len chars from line into the vector
        vector_push_back(lines, line);
    }

    if (lines->size == 0) {
        printf("no ideas stored!\n");
        return -1;
    }

    // reopen the file, change mode to w (truncate)

    while (true) {

        printf("ideas:\n");
        printf(" ID  idea\n");
        for (size_t i = 0; i < lines->size; ++i) {
            if (strlen(vector_at(lines, i)) == 0)
                continue;
            printf("%3lu: %s", i, vector_at(lines, i));
        }
        printf("remove idea (ID) or \"-1\" to write changes: ");
        int index;
        int res = scanf("%d", &index);
        if (res == EOF || res != 1) {
            printf("Invalid input, exiting.\n");
            scanf("%*[^\n]");
            break;
        }

        if (index < 0)
            break;
        if (index >= (long)lines->size) {
            printf("ID out of range\n");
            scanf("%*[^\n]");
            continue;
        }
        memset(vector_at(lines, index), 0, lines->elem_size);
    }

    freopen(NULL, "w", fp);
    for (size_t i = 0; i < lines->size; ++i) {
        if (strlen(vector_at(lines, i)) == 0)
            continue;
        fprintf(fp, "%s", vector_at(lines, i));
    }

    vector_free(lines);
    fclose(fp);
    return 0;
}

int main(int argc, char** argv) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // using nanoseconds to seed in case we call give more than once a second (likely)
    srand((time_t)ts.tv_nsec);

    if (argc > 1) {
        if (strcmp(argv[1], "add") == 0) {
            return command_add(argc, argv);
        } else if (strcmp(argv[1], "give") == 0) {
            return command_give();
        } else if (strcmp(argv[1], "help") == 0) {
            return command_help();
        } else if (strcmp(argv[1], "show") == 0) {
            return command_show();
        } else if (strcmp(argv[1], "remove") == 0) {
            return command_remove();
        }
    }

    printf("%s", STR_HELP_SHORT);
    return -1;
}

