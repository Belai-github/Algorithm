import sys
from http.cookiejar import LWPCookieJar

import requests
from bs4 import BeautifulSoup
from onlinejudge.service.atcoder import AtCoderProblem
from onlinejudge.utils import default_cookie_path
from onlinejudge_command import __about__ as oj_version


def can_submit(problem_url: str) -> bool:
    problem = AtCoderProblem.from_url(problem_url)
    if problem is None:
        print("AtCoder の問題 URL を確認できませんでした。", file=sys.stderr)
        return False

    action = f"/contests/{problem.contest_id}/submit"
    try:
        with requests.Session() as session:
            session.cookies = LWPCookieJar(str(default_cookie_path))
            session.cookies.load(ignore_discard=True)
            session.headers["User-Agent"] = (
                f"{oj_version.__package_name__}/{oj_version.__version__} (+{oj_version.__url__})"
            )
            response = session.get(
                f"https://atcoder.jp{action}", allow_redirects=False, timeout=5
            )
    except FileNotFoundError:
        return False
    except (OSError, requests.RequestException):
        print("AtCoder の提出フォームを確認できませんでした。", file=sys.stderr)
        return False

    if response.is_redirect and response.headers["Location"].startswith("/login"):
        return False
    if response.status_code != 200:
        print(
            f"AtCoder の提出フォームを取得できませんでした (HTTP {response.status_code})。",
            file=sys.stderr,
        )
        return False

    soup = BeautifulSoup(response.content, "html.parser")
    form = soup.find("form", action=action)
    if form is None or form.get("method", "").lower() != "post":
        print("AtCoder の提出フォームが見つかりませんでした。", file=sys.stderr)
        return False
    if form.select_one(".cf-turnstile, [data-sitekey], [name='cf-turnstile-response']"):
        print("AtCoder で提出時の CAPTCHA 認証が必要です。", file=sys.stderr)
        return False
    return True


if __name__ == "__main__":
    sys.exit(0 if can_submit(sys.argv[1]) else 1)
