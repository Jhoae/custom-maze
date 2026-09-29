# Custom Maze

미로의 장애물과 출발·도착점을 바꾸며 탐색 결과를 확인하는 C++ 프로그램입니다. **2024.12**에 기존 미로 탐색 프로그램을 확장했습니다.

## 주요 기능

- 미로 생성·파일 저장과 `.maz` 파일 불러오기
- 장애물 추가·삭제, 출발점·도착점 변경
- DFS/BFS 탐색과 경로 표시

## macOS 빌드·실행

macOS용 [openFrameworks 0.12.1 SDK](https://github.com/openframeworks/openFrameworks/releases/tag/0.12.1)와 Apple Command Line Tools가 필요합니다. SDK는 이 저장소에 포함하지 않습니다. 프로젝트와 SDK를 공백 없는 경로에 둡니다.

```sh
# 최초 1회: Command Line Tools가 없다면 설치
xcode-select --install

# 압축을 푼 SDK 경로를 인자로 전달
./build.sh /path/to/of_v0.12.1_osx_release
./run.sh
```

`OF_ROOT=/path/to/of_v0.12.1_osx_release ./build.sh`로 지정해도 됩니다. 결과는 `bin/CustomMaze.app`에 생성되며, 옆의 `bin/data/`를 함께 유지합니다.

## 사용 방법

1. `File → Open` 또는 `⌘O`로 `bin/data/NewMaz.maz`를 엽니다.
2. `View → Show DFS / Show BFS`에서 탐색 결과를 확인합니다.
3. `Obstacle`에서 장애물을 추가·삭제하고, `Positon`에서 출발·도착점을 변경합니다.
4. `Maze → create maze`로 미로를 생성할 수 있습니다. 저장 후 `File → Open`으로 새 파일을 엽니다.

`f`는 전체화면 전환, `Esc`는 전체화면 해제 또는 종료, `⌘Q`는 종료입니다.

## 구현 범위와 확인

기존 미로 탐색 코드에 미로 생성, 장애물 관리와 출발·도착점 변경 기능을 추가했습니다. 2026년 재실행 준비에서는 Windows 메뉴를 macOS 메뉴로 연결하고 파일 경로·문자열 처리를 호환 수정했습니다. 이 수정은 2024년 구현과 구분합니다.

macOS 14.5 / Apple Silicon에서 공개 사본의 빌드·실행, 예제 파일 열기와 DFS/BFS 경로 표시를 확인했습니다. 공개 정리 전의 같은 호환 사본에서는 예제 파일 열기, BFS 표시, 장애물 추가와 미로 생성·저장·재열기를 확인했습니다. 공개 사본의 모든 메뉴와 예외 입력을 검증한 것은 아닙니다.

기반 코드와 라이선스는 [NOTICE.md](NOTICE.md)를 참고하세요.
