import os
import argparse
from textwrap import dedent


SOURCE_DIRS = ['src']
BUILD_DIR = 'build'


class NinjaGen():
    def __init__(self, source_dirs, build_dir, toolchain):
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
        for source_dir in self.source_dirs:
            for root, _, files in os.walk(source_dir):
                for file in files:
                    if file.endswith('.c'):
                        self.c_files.append(os.path.join(root, file))
                    elif file.endswith('.S'):
                        self.s_files.append(os.path.join(root, file))

    def _add_targets(self):
        # add build rules for .c and .S files
        for file in self.c_files + self.s_files:
            obj_file = os.path.join(self.build_dir, os.path.splitext(file)[0] + '.o')
            self.build_ninja_content += f"build {obj_file}: cc {file}\n"

    def _build_target(self):
        # add build rule for the final executable
        obj_files = ' '.join([os.path.join(self.build_dir, os.path.splitext(file)[0] + '.o') for file in self.c_files + self.s_files])
        self.build_ninja_content += f"build kernel8.elf: ld {obj_files}\n"
        self.build_ninja_content += f"build kernel8.img: objcopy kernel8.elf\n"
        self.build_ninja_content += f"build compile_commands.json: generate_compile_commands\n"
        self.build_ninja_content += f"default kernel8.img compile_commands.json\n"

    def _write_to_ninja_file(self, filename: str):
        with open(filename, 'w') as f:
            f.write(self.build_ninja_content)

    def build(self):
        self._find_files()
        self._add_rules()
        self._add_targets()
        self._build_target()
        self._write_to_ninja_file("build.ninja")

def main():
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
