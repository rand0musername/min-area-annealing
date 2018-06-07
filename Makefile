default: main

obj/%.o: src/%.cc
	g++ -c -std=c++14 -O3 -o $@ $<

main: obj/main.o obj/geometry.o obj/window.o
	g++ $^ -o $@ -lsfml-graphics -lsfml-window -lsfml-system