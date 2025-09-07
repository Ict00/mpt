# MPT - My Project Tool

A tool I've made for project management (Creating/Building)

### Features
- Project configuration
- Different build strategies [`dummy`, `c_console`, `cpp_console`]. The list can (and **will**) change over time
- Project templates

### Requirements
- Compiler that supports C23
- Make

### Usage
MPT Usage: mpt \[command] \[args]

#### Commands:
| Command                             | Description                      |
|-------------------------------------|----------------------------------|
| info                                | - Show current version           |
| build   \[target]                   | - Build target                   |
| new     \[template] \[project name] | - Make new project from template |

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
output="..."
post_cmd="..."
[target_n]
...
```
* Defaults for each parameter:

```ini
compiler="cc"
flags="-O3 -std=c23 -c"
ldflags="-O3"
sources="src"
output="name of directory"
post_cmd=""
```

> NOTE:
> * 'obj' directory must exist (unless you use 'dummy' build strategy)
> * First parameter in any Project file must be 'strategy'

#### Templates:
* Templates are stored in ~/.config/mpt/
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