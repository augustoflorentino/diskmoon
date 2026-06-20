# Changes

Versions follow semver for the command line, the report layout (its `report format` number) and
the ncdu JSON it writes.

## Unreleased

## 1.0.0 - 2026-06-19

### Added
- Manual page, installed by `make install` with the binary.
- `make check-sanitize` runs the tests under ASan, UBSan and LSan.
- `bench/bench.py` times diskmoon against ncdu, gdu, dust, dua and du, warm or with a cold cache.
- `make dist` and an Arch package in `packaging/aur`.
- `INDEX.txt` with what each file does.

## 0.5.0 - 2026-05-12

### Added
- `-c FILE` compares with an older ncdu JSON scan: what grew, shrank, appeared and went away,
  as a tree or as CSV.
- `--save` keeps each scan in `~/.local/share/diskmoon/scans`; `-c last` compares with the newest.
- Changes tab, and `S` in the interactive view to save the scan it shows.

## 0.4.0 - 2026-04-11

### Added
- SUMMARY in plain words at the top of the report, and WHAT YOU CAN FREE: package caches, build
  output and other known folders, each with the command that cleans it, marked safe or check.
- BY FILE TYPE and UNTOUCHED FOR OVER A YEAR sections.
- Free tab with the same list; enter shows the item in Files.
- `--duplicates[=SIZE]` finds files with the same content, comparing size, then the first 64K,
  then everything.

## 0.3.0 - 2026-04-02

### Added
- Interactive view when stdin and stdout are a terminal: Report and Files tabs, filter, sort,
  search, and `w` to write the full report.

## 0.2.0 - 2026-04-01

### Added
- FILESYSTEMS: size, used and free of each disk next to what the scan found.
- CAPACITY: device size against what the filesystem offers, and the part kept for root.
- NOT SEEN: space a disk counts as used that no visible path holds, split into deleted files
  still open (with the program holding them) and files hidden under mount points.

## 0.1.0 - 2026-03-06

### Added
- Parallel scan that counts each hard link once and matches `du -sxB1` to the byte.
- Text report with the largest files and the folder tree, `-d`, `-m`, `-n`, `--dirs-only`.
- ncdu JSON export and import (`-o scan.json`, `-f scan.json`), and CSV.
- Names are written as UTF-8; only bytes that are not valid UTF-8, and control characters, are
  escaped.
