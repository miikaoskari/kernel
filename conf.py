import os

source_dirs = ['src']
build_dir = 'build'

cc = 'aarch64-none-elf-gcc'
ld = 'aarch64-none-elf-ld'
objcopy = 'aarch64-none-elf-objcopy'
cflags = '-ffreestanding -Wall -Wextra -Isrc -g -DAARCH64'
ldflags = '-T linker.ld -g'

c_files = []
s_files = []
for source_dir in source_dirs:
    for root, _, files in os.walk(source_dir):
        for file in files:
            if file.endswith('.c'):
                c_files.append(os.path.join(root, file))
            elif file.endswith('.S'):
                s_files.append(os.path.join(root, file))

# generate ninja build file
build_ninja_content = f"""
rule cc
  command = {cc} {cflags} -c $in -o $out
  description = CC $out

rule ld
  command = {ld} {ldflags} -o $out $in
  description = LD $out

rule objcopy
  command = {objcopy} $in -O binary $out
  description = OBJCOPY $out

rule generate_compile_commands
  command = ninja -t compdb > compile_commands.json
  description = Generating compile_commands.json
"""

# add build rules for .c and .S files
for file in c_files + s_files:
    obj_file = os.path.join(build_dir, os.path.splitext(file)[0] + '.o')
    build_ninja_content += f"build {obj_file}: cc {file}\n"

# add build rule for the final executable
obj_files = ' '.join([os.path.join(build_dir, os.path.splitext(file)[0] + '.o') for file in c_files + s_files])
build_ninja_content += f"build kernel8.elf: ld {obj_files}\n"
build_ninja_content += f"build kernel8.img: objcopy kernel8.elf\n"
build_ninja_content += f"build compile_commands.json: generate_compile_commands\n"
build_ninja_content += f"default kernel8.img compile_commands.json\n"

with open('build.ninja', 'w') as f:
    f.write(build_ninja_content)

