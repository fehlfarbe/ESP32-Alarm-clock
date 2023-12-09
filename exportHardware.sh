#!/bin/bash

HARDWARE="./hardware/pcb/"
EXPORT=${HARDWARE}/export/
GERBER=${EXPORT}/gerbers/
DOC="./doc/"
PCB=${HARDWARE}/mp3alarm.kicad_pcb

mkdir -p ${GERBER}
mkdir -p ${EXPORT}

# gerbers and drill files
kicad-cli pcb export gerbers --board-plot-params ${HARDWARE}/mp3alarm.kicad_pcb -o ${GERBER}/
kicad-cli pcb export drill --map-format gerberx2 ${HARDWARE}/mp3alarm.kicad_pcb -o ${GERBER}/

# BOM
kicad-cli sch export python-bom ${HARDWARE}/mp3alarm.kicad_sch -o ${EXPORT}/BOM.xml

# 3d step model
kicad-cli pcb export step --subst-models  ${PCB} -o ${EXPORT}/pcb_model.step

# images for doc
kicad-cli sch export svg ${HARDWARE}/mp3alarm.kicad_sch -o ${DOC}
kicad-cli sch export pdf ${HARDWARE}/mp3alarm.kicad_sch -o ${DOC}/pcb_schema.pdf
pcbdraw plot --side front ${PCB} ${DOC}/pcb_front.png
pcbdraw plot --side back ${PCB} ${DOC}/pcb_back.png
pcbdraw render --side front --renderer raytrace ${PCB} ${DOC}/pcb_render_front.png
pcbdraw render --side back --renderer raytrace ${PCB} ${DOC}/pcb_render_back.png