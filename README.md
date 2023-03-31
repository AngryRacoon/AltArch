# AltArch
ALAR Archiver is a simple file archiving utility written in C. It allows you to create and extract ALAR archives (.alar files) which store compressed versions of your files and directories.

## Features
+ Create ALAR archive from a given directory.
+ Extract files from an existing ALAR archive.
## Usage
### Compilation
To compile the ALAR Archiver, run the following command:
`gcc -o alar_archiver main.c -Wall`

This command will create an executable named alar_archiver.

### Create an ALAR archive
To create an ALAR archive from a directory, run the following command:


`./alar_archiver /path/to/directory`

This command will create an archive named archive.alar in the current directory, containing the contents of the specified directory.

### Extract an ALAR archive
To extract files from an existing ALAR archive, run the following command:
./alar_archiver /path/to/archive.alar
This command will extract the contents of the specified ALAR archive into the current directory.

## Note
This utility is a simple example and not intended for use with large directories or complex file structures. It does not support advanced features like file compression or error handling, and may not work correctly with certain file types or permissions. Use at your own risk.

## License
This project is released under the MIT License. You are free to use, modify, and distribute the code, as long as you include the original copyright and permission notice in any copies or substantial portions of the software.

For more information, please refer to the [MIT](https://opensource.org/licenses/MIT) License terms and conditions.

## Contributing
If you want to contribute to this project, you can:

Fork the repository on GitHub.
Make your changes or improvements to the code.
Create a pull request and submit it for review.
Please ensure that your changes are consistent with the existing code style and do not introduce any regressions or new bugs.

## Support
If you encounter any issues or have questions, please open an issue on GitHub or contact the project maintainer.

## Acknowledgments
This project is inspired by the TAR file format and its associated utilities. The code is designed to provide a simple example of a custom file archiving tool, and is not intended for use in production environments.
