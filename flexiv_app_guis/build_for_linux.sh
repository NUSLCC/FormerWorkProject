#!/bin/bash    

this_script_path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -z "$flexiv_thirdparty2_path" ]; then
    echo "flexiv_thirdparty2_path is not set or empty"
    exit 1
fi

mkdir -p build/Linux/Release && cd build/Linux/Release
cmake ../../.. \
    -DCMAKE_PREFIX_PATH=$flexiv_thirdparty2_path/install/Linux/Release
make -j8
