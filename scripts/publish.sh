#!/usr/bin/env sh
set -eu

PRESET="ninja-release"
OUTPUT_DIR=""
SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
REPO_ROOT=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)

show_usage() {
  echo "Usage: $0 [output-dir] [--preset <name>]"
  echo "       $0 --output-dir <path> [--preset <name>]"
}

OUTPUT_SET=0
while [ "$#" -gt 0 ]; do
  case "$1" in
    --output-dir|-o)
      if [ "$#" -lt 2 ]; then
        echo "ERROR: --output-dir requires a value." >&2
        exit 2
      fi
      OUTPUT_DIR="$2"
      OUTPUT_SET=1
      shift 2
      ;;
    --preset)
      if [ "$#" -lt 2 ]; then
        echo "ERROR: --preset requires a value." >&2
        exit 2
      fi
      PRESET="$2"
      shift 2
      ;;
    -h|--help)
      show_usage
      exit 0
      ;;
    -*)
      echo "ERROR: unknown option $1" >&2
      show_usage >&2
      exit 2
      ;;
    *)
      if [ "$OUTPUT_SET" -ne 0 ]; then
        echo "ERROR: unexpected argument $1" >&2
        show_usage >&2
        exit 2
      fi
      OUTPUT_DIR="$1"
      OUTPUT_SET=1
      shift
      ;;
  esac
done

if [ -z "$OUTPUT_DIR" ]; then
  OUTPUT_DIR="$REPO_ROOT"
fi

case "$OUTPUT_DIR" in
  /*) ;;
  *) OUTPUT_DIR="$REPO_ROOT/$OUTPUT_DIR" ;;
esac

case "$(uname -s 2>/dev/null || echo unknown)" in
  MINGW*|MSYS*|CYGWIN*) EXE_SUFFIX=".exe" ;;
  *) EXE_SUFFIX="" ;;
esac

cd "$REPO_ROOT"

cmake --preset "$PRESET"
cmake --build --preset "$PRESET" --target codelinecalculator

EXE_PATH="$REPO_ROOT/build/$PRESET/bin/codelinecalculator$EXE_SUFFIX"
if [ ! -f "$EXE_PATH" ]; then
  echo "ERROR: built executable was not found: $EXE_PATH" >&2
  exit 1
fi

mkdir -p "$OUTPUT_DIR"
DESTINATION="$OUTPUT_DIR/codelinecalculator$EXE_SUFFIX"
cp "$EXE_PATH" "$DESTINATION"

echo "Published: $DESTINATION"
