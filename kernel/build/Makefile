# Comandos de ejemplo (comentados):
# gcc src/main.c src/clock.c src/timer.c src/utils.c src/process_generator.c src/sync_resources.c -o bancos -I include/ -pthread -lm

# 1. Compilador
CC = gcc

# 2. Banderas (Flags) de Compilación (Incluye el directorio de cabeceras, que asumo está en kernel/include)
# Ajustamos la ruta -I para que apunte a la carpeta 'include' un nivel arriba.
CFLAGS = -I ../include/ -Wall -g

# 3. Banderas (Flags) del Enlazador
LDFLAGS = -pthread -lm

# 4. Archivos Fuente (.c)
# Apuntamos un nivel arriba a la carpeta 'src'
SRCS = $(wildcard ../src/*.c)

# 5. Archivos Objeto (.o)
# ¡CRUCIAL! Mantenemos los .o junto a los .c para no confundir las dependencias.
# Esto convierte "../src/main.c" en "../src/main.o"
OBJS = $(SRCS:.c=.o)

# 6. Nombre del ejecutable final y carpeta destino
BIN_DIR = ../bin
TARGET = bancos

# 7. Regla Principal
all: $(BIN_DIR)/$(TARGET)

# 8. Regla de Enlazado: Construye el ejecutable final
$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(BIN_DIR)/$(TARGET) $(LDFLAGS)

# 9. Regla de Compilación: Cómo crear un .o a partir de un .c
# Los archivos .o se crean junto a los archivos .c (en ../src/)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 10. Regla de Limpieza
# Borra el ejecutable (en kernel/tool/) y los .o (en ../src/)
clean:
	rm -f $(OBJS)
clean_exec:
	rm -f $(BIN_DIR)/$(TARGET)