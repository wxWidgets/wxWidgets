#!/usr/bin/env python3
"""Run the hidden wxGLCanvas Catch test in a private headless Sway instance."""
import argparse
from datetime import datetime
import json
import math
import os
from pathlib import Path
import re
import shutil
import signal
import subprocess
import tempfile
import time

HERE = Path(__file__).resolve().parent
PROTOCOL_ERROR = re.compile(r'wl_display.*error\(.*(?:not divisible by scale|not an integer multiple)')
CASES = [
    ("odd-height", 1012, 743, 1, 2, False),
    ("even-size", 1012, 742, 1, 2, False),
    ("odd-width", 1013, 742, 1, 2, False),
    ("late-current", 1012, 743, 1, 2, True),
    ("stable-scale-2", 1012, 743, 2, 2, False),
    ("stable-scale-1", 1012, 743, 1, 1, False),
    ("fractional-scale", 1012, 743, 1, 1.25, False),
    ("scale-down", 1012, 743, 2, 1, False),
    ("scale-down-late-current", 1012, 743, 2, 1, True),
    ("round-trip", 1012, 743, 1, 2, False),
]


def wait_for(predicate, process, seconds=10):
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        if predicate():
            return
        if process.poll() is not None:
            raise RuntimeError(f"Process exited before initialisation: {process.returncode}")
        time.sleep(0.01)
    raise TimeoutError("Timed out waiting for startup")


def stop(process):
    if process.poll() is None:
        process.terminate()
        try:
            process.wait(timeout=3)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--test", type=Path, required=True, help="Path to the built test_glcanvas executable")
    parser.add_argument("--repeat", type=int, default=1)
    parser.add_argument("--case", choices=[case[0] for case in CASES], action="append")
    parser.add_argument("--sway", default=shutil.which("sway"))
    parser.add_argument("--swaymsg", default=shutil.which("swaymsg"))
    parser.add_argument("--results", type=Path, default=HERE / "results", help="Directory for per-run logs")
    args = parser.parse_args()
    if not args.sway or not args.swaymsg:
        parser.error("Install sway, or supply --sway and --swaymsg")
    if args.repeat < 1:
        parser.error("--repeat must be positive")
    binary = args.test.resolve()
    if not binary.is_file():
        parser.error("Build the test_glcanvas target first")
    results_dir = args.results.resolve() / datetime.now().strftime("%Y%m%d-%H%M%S-%f")
    results_dir.mkdir(parents=True)
    metadata = {"test_binary": str(binary)}
    for name, command in {
        "sway": [args.sway, "--version"],
        "platform": ["uname", "-a"],
        "gtk": ["pkg-config", "--modversion", "gtk+-3.0"],
        "libraries": ["ldd", str(binary)],
        "checkout": ["git", "-C", str(HERE), "rev-parse", "HEAD"],
    }.items():
        result = subprocess.run(command, capture_output=True, text=True, check=True)
        metadata[name] = result.stdout.strip()
    (results_dir / "environment.json").write_text(json.dumps(metadata, indent=2) + "\n")

    results = []
    with tempfile.TemporaryDirectory(prefix="wx-wayland-test-") as runtime:
        env = os.environ.copy()
        for key in ("DISPLAY", "WAYLAND_DISPLAY", "SWAYSOCK", "I3SOCK",
                    "GDK_SCALE", "GDK_DPI_SCALE", "GTK_THEME", "GDK_BACKEND",
                    "WX_TEST_DISABLE_GUI", "WX_GL_TEST_WIDTH", "WX_GL_TEST_HEIGHT",
                    "WX_GL_TEST_INITIAL_SCALE", "WX_GL_TEST_FINAL_SCALE", "WX_GL_TEST_LATE_CURRENT",
                    "WX_GL_TEST_ROUND_TRIP"):
            env.pop(key, None)
        env.update(XDG_RUNTIME_DIR=runtime, WLR_BACKENDS="headless",
                   WLR_HEADLESS_OUTPUTS="1", WLR_RENDERER="gles2")
        with (results_dir / "sway.log").open("w") as compositor_log:
            compositor = subprocess.Popen(
                [args.sway, "-c", str(HERE / "sway.conf"), "-d"],
                env=env, stdout=compositor_log, stderr=subprocess.STDOUT)
            try:
                sockets = lambda: list(Path(runtime).glob("sway-ipc.*.sock"))
                displays = lambda: [p for p in Path(runtime).glob("wayland-*") if p.is_socket()]
                wait_for(lambda: sockets() and displays(), compositor)
                ipc = str(sockets()[0])
                display = displays()[0].name

                def message(command):
                    response = subprocess.run([args.swaymsg, "-s", ipc, "-r", command],
                                              env=env, check=True, capture_output=True, text=True)
                    if not all(item["success"] for item in json.loads(response.stdout)):
                        raise RuntimeError(response.stdout)

                for trial in range(1, args.repeat + 1):
                    for name, width, height, initial, final, late in CASES:
                        if args.case and name not in args.case:
                            continue
                        label = f"{trial}-{name}"
                        message(f"output HEADLESS-1 scale {initial}")
                        client_env = env | {
                            "WAYLAND_DISPLAY": display, "GDK_BACKEND": "wayland", "WAYLAND_DEBUG": "1",
                            "WX_GL_TEST_WIDTH": str(width), "WX_GL_TEST_HEIGHT": str(height),
                            "WX_GL_TEST_INITIAL_SCALE": str(math.ceil(initial)),
                            "WX_GL_TEST_FINAL_SCALE": str(math.ceil(final)),
                            "WX_GL_TEST_LATE_CURRENT": "1" if late else "0",
                            "WX_GL_TEST_ROUND_TRIP": "1" if name == "round-trip" else "0",
                        }
                        path = results_dir / f"{label}.log"
                        reason = ""
                        with path.open("w") as client_log:
                            client = subprocess.Popen([str(binary), "[wayland]"], env=client_env,
                                                      stdout=client_log, stderr=subprocess.STDOUT)
                            try:
                                wait_for(lambda: "initialised " in path.read_text(), client)
                                message(f"output HEADLESS-1 scale {final}")
                                if name == "round-trip":
                                    for scale in (initial, final):
                                        wait_for(lambda: f"scale-ready={scale}" in path.read_text(), client)
                                        message(f"output HEADLESS-1 scale {scale}")
                                client.wait(timeout=15)
                            except (RuntimeError, TimeoutError, subprocess.TimeoutExpired) as error:
                                reason = str(error)
                            finally:
                                stop(client)
                        text = path.read_text()
                        errors = [line for line in text.splitlines() if PROTOCOL_ERROR.search(line)]
                        completed = f"completed canvas={width}x{height} scale={math.ceil(final):.2f}" in text
                        passed = client.returncode == 0 and completed and not errors and not reason
                        result = dict(case=name, trial=trial, passed=passed, returncode=client.returncode,
                                      completed=completed, protocol_errors=errors, reason=reason,
                                      width=width, height=height, initial_scale=initial,
                                      final_scale=final, late_current=late)
                        results.append(result)
                        outcome = "protocol error" if errors else reason or f"exit {client.returncode}"
                        print(f"{'PASS' if passed else 'FAIL'} {label}: {outcome}", flush=True)
                        (results_dir / "summary.json").write_text(json.dumps(results, indent=2) + "\n")
            finally:
                stop(compositor)
    print(f"Logs: {results_dir}")
    return 0 if results and all(result["passed"] for result in results) else 1


if __name__ == "__main__":
    def terminate(signum, _frame):
        raise SystemExit(128 + signum)

    signal.signal(signal.SIGTERM, terminate)
    raise SystemExit(main())
