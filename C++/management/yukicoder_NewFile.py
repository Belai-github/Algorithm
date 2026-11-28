from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent
SRC_DIR = BASE_DIR / "src"

start = int(input("Enter the Problem's No. from: ").strip())
end = input("Enter the Problem's No. to: ").strip()
if not end:
    end = start + 1
else:
    end = int(end) + 1

assert 0 <= end - start < 1000

print(start, end)

for i in range(start, end):
    try:
        with open(SRC_DIR / f"yukicoder_{i}.cpp", "x") as f:
            print(f"Generated yukicoder_{i}.cpp. Problem URL: https://yukicoder.me/problems/no/{i}")
    except FileExistsError:
        print(f"yukicoder_{i}.cpp Already exists. Problem URL: https://yukicoder.me/problems/no/{i}")
