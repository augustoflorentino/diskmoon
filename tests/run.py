#!/usr/bin/env python3
import json
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
BIN = os.environ.get("DISKMOON", os.path.join(ROOT, "diskmoon"))


def run(*args, check=True):
    p = subprocess.run([BIN, "-q", *args], capture_output=True)
    if check and p.returncode:
        raise AssertionError(f"exit {p.returncode}: {' '.join(args)}\n{p.stderr.decode(errors='replace')}")
    return p


def total(path):
    line = run("-n0", "-d0", "-F", "csv", "-o", "-", path).stdout.decode(errors="replace").splitlines()[1]
    return int(line.rsplit(",", 4)[1])


def du(path):
    return int(subprocess.run(["du", "-sxB1", path], capture_output=True, text=True).stdout.split()[0])


def walk_total(path):
    st = os.lstat(path)
    seen, size = {(st.st_dev, st.st_ino)}, st.st_blocks * 512
    for _, dirs, files, fd in os.fwalk(path):
        for name in dirs + files:
            s = os.lstat(name, dir_fd=fd)
            if (s.st_dev, s.st_ino) not in seen:
                seen.add((s.st_dev, s.st_ino))
                size += s.st_blocks * 512
    return size


def without_timestamp(path):
    return re.sub(rb'"timestamp":\d+', b"", open(path, "rb").read())


def make_tree(base):
    synth = os.path.join(ROOT, "bench", "synth.py")
    subprocess.run([sys.executable, synth, "--entries", "4000", "--seed", "7", base], check=True, capture_output=True)


def make_links(base):
    os.makedirs(base)
    for i in range(30):
        d = os.path.join(base, f"d{i % 7}", f"s{i % 3}")
        os.makedirs(d, exist_ok=True)
        first = os.path.join(d, f"f{i}")
        open(first, "wb").write(os.urandom(1000 * (i + 1)))
        for k in range(i % 4):
            os.makedirs(os.path.join(base, f"d{(i + k + 1) % 7}"), exist_ok=True)
            os.link(first, os.path.join(base, f"d{(i + k + 1) % 7}", f"l{i}_{k}"))


def make_odd(base):
    os.makedirs(base)
    fd = os.open(base, os.O_RDONLY)
    for i in range(25):
        name = f"n{i:02d}" + "x" * 200
        os.mkdir(name, dir_fd=fd)
        child = os.open(name, os.O_RDONLY, dir_fd=fd)
        os.close(fd)
        fd = child
    deep = os.open("deep.bin", os.O_WRONLY | os.O_CREAT, dir_fd=fd)
    os.write(deep, os.urandom(50000))
    os.close(deep)
    os.close(fd)
    for name in [b"new\nline", b"tab\there", b"ctl\x01\x7f", b"bad\xff\xfeutf", b'quote"and\\slash', "acentuação".encode()]:
        open(os.path.join(os.fsencode(base), name), "wb").write(os.urandom(9000))


def test_total_matches_du(t):
    assert total(t["tree"]) == du(t["tree"])


def test_hard_links_counted_once(t):
    assert total(t["links"]) == du(t["links"])


def test_same_output_with_one_or_many_threads(t):
    for tree in (t["tree"], t["links"]):
        a, b = os.path.join(t["tmp"], "j1.json"), os.path.join(t["tmp"], "j8.json")
        run("-j1", "-o", a, tree)
        run("-j8", "-o", b, tree)
        assert without_timestamp(a) == without_timestamp(b), tree


def test_json_round_trip(t):
    a, b = os.path.join(t["tmp"], "a.json"), os.path.join(t["tmp"], "b.json")
    run("-o", a, t["links"])
    json.load(open(a))
    run("-f", a, "-o", b)
    assert open(a, "rb").read() == open(b, "rb").read()


def test_import_matches_scan(t):
    a = os.path.join(t["tmp"], "scan.json")
    run("-o", a, t["tree"])
    scanned = run("-F", "csv", "-o", "-", t["tree"]).stdout
    imported = run("-f", a, "-F", "csv", "-o", "-").stdout
    assert scanned == imported


def test_long_paths_and_odd_names(t):
    odd = t["odd"]
    assert total(odd) == walk_total(odd)
    csv = run("-F", "csv", "-o", "-", odd).stdout
    assert b"deep.bin" in csv and b"new\\nline" in csv and b"bad\\xff\\xfeutf" in csv
    assert max(len(line) for line in csv.splitlines()) > 5000
    a, b = os.path.join(t["tmp"], "odd.json"), os.path.join(t["tmp"], "odd2.json")
    run("-o", a, odd)
    run("-f", a, "-o", b)
    assert open(a, "rb").read() == open(b, "rb").read()


def test_report_sections(t):
    text = run("-o", "-", t["tree"]).stdout.decode(errors="replace")
    for section in ("SUMMARY", "FILESYSTEMS", "BY FILE TYPE", "LARGEST FILES", "TREE"):
        assert section in text, section


def test_compare_shows_new_file(t):
    tree, old = t["links"], os.path.join(t["tmp"], "old.json")
    run("-o", old, tree)
    grown = os.path.join(tree, "grown.bin")
    open(grown, "wb").write(os.urandom(200000))
    try:
        text = run("-c", old, "-o", "-", tree).stdout.decode(errors="replace")
    finally:
        os.unlink(grown)
    assert "grown.bin" in text


def test_duplicates(t):
    base = os.path.join(t["tmp"], "dupes")
    os.makedirs(base)
    data = os.urandom(300000)
    for name in ("a.bin", "b.bin", "c.bin"):
        open(os.path.join(base, name), "wb").write(data)
    open(os.path.join(base, "other.bin"), "wb").write(os.urandom(300000))
    text = run("--duplicates=100K", "-o", "-", base).stdout.decode(errors="replace")
    section = text[text.index("\nDUPLICATE FILES  ("):]
    section = section[:section.index("\n\n=====", 1)]
    assert "a.bin" in section and "c.bin" in section and "other.bin" not in section


def test_bad_arguments(t):
    assert run("--no-such-option", check=False).returncode == 1
    assert run("-f", os.path.join(t["tmp"], "missing.json"), check=False).returncode == 1
    assert run("-o", "/dev/null", os.path.join(t["tmp"], "missing"), check=False).returncode == 1
    assert run("--version").returncode == 0


def run_all(t):
    failures = []
    tests = [(name, fn) for name, fn in globals().items() if name.startswith("test_")]
    for name, fn in tests:
        try:
            fn(t)
            print(f"ok    {name}")
        except Exception as e:
            failures.append(name)
            print(f"FAIL  {name}: {e}")
    print(f"\n{len(tests) - len(failures)} passed, {len(failures)} failed")
    return 1 if failures else 0


def main():
    with tempfile.TemporaryDirectory(prefix="diskmoon-test-") as tmp:
        t = {"tmp": tmp, "tree": os.path.join(tmp, "tree"), "links": os.path.join(tmp, "links"), "odd": os.path.join(tmp, "odd")}
        make_tree(t["tree"])
        make_links(t["links"])
        make_odd(t["odd"])
        return run_all(t)


if __name__ == "__main__":
    sys.exit(main())
