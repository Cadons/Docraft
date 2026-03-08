#!/bin/bash
# Build Docraft documentation
# Doxygen runs automatically inside conf.py, so a single Sphinx call
# generates XML + HTML in one step.
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "==> Building Docraft documentation..."
python3 -m sphinx -b html source build/html

echo "==> Done.  Open build/html/index.html"

