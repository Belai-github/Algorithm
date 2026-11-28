import requests
from bs4 import BeautifulSoup
import sys
import json


def get_execution_time_limit(atcoder_url):
    try:
        with open("../script/Timelimit_cache.json", "r", encoding="utf-8") as f:
            saved_limits = json.load(f)
            if atcoder_url in saved_limits:
                return saved_limits[atcoder_url]
        # 指定されたURLのHTMLを取得
        response = requests.get(atcoder_url)
        response.raise_for_status()  # エラーがあれば例外を発生

        # BeautifulSoupでHTMLを解析
        soup = BeautifulSoup(response.text, "html.parser")

        # 制限時間が記載された要素を抽出
        time_limit_element = soup.find("p", string=lambda x: x and "Time Limit" in x)  # type: ignore
        if time_limit_element:
            time_limit = str(time_limit_element).split()[3]
            # キャッシュに保存
            saved_limits[atcoder_url] = time_limit
            with open("../script/Timelimit_cache.json", "w", encoding="utf-8") as f:
                json.dump(saved_limits, f, ensure_ascii=False, indent=4)
            return time_limit
        else:
            return "5"
    except requests.exceptions.RequestException as e:
        return "5"


if __name__ == "__main__":
    try:
        url = str(sys.argv[1])
        print(get_execution_time_limit(url))
    except BaseException:
        print("5")
