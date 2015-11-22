gcc -Wall -std=c99 -c src/main.c -o obj/main.o -I ~/dev/c/lib/glfw-3.1.2/include/
gcc -Wall -std=c99 -c src/block_list.c -o obj/block_list.o -I ~/dev/c/lib/glfw-3.1.2/include/
gcc obj/* -o bin/esdb -lglfw3 -lX11 -lXrandr -lXi -lXxf86vm -lGL -ldl -lpthread -lm -lXinerama -lXcursor
