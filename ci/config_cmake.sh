#!/bin/bash -e

ROOT=$(cd "$(dirname "$0")/.."; pwd)
cd "$ROOT"

# Get juce
if [ ! -d "juce" ]; then
  git clone https://github.com/WeAreROLI/JUCE.git --branch develop --single-branch juce
fi

if [ "$(uname)" == "Darwin" ]; then
  cd "$ROOT"
  cmake --preset xcode
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  cd "$ROOT"
  cmake --preset ninja-gcc
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
  cd "$ROOT"
  cmake --preset vs
fi

