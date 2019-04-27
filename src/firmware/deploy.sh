#!/bin/bash

THIS_DIR="$(dirname "$(readlink -f "$0")")";
LOGS_DIR="${THIS_DIR}/build/deployment";
MPLABX_DIR="/opt/microchip/mplabx/v5.05/mplab_platform";
PICKIT3_IPE="${MPLABX_DIR}/mplab_ipe/ipecmd.sh -TPPK3";
HEX_FILENAME="${THIS_DIR}/build/ceedling/generated/release/Cluck3Sesame.hex";

pushd .;
cd "${LOGS_DIR}";
echo "*** DEPLOYMENT LOGS CAN BE FOUND IN ${LOGS_DIR}";
${PICKIT3_IPE} -P16F15355 -OL -M -Y -F${HEX_FILENAME};
popd;
