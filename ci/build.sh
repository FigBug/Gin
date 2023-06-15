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
  cp -R "$ROOT/Builds/xcode/examples/Synth/Synth_artefacts/Release/VST3/Synth.vst3" "$ROOT/bin"
  cp -R "$ROOT/Builds/xcode/examples/Synth/Synth_artefacts/Release/AU/Synth.component" "$ROOT/bin"

  cd $ROOT/bin

  if [[ -n "$APPLICATION" ]]; then
    codesign --force -s "$DEV_APP_ID" -v "Demo.app" --deep --strict --options=runtime
    codesign --force -s "$DEV_APP_ID" -v "Synth.vst3" --deep --strict --options=runtime
    codesign --force -s "$DEV_APP_ID" -v "Synth.component" --deep --strict --options=runtime
  fi

  zip -r Gin.zip Demo.app Synth.vst3 Synth.component
  if [[ -n "$APPLE_USER" ]]; then
    xcrun notarytool submit --verbose --apple-id "$APPLE_USER" --password "$APPLE_PASS" --team-id "3FS7DJDG38" --wait --timeout 30m Gin.zip
  fi

  rm -Rf Demo.app Synth.vst3 Synth.component

# Build linux version
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  sudo apt-get update
  sudo apt-get install clang git ninja-build ladspa-sdk libgtk-3-dev freeglut3-dev g++ libasound2-dev libcurl4-openssl-dev libfreetype6-dev libjack-jackd2-dev libx11-dev libxcomposite-dev libxcursor-dev libxinerama-dev libxrandr-dev mesa-common-dev webkit2gtk-4.0 juce-tools xvfb  

  cd "$ROOT"
  cmake --preset ninja-gcc
  cmake --build --preset ninja-gcc --config Release

# Build Win version
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
  cd "$ROOT"
  cmake --preset vs
  cmake --build --preset vs --config Release

  cp -R "$ROOT/Builds/vs/examples/Demo/Demo_artefacts/Release/Demo.exe" $ROOT/bin
  cp -R "$ROOT/Builds/vs/examples/Synth/Synth_artefacts/Release/VST3/Synth.vst3" $ROOT/bin

  cd "$ROOT\bin"
  7z a Gin.zip Demo.exe Synth.vst3

  rm -Rf Demo.exe Synth.vst3
fi

