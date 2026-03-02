#!/usr/bin/env python3
import argparse
import os
import random
import string

NAME_CHARS = string.ascii_lowercase + string.digits + "-_."
EXTENSIONS = ["", ".js", ".ts", ".json", ".md", ".png", ".jpg", ".txt", ".so", ".py", ".rs", ".c", ".h", ".log", ".bin"]


def parse_args():
    p = argparse.ArgumentParser(description="Build a reproducible tree of real files for benchmarks and accuracy checks.")
    p.add_argument("target")
    p.add_argument("--entries", type=int, default=1_000_000)
    p.add_argument("--seed", type=int, default=20261123)
    p.add_argument("--max-file", type=int, default=16384)
    return p.parse_args()


def random_name(rng):
    stem = "".join(rng.choice(NAME_CHARS) for _ in range(rng.randint(3, 24)))
    return stem + rng.choice(EXTENSIONS)


def fan_out(rng, depth):
    return rng.randint(2, 12) if depth < 6 else 0


def write_file(rng, path, max_file):
    size = int(rng.paretovariate(1.2) * 200) % max_file
    with open(path, "wb") as f:
        f.write(rng.randbytes(size) if size else b"")


def build(a):
    rng = random.Random(a.seed)
    os.makedirs(a.target)
    dirs = [(a.target, 0)]
    made = 0
    files = []
    while made < a.entries and dirs:
        base, depth = dirs.pop(rng.randrange(len(dirs)))
        for _ in range(fan_out(rng, depth)):
            path = os.path.join(base, "d" + random_name(rng))
            os.mkdir(path)
            dirs.append((path, depth + 1))
            made += 1
        for _ in range(rng.randint(0, 40)):
            if made >= a.entries:
                break
            path = os.path.join(base, random_name(rng))
            if os.path.exists(path):
                continue
            kind = rng.random()
            if kind < 0.02 and files:
                os.link(rng.choice(files), path)
            elif kind < 0.03:
                os.symlink(random_name(rng), path)
            else:
                write_file(rng, path, a.max_file)
                files.append(path)
            made += 1
    print(f"{made:,} entries under {a.target}")


if __name__ == "__main__":
    build(parse_args())
