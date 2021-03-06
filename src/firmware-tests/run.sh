#!/bin/bash
GPSIM=gpsim;
BASEDIR="`readlink -e .`";
UTILITIES_BASEDIR="`readlink -e ../utilities`";
TEST_SEPARATOR="================ [ TEST RUN ] ================";

export GPSIM2TUPLE="${UTILITIES_BASEDIR}/gpsim2tuple/gpsim2tuple";
export LPF_RC="${UTILITIES_BASEDIR}/lpf-rc/lpf-rc";
export MEAN="${UTILITIES_BASEDIR}/mean/mean";
export CORDIC_DIR="${UTILITIES_BASEDIR}/trigonometry/cordic";


# If no arguments given then run this script for each directory:

if [ $# != 1 ]; then
	echo > run.log.all;
	exitCode=0;
	for module in [ * ]; do
		if ! [[ -d ${module} ]]; then
			continue;
		fi;

		${0} ${module} |& tee -a run.log.all;
		if [ ${PIPESTATUS[0]} -ne 0 ]; then exitCode=3; fi
	done;
	exit ${exitCode};
fi;


# Run all of the tests for the module given on the command-line:

module="${1}";
runLog="${BASEDIR}/run.log";
tee="tee -a ${runLog}";
echo > ${runLog};
for fixture in `find ${module} -name "*.stc"`; do
	echo ${TEST_SEPARATOR} |& ${tee};
	runTest="${GPSIM} -i -c ${fixture}";
	echo "${runTest};" |& ${tee};
	${runTest} |& ${tee};
done;

for assertions in `find ${module} -name "*TestFixtureAssertions.sh"`; do
	assertionsScript="`basename ${assertions}`";
	assertionsDirectory="`dirname ${assertions}`";
	runAssertions="./${assertionsScript} |& ${tee}";
	echo ${TEST_SEPARATOR} |& ${tee};
	echo "${runAssertions};" |& ${tee};
	pushd .;
	cd ${assertionsDirectory};
	${runAssertions} |& ${tee};
	popd;
done;

passedTestCount=`cat ${runLog} | grep "^\[PASSED\]" | wc -l`;
totalTestCount=`cat ${runLog} | grep "gpsim - the GNUPIC simulator" | wc -l`;
spuriousFailureCount=`cat ${runLog} | grep "ERROR" | wc -l`;
timeoutCount=`cat ${runLog} | grep "cycle break" | wc -l`;
runawayCount=`cat ${runLog} | grep "increment PC=0x1000 == memory size 0x1000" | wc -l`;
unexpectedBreaksCount=$((`cat ${runLog} | pcregrep -M "[^]]\nHit a Breakpoint" | wc -l` / 2));

if [ ${passedTestCount} -eq ${totalTestCount} ]; then
	if [ $((${spuriousFailureCount} + ${timeoutCount})) -eq 0 ]; then
		exitCode=0;
		result="SUCCESS";
	else
		exitCode=2;
		result="FAILURE";
	fi;
else
	exitCode=1;
	result="FAILURE";
fi

echo "------------------------------------------------------------------------------" |& ${tee};
echo "[RESULT: ${result}] ${totalTestCount} tests run for module ${module}, ${passedTestCount} of which passed." |& ${tee};
echo "There were:" |& ${tee};
echo -e "\t${spuriousFailureCount} gpsim errors that didn't trigger breakpoints" |& ${tee};
echo -e "\t${runawayCount} runaway code executions" |& ${tee};
echo -e "\t${timeoutCount} timeouts" |& ${tee};
echo -e "\t${unexpectedBreaksCount} unexpected breakpoints" |& ${tee};

exit ${exitCode};
