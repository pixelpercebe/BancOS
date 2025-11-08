#!/bin/bash

LOG_FILE="../build/logs/compile.log"
# Define la ubicación del Makefile
MAKEFILE_PATH="../build/Makefile"

# 1. Añade una marca de tiempo y un separador al log
echo -e "\n==== BUILD INICIADO: $(date) ====\n" | tee -a "$LOG_FILE"

# 2. Compilación (stdout y stderr se muestran en consola Y se guardan en log)
# La salida estándar tambien sale por la consola
# tee -a: añade al archivo sin sobrescribir (-a = append)
# 2>&1: redirige stderr a stdout antes de pasarlo a tee

make -f "$MAKEFILE_PATH" 2>&1 | tee -a "$LOG_FILE"

# 3. Limpieza (la salida de 'make clean' también se muestra y guarda)
echo -e "\n--- INICIANDO LIMPIEZA ---\n" | tee -a "$LOG_FILE"
make -f "$MAKEFILE_PATH" clean 2>&1 | tee -a "$LOG_FILE"

echo "Build y limpieza finalizados. -> $LOG_FILE"