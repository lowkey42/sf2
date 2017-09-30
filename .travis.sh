#!/bin/sh
#
# Build script for travis-ci.org builds to handle compiles and static
# analysis when ANALYZE=true.
#

if [ $ANALYZE = "true" ]; then
    if [ "$CC" = "clang" ]; then
        docker exec build scan-build cmake -G "Unix Makefiles" -H/repo -B/build
        docker exec build scan-build -enable-checker core.AdjustedReturnValue \
          -enable-checker core.AttributeNonNull \
          -enable-checker deadcode.DeadStores \
          -enable-checker security.insecureAPI.UncheckedReturn \
          --status-bugs -v \
          cmake --build /build
    else
        docker exec build cppcheck --template "{file}({line}): {severity} ({id}): {message}" \
            --enable=style --force --std=c++17 -j 8 \
            -I headers hc common headers unix win32 /repo 2> cppcheck.txt
        if [ -s cppcheck.txt ]; then
            cat cppcheck.txt
            exit 1
        fi
    fi
else
  docker exec build cmake -DSF2_BUILD_TESTS=ON -H/repo -B/build
  docker exec build cmake --build /build
  docker exec build cmake --build /build --target test
fi

