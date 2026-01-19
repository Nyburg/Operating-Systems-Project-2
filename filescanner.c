/*
 * filescanner.c
 *
 * Recursively totals the size (in bytes) of regular files in a directory
 * hierarchy. Directories are printed post-order (children before parents).
 *
 * Other file types are ignored, including symbolic links.
 */