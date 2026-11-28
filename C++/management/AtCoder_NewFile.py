import math
from pathlib import Path
from string import ascii_lowercase

BASE_DIR = Path(__file__).resolve().parent.parent
SRC_DIR = BASE_DIR / "src"

Name = input("Enter the Contest Name:").strip()
Prob = int(input("Enter the Number of Problems:").strip())

assert Prob < 1000

print(f"Contest URL: https://atcoder.jp/contests/{Name}")

for i in range(0, Prob):
    tag = ""
    digit = int(math.log(25 * i + 26) / math.log(26))
    loop = 0
    I = i
    for j in range(digit):
        tag = ascii_lowercase[I % 26 - (j > 0)] + tag
        I -= j > 0
        I //= 26
    try:
        with open(SRC_DIR / f"{Name}_{tag}.cpp", "x") as f:
            print(f"Generated {Name}_{tag}.cpp. Problem URL: https://atcoder.jp/contests/{Name}/tasks/{Name}_{tag}")
    except FileExistsError:
        print(f"{Name}_{tag}.cpp Already exists. Problem URL: https://atcoder.jp/contests/{Name}/tasks/{Name}_{tag}")
