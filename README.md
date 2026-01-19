# filescanner

filescanner is a small Unix-style utility that computes the total size (in bytes) of all regular files in a given directory hierarchy. It recursively scans subdirectories, ignores non-regular files (including symbolic links), and prints the total for each directory using post-order traversal so that subdirectories are printed before their parent directories.

This tool is built to demonstrate directory traversal using `opendir()`, `readdir()`, and recursion, and file metadata inspection using `lstat()`.

## Function call diagram

main
  -> parse_directory_arg
  -> scan_directory_postorder(dirpath)
       -> opendir
       -> readdir (loop)
            -> join_path
            -> lstat
            -> scan_directory_postorder(child_dir)   (recursive, for subdirectories)
       -> closedir
       -> printf (post-order output)