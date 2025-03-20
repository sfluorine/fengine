#!/usr/bin/bash

set -e

mkdir -p out
cmake -B out -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_BUILD_TYPE=Release

cd out
ninja
cd ..

cp ./out/compile_commands.json .

./out/game
