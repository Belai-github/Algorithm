import sys


def convert_encoding_in_place(file_path):
    try:
        # ファイルを読み込む
        with open(file_path, "r", encoding="utf-16le") as infile:
            content = infile.read()

        # UTF-8形式で上書き
        with open(file_path, "w", encoding="utf-8") as outfile:
            outfile.write(content)

        print(f"Successfully converted {file_path} from UTF-16LE to UTF-8 in-place.")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python convert_encoding.py <file_path>")
    else:
        file_path = sys.argv[1]
        convert_encoding_in_place(file_path)
