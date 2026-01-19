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
static int is_dot_or_dotdot(const char *name)
{
    return (strcmp(name, ".") == 0) || (strcmp(name, "..") == 0);
}

static int join_path(char *out, size_t out_size, const char *base, const char *name)
{
    size_t base_len = strlen(base);
    int needs_slash = 1;

    if (base_len > 0 && base[base_len - 1] == '/') {
        needs_slash = 0;
    }

    if (needs_slash) {
        return snprintf(out, out_size, "%s/%s", base, name) < (int)out_size;
    }

    return snprintf(out, out_size, "%s%s", base, name) < (int)out_size;
}

static unsigned long long scan_directory_postorder(const char *dirpath)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    unsigned long long total = 0ULL;

    dir = opendir(dirpath);
    if (dir == NULL) {
        fprintf(stderr, "filescanner: cannot open directory '%s': %s\n",
                dirpath, strerror(errno));
        /* If we cannot open the directory, treat it as size 0 and do not print. */
        return 0ULL;
    }

    errno = 0;
    while ((entry = readdir(dir)) != NULL) {
        char child_path[PATH_BUF_SIZE];
        struct stat st;

        if (is_dot_or_dotdot(entry->d_name)) {
            continue;
        }

        if (!join_path(child_path, sizeof(child_path), dirpath, entry->d_name)) {
            fprintf(stderr, "filescanner: path too long: '%s' + '%s'\n",
                    dirpath, entry->d_name);
            continue;
        }

        if (lstat(child_path, &st) != 0) {
            fprintf(stderr, "filescanner: lstat failed for '%s': %s\n",
                    child_path, strerror(errno));
            continue;
        }

        /* Ignore symlinks and non-regular/non-directory file types */
        if (S_ISLNK(st.st_mode)) {
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            total += (unsigned long long)st.st_size;
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            unsigned long long sub_total = scan_directory_postorder(child_path);
            total += sub_total;
            continue;
        }
    }

    if (errno != 0) {
        fprintf(stderr, "filescanner: readdir error in '%s': %s\n",
                dirpath, strerror(errno));
    }

    (void)closedir(dir);

    /* Post-order printing: print after all children have been scanned */
    printf("%s: %llu\n", dirpath, total);

    return total;
}
