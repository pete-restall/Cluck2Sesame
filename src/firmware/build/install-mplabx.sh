#!/bin/bash

MPLABX_VERSION="v5.20";
XC8_VERSION="v2.05";

MPLABX_INSTALLER_TAR="MPLABX-${MPLABX_VERSION}-linux-installer.tar";
MPLABX_INSTALLER="MPLABX-${MPLABX_VERSION}-linux-installer.sh";

XC8_INSTALLER="xc8-${XC8_VERSION}-full-install-linux-installer.run";

wget --progress=bar "http://ww1.microchip.com/downloads/en/DeviceDoc/${MPLABX_INSTALLER_TAR}";
tar -xvf "${MPLABX_INSTALLER_TAR}";
sudo "./${MPLABX_INSTALLER}" -- --mode unattended;

wget --progress=bar "http://ww1.microchip.com/downloads/en/DeviceDoc/${XC8_INSTALLER}";
chmod +x "${XC8_INSTALLER}";
sudo "./${XC8_INSTALLER}" --mode unattended --netservername whatever;
