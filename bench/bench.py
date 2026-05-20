#!/usr/bin/env python3
import argparse
import os
import shutil
import statistics
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
DISKMOON = os.path.join(HERE, "..", "diskmoon")

TOOLS = {
    "diskmoon": lambda t, j: [DISKMOON, "-q", "-j", str(j), "-o", "/dev/null", t],
    "diskmoon-scan": lambda t, j: [DISKMOON, "-q", "-j", str(j), "-F", "ncdu", "-o", "/dev/null", t],
    "du": lambda t, j: ["du", "-a", t],
    "ncdu": lambda t, j: ["ncdu", "-0", "-t", str(j), "-o", "/dev/null", t],
    "gdu": lambda t, j: ["gdu", "-p", "-m", str(j), "-o", "/dev/null", t],
    "dust": lambda t, j: ["dust", "-T", str(j), "-p", "-b", "-d", "1000000", "-n", "1000000000", t],
    "dua": lambda t, j: ["dua", "-t", str(j), "aggregate", t],
    "pdu": lambda t, j: ["pdu", "--threads", str(j), "--json-output", t],
    "diskus": lambda t, j: ["diskus", "-j", str(j), t],
}

NOTES = {
    "dua": "dua has no full tree output; measured with aggregate (totals only)",
    "diskus": "diskus only prints the total",
}


def find(name, extra):
    if name.startswith("diskmoon"):
        return os.path.exists(DISKMOON)
    return shutil.which(name, path=os.pathsep.join(filter(None, [extra, os.environ.get("PATH")])))


def drop_caches():
    subprocess.run(["sync"], check=True)
    with open("/proc/sys/vm/drop_caches", "w") as f:
        f.write("3\n")


def run_once(cmd, cold):
    if cold:
        drop_caches()
    start = time.perf_counter()
    child = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    _, _, usage = os.wait4(child.pid, 0)
    wall = time.perf_counter() - start
    child.returncode = 0
    return wall, usage.ru_utime + usage.ru_stime, usage.ru_maxrss


def measure(cmd, runs, cold):
    if not cold:
        run_once(cmd, False)
    samples = [run_once(cmd, cold) for _ in range(runs)]
    walls = [s[0] for s in samples]
    return min(walls), statistics.median(walls), statistics.mean(s[1] for s in samples), max(s[2] for s in samples)


def count_entries(target):
    out = subprocess.run([DISKMOON, "-q", "-d0", "-n0", "-F", "csv", "-o", "-", target], capture_output=True, text=True).stdout
    line = out.splitlines()[1] if out.count("\n") > 1 else ""
    fields = line.rsplit(",", 4)
    return int(fields[2]) + 1 if len(fields) == 5 and fields[2].isdigit() else 0


def parse_args():
    p = argparse.ArgumentParser(description="Compare disk usage scanners on one directory.")
    p.add_argument("target")
    p.add_argument("-n", "--runs", type=int, default=10)
    p.add_argument("-j", "--jobs", type=int, default=os.cpu_count())
    p.add_argument("--cold", action="store_true", help="drop the page cache before every run (root)")
    p.add_argument("--bin", help="extra directory to look for the other tools")
    p.add_argument("--only", help="comma separated tool names")
    return p.parse_args()


def main():
    a = parse_args()
    if a.cold and os.geteuid():
        sys.exit("--cold needs root")
    names = a.only.split(",") if a.only else list(TOOLS)
    print(f"target {a.target}  runs {a.runs}  jobs {a.jobs}  cache {'cold' if a.cold else 'warm'}")
    entries = count_entries(a.target)
    print(f"entries {entries:,}")
    print(f"{'tool':<15}{'min':>9}{'median':>9}{'cpu':>9}{'rss':>10}{'B/entry':>9}")
    for name in names:
        exe = find(name, a.bin)
        if not exe:
            print(f"{name:<15}{'not found':>9}")
            continue
        cmd = TOOLS[name](a.target, a.jobs)
        if exe is not True:
            cmd[0] = exe
        best, median, cpu, rss = measure(cmd, a.runs, a.cold)
        per = rss * 1024 / entries if entries else 0
        print(f"{name:<15}{best:>8.2f}s{median:>8.2f}s{cpu:>8.2f}s{rss / 1024:>8.0f}MB{per:>9.1f}", flush=True)
    for name in names:
        if name in NOTES:
            print(f"note: {NOTES[name]}")


if __name__ == "__main__":
    main()
