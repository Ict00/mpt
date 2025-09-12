# MPT - My Project Tool

A tool I've made for project management (Creating/Building)

### Features
- Project configuration
- Different build strategies [`dummy`, `c_console`, `cpp_console`, `c_static_lib`, `c_shared_lib`, `cpp_static_lib`, `cpp_shared_lib`]. The list can (and **will**) change over time
- Project templates

### Requirements
- Compiler that supports C99
- Make

### Usage
MPT Usage: mpt \[command] \[args]

#### Commands:
| Command                             | Description                                                           |
|-------------------------------------|-----------------------------------------------------------------------|
| info                                | - Show current version                                                |
| help                                | - Help command                                                        |
| gen-cf \[target]                    | - Generate 'compile_flags.txt' based on 'flags' and 'includes' fields |
| build   \[target]                   | - Build target                                                        |
| template \[template_name] \[dir]    | - Make template based on directory                                    |
| new     \[template] \[project name] | - Make new project from template                                      |

#### Project configuration:
* Project configuration is stored in 'Project' file
* **Project file** syntax:
```ini
strategy="..."
[target_1]
compiler="..."
flags="..."
ldflags="..."
sources="dir1:dir2:dirn"
includes="dir1:dir2:dirn"
output="..."
post_cmd="..."
init_cmd="..."
[target_n]
...
```
* Defaults for each parameter:

```ini
compiler="cc"
flags="-O3 -std=c23 -c"
ldflags="-O3"
sources="src"
includes=""
output="name of directory"
post_cmd=""
init_cmd=""
```

> NOTE:
> * 'obj' directory must exist (unless you use 'dummy' build strategy)
> * First parameter in any Project file must be 'strategy'

#### Templates:
* Templates are stored in '/path/to/mpt/binary/templates' OR ~/.config/mpt/
* **Template** syntax:
```ini
[dir1]
type="dir"

[dir1/dir2]
type="dir"

[dirN]
type="dir"

[file1]
type="file"
content="..."
[dir1/file1]
type="file"
content="..."
[dirN/.../fileN]
type="file"
content="..."
...
```
> NOTE:
> * Directories are created EXACTLY in the same order as specified in template, not recursively

> NOTE:
> **MPT V1** makes process of making templates way easier using `mpt template` command.
> 
> Let's say you have a directory with this structure and you want to turn it into template:
> ```
> project1
> ├── obj
> ├── Project
> └── src
>       ├── main.c
>       └── very_useful_functionality
>           ├── test.c
>           └── test.h
> ```
> Now you can just type this command:
> ```bash
> $ mpt new my_awesome_template project1
> ```
> and MPT will create `my_awesome_template` file with ready-to-use project template

### Quick Start
> NOTE: Windows is not supported; Mac OS is not test

#### Step 1. Clone the repository
```bash
$ git clone https://github.com/Ict00/mpt
```

#### Step 2. Build it using `Make`
```bash
$ cd mpt && make
```
> NOTE: If you don't have `clang` installed, change '`CC`' field in `Makefile` to any other compiler you have (e.g, `cc`)

#### Step 3 (Optional). Put `mpt` binary (and `templates` directory) into directory that's listed in `PATH` variable (for better experience)
#### Step 4. Use it. Here's example of creating `c_console` project:
```bash
$ mpt new c_console my_awesome_c_project
$ cd my_awesome_c_project
$ mpt build
$ ./my_awesome_c_project # <- output binary (name and location can be changed in 'Project' file)
```