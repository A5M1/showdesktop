CC = clang
LIB = llvm-lib
RC = windres
LIBS = -luser32 -lshell32 -lole32

OBJ_DIR = objs
DIST_DIR = dist

all: $(DIST_DIR)/ShowDesktop.exe

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

$(OBJ_DIR)/installer.obj: installer.c installer.h | $(OBJ_DIR)
	$(CC) -c installer.c -o $(OBJ_DIR)/installer.obj

$(OBJ_DIR)/installer.lib: $(OBJ_DIR)/installer.obj | $(OBJ_DIR)
	$(LIB) /out:$(OBJ_DIR)/installer.lib $(OBJ_DIR)/installer.obj

$(OBJ_DIR)/res.res: res.rc | $(OBJ_DIR)
	$(RC) -i res.rc -o $(OBJ_DIR)/res.res -O coff

$(DIST_DIR)/ShowDesktop.exe: main.c $(OBJ_DIR)/installer.lib $(OBJ_DIR)/res.res | $(DIST_DIR)
	$(CC) main.c $(OBJ_DIR)/installer.lib $(OBJ_DIR)/res.res -o $(DIST_DIR)/ShowDesktop.exe $(LIBS)

clean:
	rm -rf $(OBJ_DIR) $(DIST_DIR)
