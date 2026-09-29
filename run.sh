#!/bin/sh
set -eu

MAZE_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
MAZE_APP="$MAZE_ROOT/bin/CustomMaze.app/Contents/MacOS/CustomMaze"

if [ ! -x "$MAZE_APP" ]; then
  printf '%s\n' 'Build the application first with ./build.sh /path/to/openFrameworks.' >&2
  exit 1
fi

cd "$MAZE_ROOT/bin/CustomMaze.app/Contents/MacOS"
exec ./CustomMaze
