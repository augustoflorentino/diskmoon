# Security

diskmoon only reads. It never deletes, moves or changes a file; the cleanup commands it prints
are for you to run. Still, it is often run as root over a whole disk, so a flaw in how it reads
directories, names or `/proc` matters.

## Reporting

Open a private advisory at
<https://github.com/augustoflorentino/diskmoon/security/advisories/new>. Please do not use a public
issue. Include the version (`diskmoon -V`), the filesystem, and a way to build the tree that
triggers it, for example a short script.

You will get an answer within a week. Fixes go into a patch release of the latest version only.

## In scope

- Memory errors on any directory tree, name, or ncdu JSON file given to `-f` or `-c`.
- Following a symbolic link, or crossing into another filesystem without `--all-fs`.
- A printed cleanup command that would remove more than the report says it does.
