#!/bin/bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# 깨끗이
rm -f main.o bmp.o image.o ui.o app.o img  

# 각 소스 파일 컴파일
gcc -std=c11 -O2 -Wall -Wextra -c main.c
gcc -std=c11 -O2 -Wall -Wextra -c bmp.c
gcc -std=c11 -O2 -Wall -Wextra -c image.c
gcc -std=c11 -O2 -Wall -Wextra -c ui.c
gcc -std=c11 -O2 -Wall -Wextra -c app.c

# 링크
gcc -std=c11 -O2 -Wall -Wextra -o img main.o bmp.o image.o ui.o app.o

# 실행 (macOS)
osascript -e 'tell application "Terminal"
    activate
    do script "cd '"$SCRIPT_DIR"' && ./img"
end tell'
