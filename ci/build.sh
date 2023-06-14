#!/bin/bash -e

ROOT=$(cd "$(dirname "$0")/.."; pwd)
cd "$ROOT"

rm -Rf bin
mkdir -p bin

# Get juce
if [ ! -d "juce" ]; then
  git clone https://github.com/WeAreROLI/JUCE.git --branch develop --single-branch juce
fi

# Build mac version
if [ "$(uname)" == "Darwin" ]; then
  if [[ -n "$APPLICATION" ]]; then
    echo "Create a keychain"
    security create-keychain -p nr4aGPyz Keys.keychain

    echo $APPLICATION | base64 -D -o /tmp/Application.p12
    echo $INSTALLER | base64 -D -o /tmp/Installer.p12

    security import /tmp/Application.p12 -t agg -k Keys.keychain -P aym9PKWB -A -T /usr/bin/codesign
    security import /tmp/Installer.p12 -t agg -k Keys.keychain -P aym9PKWB -A -T /usr/bin/codesign

    security list-keychains -s Keys.keychain
    security default-keychain -s Keys.keychain
    security unlock-keychain -p nr4aGPyz Keys.keychain
    security set-keychain-settings -l -u -t 13600 Keys.keychain
    security set-key-partition-list -S apple-tool:,apple: -s -k nr4aGPyz Keys.keychain

    DEV_APP_ID="Developer ID Application: Roland Rabien (3FS7DJDG38)"
    DEV_INST_ID="Developer ID Installer: Roland Rabien (3FS7DJDG38)"
  fi

  cd "$ROOT"
  cmake --preset xcode
  cmake --build --preset xcode --config Release

  cp -R "$ROOT/Builds/xcode/examples/Demo/Demo_artefacts/Release/Demo.app" "$ROOT/bin"

  cd $ROOT
  zip -r Gin.zip Demo.app
  xcrun notarytool submit --verbose --apple-id "$APPLE_USER" --password "$APPLE_PASS" --team-id "3FS7DJDG38" --wait --timeout 30m Gin.zip

  rm -Rf Demo.app
fi

# Build linux version
if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  cd "$ROOT"
  cmake --preset ninja-gcc
  cmake --build --preset ninja-gcc --config Release
fi

# Build Win version
if [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
  cd "$ROOT"
  cmake --preset vs
  cmake --build --preset vs --config Release
fi

