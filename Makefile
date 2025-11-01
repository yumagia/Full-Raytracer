.PHONY: clean

SRC_DIR = src
TARGET_EXE = Raytracer
CFLAGS = -fsanitize=address -O2 


build: $(SRC_DIR)/Raytracer.cpp $(SRC_DIR)/Image.cpp $(SRC_DIR)/scene/SceneLoader.cpp $(SRC_DIR)/Math.cpp
	g++ $(CFLAGS) -o $(TARGET_EXE) $(SRC_DIR)/Raytracer.cpp $(SRC_DIR)/Image.cpp $(SRC_DIR)/scene/SceneLoader.cpp $(SRC_DIR)/Math.cpp -I$(SRC_DIR) $(LDFLAGS)

clean:
	-rm $(TARGET_EXE)
