#!/usr/bin/env bash
# Standalone reset-and-run. Use this if the target ever comes up halted
# after flashing instead of running - the same ST-Link firmware quirk
# you hit on the F407 project (SRST re-asserting a debug halt).
#
# Usage: ./reset.sh

set -e

CFG_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

openocd -f "$CFG_DIR/blackpill_f411.cfg" \
  -c "init; reset run; exit"
