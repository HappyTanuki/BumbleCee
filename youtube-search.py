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

if uri_validator(sys.argv[1]) == True:
    exit()

from youtube_search import YoutubeSearch

results = YoutubeSearch(sys.argv[1], max_results=10).to_dict()

print(results[0]["id"])