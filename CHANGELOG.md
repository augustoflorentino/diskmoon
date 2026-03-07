# Changes

Versions follow semver for the command line, the report layout (its `report format` number) and
the ncdu JSON it writes.

## Unreleased

## 0.1.0 - 2026-03-06

### Added
- Parallel scan that counts each hard link once and matches `du -sxB1` to the byte.
- Text report with the largest files and the folder tree, `-d`, `-m`, `-n`, `--dirs-only`.
- ncdu JSON export and import (`-o scan.json`, `-f scan.json`), and CSV.
- Names are written as UTF-8; only bytes that are not valid UTF-8, and control characters, are
  escaped.
