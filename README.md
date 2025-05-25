# Custom File System Project

![Screenshot_2024-05-06_at_12 22 21_AM](https://github.com/user-attachments/assets/348d5e25-39e0-4bdc-9327-5be5347e4f8c)

---

## Overview

This project implements a custom file system in C for educational purposes (CSC-415-02 Spring 2024, Group: Something Simple). It provides a shell interface (`fsshell`) to interact with a virtual disk, supporting file and directory operations similar to those in Unix/Linux systems.

## Features

- Create, read, write, and delete files
- Create and remove directories
- Change and print working directory
- List directory contents (with options for long/all formats)
- Copy/move files within the file system
- Copy files to/from the Linux file system
- View file contents (`cat`)
- Command history and help
- FAT-based free space management
- Volume Control Block (VCB) for disk metadata
- Hexdump utility for low-level disk inspection

## Build Instructions

### Prerequisites

- GCC (tested with gcc:latest)
- `make`
- `libreadline` (for shell input)
- `pthread` library

### Build with Make

```
make
```

This will build the main shell executable (`fsshell`) and all dependencies.

### Build with Docker (optional)

A `Dockerfile` is provided:

```
docker build -t custom-fs .
docker run -it custom-fs
```

## Run Instructions

To start the file system shell:

```
./fsshell SampleVolume 10000000 512
```

- `SampleVolume`: Name of the virtual disk file
- `10000000`: Volume size in bytes
- `512`: Block size in bytes

You will be presented with a shell prompt. Type commands as you would in a Unix shell.

## Shell Commands

| Command | Description                                              |
| ------- | -------------------------------------------------------- |
| ls      | List files in a directory (supports --all/-a, --long/-l) |
| cp      | Copy a file: `cp src [dest]`                             |
| mv      | Move a file: `mv src dest`                               |
| md      | Make a new directory: `md dirname`                       |
| rm      | Remove a file or directory: `rm path`                    |
| touch   | Create an empty file: `touch filename`                   |
| cat     | Display file contents: `cat filename`                    |
| cp2l    | Copy file from FS to Linux: `cp2l src [dest]`            |
| cp2fs   | Copy file from Linux to FS: `cp2fs src [dest]`           |
| cd      | Change directory: `cd path`                              |
| pwd     | Print working directory                                  |
| history | Show command history                                     |
| help    | Show help for commands                                   |
| exit    | Exit the shell and save state                            |

## Example Usage

```
md docs
cd docs
touch notes.txt
echo "Hello World" > notes.txt
ls -l
cat notes.txt
cd ..
ls
rm docs/notes.txt
```

## File/Module Descriptions

- **fsshell.c**: Main shell interface and command dispatcher
- **fileSystem.c/h**: Core file system logic, directory and file management
- **fsLow.c/h**: Low-level block device emulation and partition management
- **b_io.c/h**: Buffered file I/O operations
- **freeSpace.c/h**: FAT-based free space management
- **parsePath.c**: Path parsing and directory entry resolution
- **manageFileSize.c**: File size management and block allocation
- **fs_mkdir.c, fs_rmdir.c, fs_delete.c, fs_stat.c, fs_getcwd.c, fs_setcwd.c, fs_isDir.c, fs_isFile.c, fs_opendir.c, fs_readdir.c, fs_closedir.c**: Individual file system operations
- **b_readHelpers.c, b_writeHelpers.c**: Helper functions for buffered I/O
- **DirectoryEntry.h, FileControlBlock.h**: Data structures for files and directories
- **Hexdump/hexdump.c**: Utility to view raw disk contents in hex/ascii
- **Makefile**: Build instructions
- **Dockerfile**: Containerized build/run environment

## Authors

- Tushin Kulshreshtha (Dextron04)
- Hayden Coke (crowcode17)
- Eric Khuong (ekhuong)
- Thanh Duong (DanielDoubleDx)

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
