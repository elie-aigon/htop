# Nom de l'exécutable
TARGET = htop

# Dossier de sortie
OUTDIR = out

# Compiler et flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Source files
SRCS = $(wildcard src/*.c)

# Commande pour compiler et lier en une seule étape
$(TARGET):
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUTDIR)/$(TARGET) -lncurses

# Clean
clean:
	rm -rf $(OUTDIR)
