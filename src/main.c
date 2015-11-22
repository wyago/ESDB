#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "block_list.h"

struct vec2f {
	float x, y;
};

int main(void) {
	struct block_list position = make_block_list(1024, sizeof(struct vec2f));
	struct block_list velocity = make_block_list(1024, sizeof(struct vec2f));
	GLFWwindow* window;

	srand(12);

	if (!glfwInit()) { return -1; }

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	long int newKeys[1000];
	struct vec2f newPositions[1000];
	struct vec2f newVelocities[1000];
	int i;
	for (i = 0; i < 1000; ++i) {
		newPositions[i].x = 0;
		newPositions[i].y = 0;
	}

	long int nextKey = 1;

	float lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		//glClear(GL_COLOR_BUFFER_BIT);
		//glBegin(GL_POINTS);

		struct block_list_iterator it1 = iterate(position);
		struct block_list_iterator it2 = iterate(velocity);
		while (available(it1) && available(it2)) {
			long int key1 = current_key(it1);
			long int key2 = current_key(it2);

			if (key1 == key2) { // key1 == key2
				struct vec2f *pos = (struct vec2f*)current(it1);
				struct vec2f *vel = (struct vec2f*)current(it2);

				pos->x += vel->x;
				pos->y += vel->y;

				//if (pos->x < 1 && pos->x > -1 &&
				//	pos->y < 1 && pos->y > -1)
				//glVertex2f(pos->x, pos->y);

				it1 = next(it1);
				it2 = next(it2);
			} else if (key1 < key2) {
				it1 = next(it1);
			} else  { // if (key2 < key1) 
				it2 = next(it2);
			}
		}

		//glEnd();

		if (!glfwGetKey(window, 'S')) {
			for (i = 0; i < 1000; ++i) {
				newVelocities[i].x = (rand() % 100 - 50) * 0.001f;
				newVelocities[i].y = (rand() % 100 - 50) * 0.001f;
				newKeys[i] = nextKey + i;
			}
			nextKey += 1000;
			block_insert(position, newKeys, newPositions, 1000);
			block_insert(velocity, newKeys, newVelocities, 1000);
		}

		printf("%d\t", nextKey);

		float time = glfwGetTime();
		float delta = time - lastTime;
		lastTime = time;
		printf("%f\n", delta);


		//glfwSwapBuffers(window);
		//glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
