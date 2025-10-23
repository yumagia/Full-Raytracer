std=c++14
-fsanitize=address 

Raytracer: main.cpp Math.cpp Image.cpp
	g++ -o main.cpp Math.cpp Image.cpp -I.