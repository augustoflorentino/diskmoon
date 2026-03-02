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


def test_bad_arguments(t):
    assert run("--no-such-option", check=False).returncode == 1
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
        t = {"tmp": tmp, "tree": os.path.join(tmp, "tree"), "links": os.path.join(tmp, "links")}
        make_tree(t["tree"])
        make_links(t["links"])
        return run_all(t)


if __name__ == "__main__":
    sys.exit(main())
