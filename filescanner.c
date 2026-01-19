/*
 * filescanner.c
 *
 * Recursively totals the size (in bytes) of regular files in a directory
 * hierarchy. Directories are printed post-order (children before parents).
 *
 * Other file types are ignored, including symbolic links.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#define EXIT_USAGE 1
#define EXIT_OK 0

#define USAGE_MSG "usage: filescanner [directory]\n"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define PATH_BUF_SIZE PATH_MAX

static void print_usage_and_exit(void);
static const char *parse_directory_arg(int argc, char **argv);
static int is_dot_or_dotdot(const char *name);
static int join_path(char *out, size_t out_size, const char *base, const char *name);
static unsigned long long scan_directory_postorder(const char *dirpath);

int main(int argc, char **argv)
{
    const char *dirpath = parse_directory_arg(argc, argv);

    (void)scan_directory_postorder(dirpath);

    return EXIT_OK;
}

static void print_usage_and_exit(void)
{
    fprintf(stderr, "%s", USAGE_MSG);
    exit(EXIT_USAGE);
}

static const char *parse_directory_arg(int argc, char **argv)
{
    if (argc == 1) {
        return ".";
    }

    if (argc == 2) {
        return argv[1];
    }

    print_usage_and_exit();
    return ".";
}