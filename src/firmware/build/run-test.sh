#!/bin/bash

if [ $# -ne 5 ]; then
	echo "Usage: $0 <mdb> <device> <elf> <fixtureSearchRoot> <fixtureFilename>";
	exit 1;
fi;

MDB="${1}";
DEVICE="${2}";
TEST_FILENAME="${3}";
PARTIAL_FIXTURE_SEARCHDIR="${4}";
PARTIAL_FIXTURE_FILENAME="${5}";

PARTIAL_FIXTURE_PATH_AND_FILENAME="$(find ${PARTIAL_FIXTURE_SEARCHDIR} -name ${PARTIAL_FIXTURE_FILENAME} -exec realpath -e \{\} \; | head -n 1)";
PARTIAL_FIXTURE_PATH="$(dirname ${PARTIAL_FIXTURE_PATH_AND_FILENAME})";

FIXTURE_FILENAME="$(mktemp /tmp/ceedling-test-fixture.XXXXXXXX)";

RNG_SEED=${RANDOM};
RNG_SEED_HIGH=$(((${RNG_SEED} >> 8) & 0xff));
RNG_SEED_LOW=$((${RNG_SEED} & 0xff));

cat << EOF > ${FIXTURE_FILENAME}
device ${DEVICE}
hwtool SIM
# set codecoverage.enabled Enabled_Reset_on_POR
set oscillator.frequency 32
set oscillator.frequencyunit Mega
set uart1io.uartioenabled true
set uart2io.uartioenabled true
set uart2io.output window
program "${TEST_FILENAME}"
break unityBeforeRunHook
break unityBreakpointHook

reset
run
wait

write rngSeed ${RNG_SEED_LOW} ${RNG_SEED_HIGH}

$(cat ${PARTIAL_FIXTURE_PATH_AND_FILENAME} | sed s/\$\{cwd\}/$(echo ${PARTIAL_FIXTURE_PATH} | sed 's/\//\\\//g')/g)

wait 10000
quit
EOF

${MDB} ${FIXTURE_FILENAME}