#!/bin/bash

pushd Sandbox
# Try to use NVIDIA gpu
#__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia ../build/Sandbox/Sandbox
../build/Sandbox/Sandbox
popd
