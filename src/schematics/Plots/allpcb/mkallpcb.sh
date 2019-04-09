#!/bin/sh

PROJECT="Cluck3Sesame";
PLOT_REPORT="../${PROJECT} (Plot Report).txt";
PLOT_DIR="../";
ZIP_DIR="${PROJECT}";
ZIP_FILE="${PROJECT}.zip";

if [[ -d ${ZIP_DIR} ]]; then
	rm ${ZIP_DIR}/*;
else
	mkdir ${ZIP_DIR};
fi;

if [[ -a ${ZIP_FILE} ]]; then
	rm ${ZIP_FILE};
fi;

cp "${PLOT_DIR}/${PROJECT} - Board Outline.gbr" "${ZIP_DIR}/Board Outline.GBL";
cp "${PLOT_DIR}/${PROJECT} - Bottom Copper.gbr" "${ZIP_DIR}/Bottom Layer.GBL";
cp "${PLOT_DIR}/${PROJECT} - Bottom Solder Mask.gbr" "${ZIP_DIR}/Bottom Solder Mask.GBS";
cp "${PLOT_DIR}/${PROJECT} - Bottom Paste Mask.gbr" "${ZIP_DIR}/Bottom Paste Mask.GBP";
cp "${PLOT_DIR}/${PROJECT} - Bottom Silkscreen.gbr" "${ZIP_DIR}/Bottom Overlay.GBO";
cp "${PLOT_DIR}/${PROJECT} - Drill Data - [Through Hole].drl" "${ZIP_DIR}/Drill Drawing.NC";
cp "${PLOT_DIR}/${PROJECT} - Drill Data - [Through Hole] (Unplated).drl" "${ZIP_DIR}/Drill Drawing Unplated.NC";
cp "${PLOT_DIR}/${PROJECT} - Top Copper.gbr" "${ZIP_DIR}/Top Layer.GTL";
cp "${PLOT_DIR}/${PROJECT} - Top Solder Mask.gbr" "${ZIP_DIR}/Top Solder Mask.GTS";
cp "${PLOT_DIR}/${PROJECT} - Top Paste Mask.gbr" "${ZIP_DIR}/Top Paste Mask.GTP";
cp "${PLOT_DIR}/${PROJECT} - Top Silkscreen.gbr" "${ZIP_DIR}/Top Overlay.GTO";
cp "README.txt" "${ZIP_DIR}/README.TXT";
cp "${PLOT_REPORT}" "${ZIP_DIR}/DesignSpark Plot Report.TXT";

zip -9 -r "${ZIP_FILE}" "${ZIP_DIR}";
