import sys

if len(sys.argv) != 2:
    sys.exit()

import urllib.parse

def uri_validator(x):
    try:
        result = urllib.parse.urlparse(x)
        return all([result.scheme, result.netloc])
    except AttributeError:
        return False

#URL인 경우
if uri_validator(sys.argv[1]) == True:
    result = urllib.parse.urlparse(sys.argv[1])

    #플레이리스트인 경우
    if result.path == '/playlist':
        import re, requests

        response = requests.get("https://www.youtube.com/playlist?" + result.query)

        pattern = re.compile('"videoId":"(.{11})"')

        list = set(pattern.findall(response.text))
        for it in list:
            print(it)
    #영상인 경우
    elif result.path == '/watch':
        print(result.query[2:13])
else:
    from youtube_search import YoutubeSearch

    results = YoutubeSearch(sys.argv[1], max_results=10).to_dict()

    #검색 결과가 없는 경우 확인 불가
    print(results[0]["id"])