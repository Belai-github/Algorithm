import math
from pathlib import Path
from string import ascii_lowercase

BASE_DIR = Path(__file__).resolve().parent.parent
SRC_DIR = BASE_DIR / "src"
start = 319
end = 500


for constest in range(start, end):
    Name = f"abc{constest:03}"
    for i in range(0, 8):
        tag = ""
        digit = int(math.log(25 * i + 26) / math.log(26))
        loop = 0
        I = i
        for j in range(digit):
            tag = ascii_lowercase[I % 26 - (j > 0)] + tag
            I -= j > 0
            I //= 26
        try:
            with open(SRC_DIR / f"{Name}_{tag}.py", "x") as f:
                print(f"Generated {Name}_{tag}.py. Problem URL: https://atcoder.jp/contests/{Name}/tasks/{Name}_{tag}")
        except FileExistsError:
            print(f"{Name}_{tag}.py Already exists. Problem URL: https://atcoder.jp/contests/{Name}/tasks/{Name}_{tag}")
