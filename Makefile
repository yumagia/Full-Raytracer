std=c++14
-fsanitize=address 

image: main.cpp Camera.cpp Math.cpp
	g++ -o main.cpp Camera.cpp Math.cpp -I.