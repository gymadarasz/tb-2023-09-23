rm ./build/main
g++ -c src/main.cpp -o build/main.o
g++ -c src/Graphics.cpp -o build/Graphics.o
g++ -o build/main build/main.o build/Graphics.o -lX11
./build/main
