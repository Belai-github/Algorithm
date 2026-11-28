import os
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent
SRC_DIR = BASE_DIR / "src"

for path in os.listdir(SRC_DIR):
    # Check if the file is a C++ source file
    if not path.endswith(".py"):
        continue
    contest = path[:3]
    try:
        with open(SRC_DIR / path, encoding="utf-8_sig") as f:
            code = f.read()
    except:
        print(f"Error reading {path}. Skipping.")
        continue
    if code == "":
        os.remove(SRC_DIR / path)
        print(path)
print("Cleaned up empty Python files in 'src' directory.")
