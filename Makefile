CC = gcc  # Compilador a usar
CFLAGS = -Wall -Wextra -g # Flags: advertencias y debug
TARGET = psinfo  # Nombre del ejecutable final
SRCDIR = src  # Directorio de código fuente

# Lista de archivos fuente y objetos
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/process_info.c
OBJECTS = $(SOURCES:.c=.o)

# Regla principal: compila el ejecutable
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar cada .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Limpia archivos generados
clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean # Define objetivos "falsos"