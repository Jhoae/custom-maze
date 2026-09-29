# 기반 코드와 변경 범위

- `src/main.cpp`, `src/ofApp.h`, `src/ofApp.cpp`에는 Lynn Jarvis의 **ofxWinMenu basic example**에서 유래한 코드가 포함되어 있습니다. 각 파일의 원저작자 표시와 GNU LGPL v3 이상 고지를 유지했습니다. 원 출처: <https://github.com/leadedge/ofxWinMenu>.
- 기존 미로 탐색 프로그램을 바탕으로 2024.12에 미로 생성, 장애물 관리, 출발·도착점 변경 기능을 확장했습니다.
- 2026.09에는 macOS 재실행을 위해 `MacMenu.h`/`MacMenu.mm`의 Cocoa 메뉴 연결, 파일 경로·문자열 호환 처리를 추가했습니다. 공개용 정리에서는 화면 하단 문구, 실행과 무관한 안내 주석, 더 이상 쓰지 않는 글꼴 로드를 제거했습니다. 이 작업은 당시 기능 개발과 별도입니다.
- 원본의 LGPL 고지에 해당하는 라이선스 본문은 `LICENSES/LGPL-3.0.txt`와 해당 문서가 참조하는 `LICENSES/GPL-3.0.txt`에 포함했습니다. 파일 안의 기존 고지를 따릅니다.
- openFrameworks SDK와 그 외부 라이브러리, 글꼴, 실행 파일은 포함하지 않습니다. SDK의 라이선스는 해당 배포본을 참고하세요.
