# 🔍 FileChangeDetection

A lightweight Windows CLI tool written in C that snapshots a directory tree and compares it against a previously saved snapshot to detect added, modified, or deleted files.

---

## Features

- **Directory traversal** — Recursively walks a directory tree and records its structure
- **Snapshot saving** — Persists a directory snapshot to a custom binary file with `-o` for later comparison
- **Manual diffing** — Compare a live directory scan against any saved snapshot file
- **Diff reporting** — Optionally show added/deleted files (`-d`) and write results to a file (`-w`)
- **O(n) comparison** — Uses hash tables for path lookups, so diffing two snapshots scales linearly with the number of entries
- **Modular design** — Logic cleanly split across `traverse`, `utils`, and `types` modules

---

## Project Structure

```
FileChangeDetection/
├── main.c          # Entry point & CLI argument handling (argtable3)
├── traverse.c/.h   # Directory traversal logic
├── utils.c/.h      # Snapshot I/O, comparison, and stat output
├── types.h         # Shared type definitions
└── extern/         # External dependency: argtable3 and uthash
```

---

## Getting Started

### Prerequisites

- Windows (the project uses `windows.h` and Windows-specific APIs)
- MSVC (primary target) or MinGW
- The `argtable3` and `uthash` are bundled in `extern/`

### Build

```bash
git clone https://github.com/TheEugen/FileChangeDetection.git
cd FileChangeDetection
```

Open the project in Visual Studio, or compile manually with your preferred Windows C toolchain. Make sure `extern/argtable3.h` and its corresponding source file are included in your build.

---

## Usage

```
FileChangeDetection <dir> [options]
```

| Flag | Description |
|------|-------------|
| `<dir>` | Directory to scan |
| `-o <saveFile>` | Save the scanned directory structure to a binary snapshot file |
| `<file>` | Snapshot file to compare against |
| `-c` | Compare only (load both snapshots from files, no live scan) |
| `-d` | Show added and deleted files/directories in the diff |
| `-w <file>` | Write diff results to a file |
| `--help` | Display usage information |
| `--version` | Display version info |

### Examples

**Take a snapshot:**
```bash
FileChangeDetection C:\MyFolder -o snapshot.dat
```

**Compare current state against a snapshot:**
```bash
FileChangeDetection C:\MyFolder snapshot.dat -d
```

**Compare two saved snapshots (no live scan):**
```bash
FileChangeDetection C:\MyFolder -c snapshot.dat -d -w diff_output.txt
```

---

## How It Works

The tool is entirely manual — there is no background process or polling loop. You control when snapshots are taken and compared:

1. **Scan & save** — Run with `-o` to traverse a directory and serialize its structure to a custom binary file (storing root path, total size, file count, per-file name/path/size/last-write time, and directory entries)
2. **Scan & compare** — Run with a snapshot file to traverse the directory again and immediately diff the live state against the saved snapshot
3. **Compare only** — Run with `-c` and a snapshot file to compare two previously saved snapshots without doing a live scan

Comparison uses hash tables for efficient path lookups. Diff output (added/deleted entries) can be printed to the console with `-d` and/or written to a file with `-w`.

---

## License

This project is licensed under the [MIT License](LICENSE).