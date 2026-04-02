# Changes

Versions follow semver for the command line, the report layout (its `report format` number) and
the ncdu JSON it writes.

## Unreleased

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
