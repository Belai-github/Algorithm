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
        with open(f"/home/ruthe/AtCoder/Algorithm/Python/src/yukicoder_{i}.py", "x") as f:
            print(f"Generated yukicoder_{i}.py https://yukicoder.me/problems/no/{i}")
    except FileExistsError:
        print(f"yukicoder_{i}.py Already exists. https://yukicoder.me/problems/no/{i}")
