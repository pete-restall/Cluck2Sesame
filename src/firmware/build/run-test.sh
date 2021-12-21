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

PARTIAL_FIXTURE_PATH_AND_FILENAME="$(find ${PARTIAL_FIXTURE_SEARCHDIR} -name ${PARTIAL_FIXTURE_FILENAME} -exec readlink -f \{\} \; | head -n 1)";
PARTIAL_FIXTURE_PATH="$(dirname ${PARTIAL_FIXTURE_PATH_AND_FILENAME})";

FIXTURE_FILENAME="$(mktemp /tmp/ceedling-test-fixture.XXXXXXXX)";

RNG_SEED=${RANDOM};
RNG_SEED_HIGH=$(((${RNG_SEED} >> 8) & 0xff));
RNG_SEED_LOW=$((${RNG_SEED} & 0xff));

cat << EOF > ${FIXTURE_FILENAME}
device ${DEVICE}
hwtool SIM
set breakoptions.coreerrors Break
set breakoptions.corewarnings Break
set breakoptions.peripheralerrors Break
set breakoptions.peripheralwarnings Break
set breakoptions.stimulusmessags.errors Break
set breakoptions.stimulusmessags.warnings Break

set oscillator.auxfrequency 32
set oscillator.auxfrequencyunit Mega
set oscillator.rcfrequency 32
set oscillator.rcfrequencyunit Mega
set oscillator.frequency 8
set oscillator.frequencyunit Mega

set uart2io.uartioenabled true
set uart2io.output window

set periphADC1.altscl true
set periphADC1.minTacq 5
set periphADC1.tacqunits microseconds

program "${TEST_FILENAME}"
break unityBeforeRunHook
break unityBreakpointHook
break fpbase

reset
write 0x0420 ${RNG_SEED_LOW} ${RNG_SEED_HIGH}
run
wait

$(cat ${PARTIAL_FIXTURE_PATH_AND_FILENAME} | sed s/\$\{cwd\}/$(echo ${PARTIAL_FIXTURE_PATH} | sed 's/\//\\\//g')/g)

wait 30000
quit
EOF

${MDB} ${FIXTURE_FILENAME}
