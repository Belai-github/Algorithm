import os
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent

for path in os.listdir(BASE_DIR / "src"):
    # Check if the file is a C++ source file
    if not path.endswith(".cpp"):
        continue
    contest = path[:3]
    try:
        with open(BASE_DIR / "src" / path, encoding="utf-8_sig") as f:
            code = f.read()
    except:
        print(f"Error reading {path}. Skipping.")
        continue
    if code == "":
        os.remove(BASE_DIR / "src" / path)
        print(path)
print("Cleaned up empty C++ files in 'src' directory.")
