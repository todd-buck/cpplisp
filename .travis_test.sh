#!/bin/bash

set -ex

ACTION=${1}
IMPL=${2}

# Environment variable configuration
BUILD_IMPL=${BUILD_IMPL:-${IMPL}}
TEST_OPTS="${TEST_OPTS} --debug-file ../../${ACTION}.err"

if [ "${DO_SELF_HOST}" ]; then
    MAL_IMPL=${IMPL}
    IMPL=mal
fi
if [ "${DO_HARD}" ]; then
    TEST_OPTS="${TEST_OPTS} --hard"
fi

echo "ACTION: ${ACTION}"
echo "IMPL: ${IMPL}"
echo "BUILD_IMPL: ${BUILD_IMPL}"
echo "MAL_IMPL: ${MAL_IMPL}"

if [ "${NO_PERF}" -a "${ACTION}" = "perf" ]; then
    echo "Skipping perf test"
    exit 0
fi
if [ "${NO_SELF_HOST}" -a "${DO_SELF_HOST}" ]; then
    echo "Skipping ${ACTION} of ${MAL_IMPL} self-host"
    exit 0
fi
if [ "${NO_SELF_HOST_PERF}" -a "${DO_SELF_HOST}" -a "${ACTION}" = "perf" ]; then
    echo "Skipping only perf test for ${MAL_IMPL} self-host"
    exit 0
fi

raw_mode_var=${MAL_IMPL:-${IMPL}}_MODE
mode_var=${raw_mode_var/./__}
mode_val=${!mode_var}

MAKE="make ${mode_val:+${mode_var}=${mode_val}}"

# If NO_DOCKER is blank then launch use a docker image, otherwise use
# the Travis image/tools directly.
if [ -z "${NO_DOCKER}" ]; then
    img_impl=$(echo "${MAL_IMPL:-${IMPL}}" | tr '[:upper:]' '[:lower:]')
    # We could just use make DOCKERIZE=1 instead but that does add
    # non-trivial startup overhead for each step.
    MAKE="docker run -it -u $(id -u) -v `pwd`:/mal kanaka/mal-test-${img_impl%%-mal} ${MAKE}"
fi

esac
