std=c++14
-fsanitize=address 

Raytracer: Raytracer.cpp Math.cpp Image.cpp scene/SceneLoader.cpp
	g++ -fsanitize=address -o Raytracer Raytracer.cpp Math.cpp Image.cpp scene/SceneLoader.cpp -I.