#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "block_list.h"

struct vec2f {
	float x, y;
};

void move(struct vec2f **items) {
    struct vec2f *pos = items[0];
    struct vec2f *vel = items[1];
    pos->x += vel->x;
    pos->y += vel->y;
}

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
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_POINTS);

        block_act((void (*)(void **))move, 2, position, velocity);

		glEnd();

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


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
