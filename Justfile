release-build:
    rm -rf build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build

debug-build:
    rm -rf build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build

sample-debug-build:
    make clean -C examples/echoback
    make BUILD=debug -C examples/echoback

sample-release-build:
    make clean -C examples/echoback
    make BUILD=release -C examples/echoback

fmt:
    clang-format -i $(find ./src -name '*.[ch]')

