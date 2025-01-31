import os
import argparse
from textwrap import dedent


SOURCE_DIRS = ['src']
BUILD_DIR = 'build'


class NinjaGen():
    """
    A class to generate a Ninja build file for a given set of source directories, build directory, and toolchain.

    Attributes:
        source_dirs (list): List of directories containing source files.
        build_dir (str): Directory where build artifacts will be stored.
        toolchain (str): Prefix for the toolchain binaries (e.g., 'aarch64-linux-gnu-').
        c_files (list): List of C source files found in source_dirs.
        s_files (list): List of assembly source files found in source_dirs.
        build_ninja_content (str): Content of the generated Ninja build file.
        cc (str): Command for the C compiler.
        ld (str): Command for the linker.
        objcopy (str): Command for the objcopy tool.
        cflags (str): Flags for the C compiler.
        ldflags (str): Flags for the linker.

    Methods:
        _add_rules(): Adds the build rules to the Ninja build file content.
        _find_files(): Finds all .c and .S files in the source directories.
        _add_targets(): Adds build targets for the found source files.
        _build_target(): Adds the final build target for the executable and other outputs.
        _write_to_ninja_file(filename: str): Writes the Ninja build file content to a file.
        build(): Generates the Ninja build file by finding files, adding rules, targets, and writing to a file.
    """
    def __init__(self, source_dirs, build_dir, toolchain):
        """
        Initialize the configuration for the build system.

        Args:
            source_dirs (list): List of directories containing source files.
            build_dir (str): Directory where the build output will be stored.
            toolchain (str): Prefix for the toolchain binaries (e.g., 'aarch64-linux-gnu-').

        Attributes:
            source_dirs (list): List of directories containing source files.
            build_dir (str): Directory where the build output will be stored.
            c_files (list): List of C source files.
            s_files (list): List of assembly source files.
            build_ninja_content (str): Content for the Ninja build file.
            toolchain (str): Prefix for the toolchain binaries.
            cc (str): Command for the C compiler.
            ld (str): Command for the linker.
            objcopy (str): Command for the objcopy tool.
            cflags (str): Flags for the C compiler.
            ldflags (str): Flags for the linker.
        """
        self.source_dirs = source_dirs
        self.build_dir = build_dir
        self.c_files = []
        self.s_files = []
        self.build_ninja_content = ""
        self.toolchain = toolchain
        self.cc = f'{self.toolchain}gcc'
        self.ld = f'{self.toolchain}ld'
        self.objcopy = f'{self.toolchain}objcopy'
        self.cflags = '-ffreestanding -Wall -Wextra -Isrc -g -DAARCH64'
        self.ldflags = '-T linker8.ld -g'

    def _add_rules(self):
        """
        Adds build rules to the Ninja build system configuration.

        This method defines several build rules using the Ninja build system syntax:
        - `cc`: Compiles source files into object files using the specified compiler and flags.
        - `ld`: Links object files into an executable using the specified linker and flags.
        - `objcopy`: Converts object files into binary format using the specified objcopy tool.
        - `generate_compile_commands`: Generates a compile_commands.json file for use with tools that support the 
          Compilation Database format.

        The rules are formatted and stored in the `build_ninja_content` attribute.
        """
        self.build_ninja_content = dedent(f"""
        rule cc
          command = {self.cc} {self.cflags} -c $in -o $out
          description = CC $out

        rule ld
          command = {self.ld} {self.ldflags} -o $out $in
          description = LD $out

        rule objcopy
          command = {self.objcopy} $in -O binary $out
          description = OBJCOPY $out

        rule generate_compile_commands
          command = ninja -t compdb > compile_commands.json
          description = Generating compile_commands.json
        """)

    def _find_files(self):
        """
        Recursively searches for .c and .S files in the specified source directories.

        This method walks through each directory in self.source_dirs and appends the 
        paths of files ending with .c to self.c_files and files ending with .S to 
        self.s_files.

        Attributes:
            self.source_dirs (list): List of directories to search for source files.
            self.c_files (list): List to store paths of found .c files.
            self.s_files (list): List to store paths of found .S files.
        """
        for source_dir in self.source_dirs:
            for root, _, files in os.walk(source_dir):
                for file in files:
                    if file.endswith('.c'):
                        self.c_files.append(os.path.join(root, file))
                    elif file.endswith('.S'):
                        self.s_files.append(os.path.join(root, file))

    def _add_targets(self):
        """
        Adds build rules for .c and .S files to the Ninja build script.

        This method iterates over the list of C source files (`self.c_files`) and
        assembly source files (`self.s_files`). For each file, it generates the
        corresponding object file path and appends a build rule to the Ninja build
        script content (`self.build_ninja_content`).

        The build rule specifies that the object file is built from the source file
        using the `cc` command.

        Example build rule:
            build /path/to/build/dir/source.o: cc /path/to/source.c
        """
        # add build rules for .c and .S files
        for file in self.c_files + self.s_files:
            obj_file = os.path.join(self.build_dir, os.path.splitext(file)[0] + '.o')
            self.build_ninja_content += f"build {obj_file}: cc {file}\n"

    def _build_target(self):
        """
        Build the target rules for the final executable and related files.

        This method constructs the build rules for the final executable (kernel8.elf),
        the image file (kernel8.img), and the compile commands file (compile_commands.json).
        It appends these rules to the build_ninja_content attribute.

        The method performs the following steps:
        1. Joins the object files from the C and assembly source files.
        2. Adds a build rule for linking the object files into the final executable (kernel8.elf).
        3. Adds a build rule for creating the image file (kernel8.img) from the executable.
        4. Adds a build rule for generating the compile commands file (compile_commands.json).
        5. Sets the default build targets to be the image file and the compile commands file.

        Returns:
            None
        """
        # add build rule for the final executable
        obj_files = ' '.join([os.path.join(self.build_dir, os.path.splitext(file)[0] + '.o') for file in self.c_files + self.s_files])
        self.build_ninja_content += f"build kernel8.elf: ld {obj_files}\n"
        self.build_ninja_content += f"build kernel8.img: objcopy kernel8.elf\n"
        self.build_ninja_content += f"build compile_commands.json: generate_compile_commands\n"
        self.build_ninja_content += f"default kernel8.img compile_commands.json\n"

    def _write_to_ninja_file(self, filename: str):
        """
        Writes the build ninja content to a specified file.

        Args:
            filename (str): The name of the file to write the build ninja content to.
        """
        with open(filename, 'w') as f:
            f.write(self.build_ninja_content)

    def build(self):
        """
        Executes the build process by performing the following steps:
        
        1. Finds the necessary files.
        2. Adds build rules.
        3. Adds build targets.
        4. Builds the target.
        5. Writes the build configuration to a Ninja build file named "build.ninja".
        """
        self._find_files()
        self._add_rules()
        self._add_targets()
        self._build_target()
        self._write_to_ninja_file("build.ninja")

def main():
    """
    Main function to parse command-line arguments and generate a Ninja build file.

    Command-line arguments:
    --source-dirs: List of source directories (default: SOURCE_DIRS)
    --build-dir: Build directory (default: BUILD_DIR)
    --toolchain: Toolchain (default: 'aarch64-none-elf-')

    This function initializes the argument parser, parses the provided arguments,
    and uses them to create and build a NinjaGen object.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('--source-dirs', nargs='+', default=SOURCE_DIRS, help='List of source directories')
    parser.add_argument('--build-dir', default=BUILD_DIR, help='Build directory')
    parser.add_argument('--toolchain', default='aarch64-none-elf-', help='Toolchain')
    args = parser.parse_args()

    # generate ninja build file
    ng = NinjaGen(args.source_dirs, args.build_dir, args.toolchain)
    ng.build()

if __name__ == '__main__':
    main()
