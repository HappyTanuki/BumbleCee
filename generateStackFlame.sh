#!/bin/bash

# 기본값 설정
n_flag=20
i_flag=0
process_args=()

# 입력값 파싱
while [[ $# -gt 0 ]]; do
    case "$1" in
        -n)
            shift
            if [[ ! "$1" =~ ^[0-9]+$ ]]; then
                echo "오류: -n 옵션 뒤에는 숫자가 와야 합니다." >&2
                exit 1
            fi
            n_flag="$1"
            ;;
        -i)
            shift
            if [[ ! "$1" =~ ^[0-9]+$ ]]; then
                echo "오류: -i 옵션 뒤에는 숫자가 와야 합니다." >&2
                exit 1
            fi
            i_flag="$1"
            ;;
        *)
            # 프로세스 명 또는 PID 검증
            if [[ "$1" =~ ^[0-9]+$ ]]; then
                # 숫자인 경우, PID 존재 여부 확인
                if ! ps -p "$1" > /dev/null 2>&1; then
                    echo "오류: PID $1 에 해당하는 프로세스가 존재하지 않습니다." >&2
                    exit 1
                fi
            else
                # 문자열인 경우, 프로세스 이름 존재 여부 확인
                if ! pgrep -x "$1" > /dev/null 2>&1; then
                    echo "오류: 프로세스 '$1' 가 실행 중이지 않습니다." >&2
                    exit 1
                fi
            fi
            process_args+=("$1")
            ;;
    esac
    shift
done

# 프로세스 인자 검증
if [[ ${#process_args[@]} -eq 0 ]]; then
    echo "오류: 최소한 하나 이상의 프로세스 이름 또는 PID를 입력해야 합니다." >&2
    exit 1
fi

pid=$(pidof $process_args)

for x in $(seq 1 $n_flag)
    do
        sudo gdb -ex "set pagination 0" \
                -ex "thread apply all bt" \
                -batch -p $pid >> out.gdb 2> /dev/null 
        sleep $i_flag
    done

./stackcollapse-gdb.pl out.gdb > out.folded
rm out.gdb
./flamegraph.pl out.folded > stackflame.svg
rm out.folded