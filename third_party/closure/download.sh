#!/bin/bash

# Copyright 2014 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Script to assist downloading of the closure compiler from the gclient DEPS
# file. This is handled differently than the other DEPS because the compiler
# requires downloading and unzipping a jar file instead of checking out a
# svn/git repo. This avoids downloading the compiler every time gclient syncs.

if [ $# -ne 1 ]; then
  echo "Usage: $(basename $0) <directory>" >&2
  exit 1
fi

DIR="$1"

# If the version of the compiler is updated, the closure library version in
# bazel/repositories.bzl must also be updated.
# Override with CLOSURE_COMPILER_VERSION when testing newer compiler builds.
VERSION="${CLOSURE_COMPILER_VERSION:-20191111}"
ZIP=compiler-$VERSION.zip
JAR_IN=$DIR/closure-compiler-v$VERSION.jar
JAR=$DIR/compiler.jar
CLOSURE_DOWNLOAD_BASE="${CLOSURE_DOWNLOAD_BASE:-https://dl.google.com/closure-compiler}"
MAVEN_BASE="${MAVEN_BASE:-https://repo1.maven.org/maven2/com/google/javascript/closure-compiler}"

# Download and unzip the compiler if we haven't before or if it is the wrong
# verison.
if [[ ! -e $JAR || -z "$(java -jar $JAR --version | grep $VERSION)" ]]
then
  mkdir -p "$DIR"
  if curl -fL "$CLOSURE_DOWNLOAD_BASE/$ZIP" -o "$DIR/$ZIP"; then
    unzip -o "$DIR/$ZIP" -d "$DIR"
    cp "$JAR_IN" "$JAR"
  else
    curl -fL "$MAVEN_BASE/v$VERSION/closure-compiler-v$VERSION.jar" \
      -o "$JAR"
  fi
fi
