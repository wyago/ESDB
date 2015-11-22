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
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_POINTS);

		struct block *it1 = position.head;
		struct block *it2 = velocity.head;
		while (it1 != 0x0 && it2 != 0x0) {
			if (it1->min_value == it2->min_value) {
                int j = 0;
                for (j = 0; j < position.block_size; ++j) {
                    if (it1->contained[j] && it2->contained[j]) {
                        ((struct vec2f*)it1->values)->x += ((struct vec2f*)it2->values)->x;
                        ((struct vec2f*)it1->values)->y += ((struct vec2f*)it2->values)->y;
				        if (pos->x < 1 && pos->x > -1 &&
				            pos->y < 1 && pos->y > -1)
				            glVertex2f(pos->x, pos->y);
                    }
                }


				it1 = it1->next;
				it2 = it2->next;
			} else if (it1->min_value < it2->min_value) {
				it1 = it1->next;
			} else  { // if (key2 < key1) 
				it2 = it2->next;
			}
		}

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


		//glfwSwapBuffers(window);
		//glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
