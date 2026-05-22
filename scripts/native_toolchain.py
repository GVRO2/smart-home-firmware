Import("env")

import os
import shutil
from pathlib import Path


toolchain_bin = os.path.join(
    os.path.expanduser("~"),
    ".platformio",
    "packages",
    "toolchain-gccmingw32",
    "bin",
)

env.PrependENVPath("PATH", toolchain_bin)

build_dir = Path(env.subst("$BUILD_DIR"))
build_dir.mkdir(parents=True, exist_ok=True)

for dll_path in Path(toolchain_bin).glob("*.dll"):
    shutil.copy2(dll_path, build_dir / dll_path.name)