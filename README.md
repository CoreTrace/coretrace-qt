# CoreTrace Qt IDE

A modern Qt-based IDE for the CoreTrace project, providing a user-friendly interface for managing and analyzing traces.

## Documentation

Complete documentation for this project is available online:
[https://coretrace.github.io/coretrace-qt/](https://coretrace.github.io/coretrace-qt/)

The documentation includes:
- API Reference: Technical documentation of the codebase
- User Guide: Instructions for using the application

## Features

- Modern Qt-based GUI
- File tree view with context menu support
- Project management
- Trace analysis tools
- Cross-platform support

## Prerequisites

- Qt 6.x
- CMake 3.16 or higher
- C++17 compatible compiler
- Git

### Required Libraries

- Qt6 Core
- Qt6 Widgets
- Qt6 Gui
- libcurl
- libssh2

## Building the Project

### Linux/macOS

1. Clone the repository:
```bash
git clone https://github.com/yourusername/coretrace-qt.git
cd coretrace-qt
```

2. Make the build script executable:
```bash
chmod +x build.sh
```

3. Run the build script:
```bash
./build.sh
```

The build script will:
- Check for required dependencies
- Create a build directory
- Configure the project with CMake
- Build the project
- Install the executable (if specified)

### Windows

For Windows builds, we recommend using MXE (MinGW-w64 Cross Compiler) to create a standalone executable.

1. Install MXE following their [installation guide](https://mxe.cc/#tutorial)

2. Use the provided Makefile:
```bash
make -f Makefile.windows
```

This will create a standalone .exe file with all required dependencies.

## Project Structure

```
coretrace-qt/
├── src/                    # Source files
├── includes/              # Header files
├── resources/             # Resources (icons, etc.)
├── build/                 # Build directory (created by build.sh)
├── CMakeLists.txt        # CMake configuration
├── Makefile              # Windows build configuration USE WITH MXE !
├── build.sh              # Build script
└── README.md             # This file
```

## Development

### Adding New Features

1. Create new source files in the `src/` directory
2. Add corresponding headers in the `includes/` directory
3. Update CMakeLists.txt with new files
4. Rebuild the project

### Code Style

- Follow Qt coding conventions
- Use C++17 features
- Document all public interfaces
- Write unit tests for new features

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## Contact

Mail. cedric.roulof@epitech.eu

Discord. : shookapic (faster response)