
# 이게 뭔가요?
C++ Dpp 라이브러리를 이용해서 개발된 간단한 디스코드 음악봇입니다!
<div align="center">
  <a href="https://github.com/brainboxdotcc/DPP" alt="DPP"> <img src="DPP-markdown-logo.png" /> </a>
</div>
[![CodeFactor](https://www.codefactor.io/repository/github/happytanuki/bumblecee/badge)](https://www.codefactor.io/repository/github/happytanuki/bumblecee)

# 어떻게 써요?
1. 실행파일 경로에 config.json 파일을 만들고 다음과 같이 입력하세요:
```
{
    "CLEAR_PREVIOUS_COMMAND": true,
    "FFMPEG_CMD": "ffmpeg",
    "LOGLEVEL": "debug",
    "TOKEN": "발급받은 토큰",
    "YTDLP_CMD": "./yt-dlp"
}
```
2. 봇을 초대하시고 사용하시면 됩니다.

# 명령어
## /p
노래를 예약합니다.
사용법:
/p (노래링크 또는 노래 제목)
## /q
현재 큐의 내용물을 확인합니다.
사용법:
/q
## /d
현재 큐의 내용물을 삭제합니다.
현재 재생중인 곡은 0번입니다.
사용법:
/d (큐의 노래번호)
## /s
현재 곡을 스킵합니다.
사용법:
/s
## /l
음성 채팅을 떠납니다.
사용법:
/l

# docker
happytanuki12/bumblebee:latest
