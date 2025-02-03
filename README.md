# AntiVirus Project

## Overview
The **AntiVirus Project** is a simple command-line antivirus scanner designed to detect files infected with known virus signatures. It scans directories for potentially harmful files by comparing their content against predefined virus signatures.

## Features
- **Signature-based scanning**: Compares file content with predefined virus signatures.
- **Normal and Quick Scan modes**: Choose between a full scan or a faster, limited scan.
- **Logging system**: Records scan results in `AntiVirusLog.txt`.
- **Directory scanning**: Recursively scans all files in a specified folder.

## Installation & Compilation
### Prerequisites
- A C compiler (e.g., GCC, Clang, or MSVC)
- Windows operating system (uses `dirent.h` for directory handling)

### Compilation
To compile the program, use the following command:
```sh
gcc -o antivirus prog.c
```

## Usage
### Running the Scanner
To scan a directory, run the following command:
```sh
./antivirus <directory_path> <signature_file>
```
- `<directory_path>`: The path of the directory to scan.
- `<signature_file>`: A file containing the virus signature to detect.

### Scan Modes
- **Normal Scan**: Scans the entire file.
- **Quick Scan**: Analyzes only specific portions of the file for faster results.

### Example Output
```
Anti-virus began! Welcome!

Folder to scan: files
Virus signature: KittenVirusSign
Scanning option: Normal Scan 
Results:
files//kitten_frog.jpg Infected!
files//kitten_run.jpg Clean
```

## Code Structure
### Main Components
- **prog.c** – Main antivirus logic, handles file scanning and logging.
- **dirent.h** – Directory handling for Windows environments.
- **AntiVirusLog.txt** – Stores scan results.
- **KittenVirusSign** – Example virus signature file.

## Future Improvements
- Implement heuristic scanning for unknown virus patterns.
- Add real-time file monitoring.
- Improve scan performance using multithreading.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
