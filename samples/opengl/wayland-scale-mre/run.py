#!/usr/bin/env python3
"""Run the wxGLCanvas reproducer in a private, headless Sway instance."""
import argparse
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import time

HERE = Path(__file__).resolve().parent
PROTOCOL_ERROR = re.compile(r'wl_display.*error\(.*(?:not divisible by scale|not an integer multiple)')


def wait_for(predicate, process, seconds=10):
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        if predicate():
            return
        if process.poll() is not None:
            raise RuntimeError(f"Process exited early with {process.returncode}")
        time.sleep(0.01)
    raise TimeoutError("Timed out waiting for startup; inspect the saved logs")


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
    parser.add_argument("--repeat", type=int, default=3)
    parser.add_argument("--sway", default=shutil.which("sway"))
    parser.add_argument("--swaymsg", default=shutil.which("swaymsg"))
    parser.add_argument("--results", type=Path, default=HERE / "results" / time.strftime("%Y%m%d-%H%M%S"))
    args = parser.parse_args()
    if not args.sway or not args.swaymsg:
        parser.error("Install sway, or supply --sway and --swaymsg")
    if args.repeat < 1:
        parser.error("--repeat must be positive")
    args.results.mkdir(parents=True, exist_ok=False)
    binary = HERE / "build" / "wx-wayland-scale-mre"
    if not binary.exists():
        parser.error("Run ./build.sh first")

    metadata = {}
    for name, command in {
        "wxWidgets": [os.environ.get("WX_CONFIG", "wx-config"), "--version-full"],
        "sway": [args.sway, "--version"],
        "platform": ["uname", "-a"],
        "gtk": ["pkg-config", "--modversion", "gtk+-3.0"],
        "checkout": ["git", "-C", str(HERE), "rev-parse", "HEAD"],
    }.items():
        result = subprocess.run(command, capture_output=True, text=True, check=True)
        metadata[name] = result.stdout.strip()
    (args.results / "environment.json").write_text(json.dumps(metadata, indent=2) + "\n")

    # Both a parity control and controls for allocation timing and output scale.
    cases = [
        ("odd-height", 1012, 743, 1, 2, False, True),
        ("even-size", 1012, 742, 1, 2, False, False),
        ("odd-width", 1013, 742, 1, 2, False, True),
        ("late-current", 1012, 743, 1, 2, True, False),
        ("stable-scale-2", 1012, 743, 2, 2, False, False),
        ("stable-scale-1", 1012, 743, 1, 1, False, False),
        ("fractional-scale", 1012, 743, 1, 1.25, False, True),
    ]
    results = []
    with tempfile.TemporaryDirectory(prefix="wx-wayland-mre-") as runtime:
        env = os.environ.copy()
        # Do not connect to or export IPC variables into the real desktop.
        for key in ("DISPLAY", "WAYLAND_DISPLAY", "SWAYSOCK", "I3SOCK",
                    "GDK_SCALE", "GDK_DPI_SCALE", "GTK_THEME", "GDK_BACKEND"):
            env.pop(key, None)
        env.update(XDG_RUNTIME_DIR=runtime, WLR_BACKENDS="headless",
                   WLR_HEADLESS_OUTPUTS="1", WLR_RENDERER="gles2")
        with (args.results / "sway.log").open("w") as compositor_log:
            compositor = subprocess.Popen(
                [args.sway, "-c", str(HERE / "sway.conf"), "-d"],
                env=env, stdout=compositor_log, stderr=subprocess.STDOUT)
            try:
                sockets = lambda: list(Path(runtime).glob("sway-ipc.*.sock"))
                displays = lambda: [p for p in Path(runtime).glob("wayland-*") if p.is_socket()]
                wait_for(lambda: sockets() and displays(), compositor)
                ipc = str(sockets()[0])
                display = displays()[0].name

                def message(command, kind=None):
                    invocation = [args.swaymsg, "-s", ipc, "-r"]
                    invocation += ["-t", kind] if kind else [command]
                    response = subprocess.run(invocation, env=env, check=True,
                                              capture_output=True, text=True)
                    data = json.loads(response.stdout)
                    if not kind and not all(item["success"] for item in data):
                        raise RuntimeError(response.stdout)
                    return data

                for trial in range(1, args.repeat + 1):
                    for name, width, height, initial, final, late, expect_error in cases:
                        label = f"{trial}-{name}"
                        message(f"output HEADLESS-1 scale {initial}")
                        client_env = env | {
                            "WAYLAND_DISPLAY": display, "GDK_BACKEND": "wayland",
                            "WAYLAND_DEBUG": "1", "MRE_WIDTH": str(width),
                            "MRE_HEIGHT": str(height), "MRE_WAIT_SCALE": str(math.ceil(final)),
                        }
                        client_env.pop("MRE_LATE_CURRENT", None)
                        if late:
                            client_env["MRE_LATE_CURRENT"] = "1"
                        path = args.results / f"{label}.log"
                        with path.open("w") as client_log:
                            client = subprocess.Popen([str(binary)], env=client_env,
                                                      stdout=client_log, stderr=subprocess.STDOUT)
                            try:
                                wait_for(lambda: "initialised " in path.read_text(), client)
                                message(f"output HEADLESS-1 scale {final}")
                                # A successful control must present a frame and stay alive.
                                deadline = time.monotonic() + 1.5
                                while client.poll() is None and time.monotonic() < deadline:
                                    time.sleep(0.01)
                                alive = client.poll() is None
                                returncode = client.poll()
                                outputs = message(None, "get_outputs")
                            finally:
                                stop(client)
                        text = path.read_text()
                        errors = [line for line in text.splitlines() if PROTOCOL_ERROR.search(line)]
                        presented = "frame presented" in text
                        passed = (bool(errors) and not alive and returncode == 1) if expect_error else (alive and presented and not errors)
                        result = dict(case=name, trial=trial, passed=passed, alive=alive,
                                      returncode=returncode, presented=presented, protocol_errors=errors,
                                      width=width, height=height, initial_scale=initial,
                                      final_scale=final, late_current=late, outputs=outputs)
                        results.append(result)
                        print(f"{'PASS' if passed else 'FAIL'} {label}: "
                              f"{'protocol error' if errors else 'alive' if alive else f'exit {returncode}'}",
                              flush=True)
                        (args.results / "summary.json").write_text(json.dumps(results, indent=2) + "\n")
            finally:
                stop(compositor)
    print(f"Logs: {args.results}")
    return 0 if all(result["passed"] for result in results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
