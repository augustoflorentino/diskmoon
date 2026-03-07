# diskmoon

Disk usage for Linux. diskmoon walks a directory tree with one thread per CPU and writes where
the space went: the largest files and the folder tree, with sizes that match `du -sxB1` to the
byte. A file with several hard links is counted once.

```sh
make
./diskmoon /home -o home.txt      # text report
./diskmoon /home -o home.json     # ncdu JSON export
./diskmoon -f home.json           # report from an export
./diskmoon /home -F csv -o -      # one CSV row per entry
```

`./diskmoon -h` lists the options. `make check` runs the tests on generated trees.

MIT license.
