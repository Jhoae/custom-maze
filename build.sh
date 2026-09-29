#!/bin/sh
set -eu

MAZE_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
MAZE_SDK=${1:-${OF_ROOT:-}}

if [ -z "$MAZE_SDK" ]; then
  printf '%s\n' 'Usage: ./build.sh /path/to/openFrameworks' 'Or set OF_ROOT before running ./build.sh.' >&2
  exit 1
fi

if [ ! -f "$MAZE_SDK/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk" ]; then
  printf '%s\n' 'The selected directory is not an openFrameworks SDK.' >&2
  exit 1
fi

MAZE_SDK=$(CDPATH= cd -- "$MAZE_SDK" && pwd)
case "$MAZE_ROOT:$MAZE_SDK" in
  *[[:space:]]*)
    printf '%s\n' 'Use project and SDK paths without spaces (required by the openFrameworks Makefiles).' >&2
    exit 1
    ;;
esac

cd "$MAZE_ROOT"
make Release OF_ROOT="$MAZE_SDK"
mkdir -p bin/data
cp examples/NewMaz.maz bin/data/NewMaz.maz
printf '%s\n' 'Build complete. Run ./run.sh.'
