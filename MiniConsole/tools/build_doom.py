"""Build the OLED or TFT launcher; optionally upload firmware and Doom assets."""
import argparse, hashlib, os, shutil, subprocess, sys, tempfile, urllib.request
from pathlib import Path

workspace = Path(__file__).resolve().parents[2]
sketch = workspace / "MiniConsole"
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--port", help="USB serial port; omit to build only")
parser.add_argument("--assets", action="store_true", help="install Doom WHD data, required on first installation")
parser.add_argument("--bench", action="store_true", help="enable USB input injection on launcher screens for testing")
parser.add_argument("--door-debug", action="store_true", help="trace Doom Use input and door line traversal over USB")
parser.add_argument("--display", choices=("oled", "tft"), default="oled", help="display hardware profile")
parser.add_argument("--build-dir", type=Path)
parser.add_argument("--asset-file", type=Path, default=workspace / "assets/doom1.whd", help="compressed shareware IWHD file")
parser.add_argument("--fetch-assets", action="store_true", help="download the pinned upstream shareware IWHD, verifying SHA-256")
args = parser.parse_args()
if args.build_dir is None:
    args.build_dir = Path(tempfile.gettempdir()) / ("miniconsole-tft114-build" if args.display == "tft" else "miniconsole-doom-build")
if args.assets and not args.port:
    parser.error("--assets requires --port")
cli = os.environ.get("ARDUINO_CLI") or shutil.which("arduino-cli")
if not cli:
    mac_cli = Path("/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli")
    if mac_cli.is_file(): cli = str(mac_cli)
if not cli:
    parser.error("Install arduino-cli or set ARDUINO_CLI to its executable")
esptool = [os.environ["ESPTOOL"]] if os.environ.get("ESPTOOL") else [sys.executable, "-m", "esptool"]
fqbn = "esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=4M,FlashMode=dio,PSRAM=enabled"
asset = args.asset_file
asset_sha = "1b3d0335b283e6132679291bf8ba026a16ef5442c3b97c00657a84e69d8100b4"
if args.fetch_assets:
    url = "https://raw.githubusercontent.com/aedile/DIABLITO/8602f28e31e9c35c7b6198f5235543cea9a53588/build-artifacts/doom1.whd"
    with urllib.request.urlopen(url, timeout=60) as response:
        data = response.read(2073677)
    if len(data) != 2073676 or hashlib.sha256(data).hexdigest() != asset_sha:
        raise SystemExit("Downloaded Doom asset failed integrity verification")
    asset.parent.mkdir(parents=True, exist_ok=True)
    asset.write_bytes(data)
if args.assets:
    if not asset.is_file():
        parser.error("Doom asset missing; use --fetch-assets or --asset-file PATH")
    data = asset.read_bytes()
    if len(data) != 2073676 or data[:4] != b"IWHD" or hashlib.sha256(data).hexdigest() != asset_sha:
        raise SystemExit("Incorrect Doom asset file; expected upstream 2,073,676-byte IWHD")
cflags = "-fms-extensions -Wno-error -w" + (" -DMINICONSOLE_TFT114=1" if args.display == "tft" else "")
if args.door_debug: cflags += " -DMINICONSOLE_DOOR_DIAGNOSTICS=1"
cppflags = cflags + " -Wno-narrowing -Wno-register" + (" -DMINICONSOLE_USB_BENCH=1" if args.bench else "")
subprocess.run([cli, "compile", "--fqbn", fqbn, "--libraries", str(workspace / "libraries"), "--build-property", "compiler.c.extra_flags=" + cflags,
                "--build-property", "compiler.cpp.extra_flags=" + cppflags,
                "--build-path", str(args.build_dir), str(sketch)], check=True, cwd=workspace)
binary = args.build_dir / "MiniConsole.ino.bin"
print("Application SHA-256:", hashlib.sha256(binary.read_bytes()).hexdigest(), flush=True)
if args.port:
    subprocess.run([cli, "upload", "--fqbn", fqbn, "--port", args.port,
                    "--input-dir", str(args.build_dir), str(sketch)], check=True, cwd=workspace)
    if args.assets:
        # Write only this partition. A padded merged image would overwrite NVS.
        subprocess.run(esptool + ["--chip", "esp32s3", "--port", args.port, "--baud", "460800",
                        "write-flash", "0x1e0000", str(asset)], check=True, cwd=workspace)
