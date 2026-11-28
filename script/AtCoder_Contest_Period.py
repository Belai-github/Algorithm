import requests
from bs4 import BeautifulSoup
import sys
from datetime import datetime
import re
import json


def debug(*args, sep=" ", end="\n") -> None:
    print(*args, sep=sep, end=end, file=sys.stderr)


def get_contest_period(contest_url):
    """
    AtCoderのコンテストURLからコンテスト期間を取得し、
    現在がコンテスト期間内かどうかを判定する

    Args:
        contest_url (str): AtCoderのコンテストURL (tasks含む場合は除去)

    Returns:
        bool: コンテスト期間内ならTrue、そうでなければFalse
    """
    try:
        with open("../script/Contest_Period_cache.json", "r", encoding="utf-8") as f:
            saved_periods = json.load(f)
            if contest_url in saved_periods:
                start_str = saved_periods[contest_url]["start"]
                end_str = saved_periods[contest_url]["end"]
                start_datetime = parse_datetime(start_str)
                end_datetime = parse_datetime(end_str)
                if start_datetime and end_datetime:
                    current_time = datetime.now()
                    debug(f"Contest period: {start_str} - {end_str}")
                    debug(f"Current time: {current_time}")
                    is_active = start_datetime <= current_time <= end_datetime
                    debug(f"Contest is active: {is_active}")
                    return is_active

        # tasks部分を除去してコンテストのベースURLを取得
        if "/tasks/" in contest_url:
            base_url = contest_url.split("/tasks/")[0]
        else:
            base_url = contest_url

        # コンテストのトップページを取得
        response = requests.get(base_url)
        response.raise_for_status()

        soup = BeautifulSoup(response.text, "html.parser")

        # ページのテキスト全体からコンテスト時間を検索
        page_text = soup.get_text()

        # コンテスト時間の正規表現パターン
        # 例: 2024-01-14 21:00:00+0900 - 2024-01-14 22:40:00+0900 (local time)
        contest_time_pattern = r"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\+\d{4}\s*-\s*(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\+\d{4}"

        match = re.search(contest_time_pattern, page_text)
        if not match:
            debug("Contest time pattern not found in page")
            # デバッグ用: Contest Duration周辺のテキストを表示
            duration_match = re.search(r"Contest Duration:[^-]+-[^(]+\(local time\)", page_text)
            if duration_match:
                debug(f"Found Contest Duration text: {duration_match.group(0)}")
            return False

        start_datetime_str, end_datetime_str = match.groups()
        debug(f"Contest period:\n{match.group(0)}")

        # datetimeオブジェクトに変換
        start_datetime = parse_datetime(start_datetime_str)
        end_datetime = parse_datetime(end_datetime_str)

        if not start_datetime or not end_datetime:
            debug("Failed to parse datetime strings")
            return False

        # キャッシュに保存
        saved_periods[contest_url] = {"start": start_datetime_str, "end": end_datetime_str}
        with open("../script/Contest_Period_cache.json", "w", encoding="utf-8") as f:
            json.dump(saved_periods, f, ensure_ascii=False, indent=4)

        current_time = datetime.now()
        debug(f"Current time: {current_time}")

        # コンテスト期間内かどうかを判定
        is_active = start_datetime <= current_time <= end_datetime
        debug(f"Contest is active: {is_active}")

        return is_active

    except requests.exceptions.RequestException as e:
        debug(f"Network error: {e}")
        return False
    except Exception as e:
        debug(f"Error: {e}")
        return False


def parse_datetime(datetime_str):
    """
    YYYY-MM-DD HH:MM:SS形式の文字列をdatetimeオブジェクトに変換

    Args:
        datetime_str (str): 日時文字列 (YYYY-MM-DD HH:MM:SS)

    Returns:
        datetime: 変換されたdatetimeオブジェクト、失敗時はNone
    """
    try:
        return datetime.strptime(datetime_str, "%Y-%m-%d %H:%M:%S")
    except ValueError:
        try:
            # 分まで（秒なし）の場合も対応
            return datetime.strptime(datetime_str, "%Y-%m-%d %H:%M")
        except ValueError:
            return None


if __name__ == "__main__":
    try:
        if len(sys.argv) != 2:
            print("False")
            sys.exit(1)

        contest_url = str(sys.argv[1])
        is_active = get_contest_period(contest_url)

        if is_active:
            print("True")
            sys.exit(0)
        else:
            print("False")
            sys.exit(1)

    except Exception:
        print("False")
        sys.exit(1)
