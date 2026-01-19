# filescanner

filescanner is a small Unix-style utility that computes the total size (in bytes) of all regular files in a given directory hierarchy. It recursively scans subdirectories, ignores non-regular files (including symbolic links), and prints the total for each directory using post-order traversal so that subdirectories are printed before their parent directories.

This tool is built to demonstrate directory traversal using `opendir()`, `readdir()`, and recursion, and file metadata inspection using `lstat()`.
