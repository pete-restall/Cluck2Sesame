#!/bin/bash

MPLABX_VERSION="v5.20";
XC8_VERSION="v2.32";

MPLABX_INSTALLER_TAR="MPLABX-${MPLABX_VERSION}-linux-installer.tar";
MPLABX_INSTALLER="MPLABX-${MPLABX_VERSION}-linux-installer.sh";

XC8_INSTALLER="xc8-${XC8_VERSION}-full-install-linux-x64-installer.run";

wget --quiet "https://ww1.microchip.com/downloads/en/DeviceDoc/${MPLABX_INSTALLER_TAR}";
tar -xvf "${MPLABX_INSTALLER_TAR}";
sudo "./${MPLABX_INSTALLER}" -- --mode unattended;

wget --quiet "https://www.microchip.com/content/dam/mchp/documents/DEV/ProductDocuments/SoftwareTools/${XC8_INSTALLER}";
chmod +x "${XC8_INSTALLER}";
sudo "./${XC8_INSTALLER}" --mode unattended --netservername whatever;
