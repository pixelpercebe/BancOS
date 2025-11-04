#!/bin/bash

LOG_FILE="../build/logs/compile.log"
# Define la ubicación del Makefile
MAKEFILE_PATH="../build/Makefile"

# 1. Añade una marca de tiempo y un separador al log
echo -e "\n==== BUILD INICIADO: $(date) ====\n" >> "$LOG_FILE" 2>&1

# 2. Compilación (stdout y stderr se añaden al final del archivo con >>)
# La salida estándar (STDOUT) se añade a LOG_FILE
# La salida de error (STDERR, canal 2) se redirige a STDOUT (canal 1)
make -f "$MAKEFILE_PATH" >> "$LOG_FILE" 2>&1

# 3. Limpieza (la salida de 'make clean' también se añade al log)
echo -e "\n--- INICIANDO LIMPIEZA ---\n" >> "$LOG_FILE" 2>&1
make -f "$MAKEFILE_PATH" clean >> "$LOG_FILE" 2>&1

echo "Build y limpieza finalizados. -> $LOG_FILE"