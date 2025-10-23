std=c++14
-fsanitize=address 

Raytracer: main.cpp Math.cpp Image.cpp scene/SceneLoader.cpp
	g++ -o main.cpp Math.cpp Image.cpp scene/SceneLoader.cpp -I.