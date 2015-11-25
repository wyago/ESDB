#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "esdb.h"
#include "block_list.h"

struct vec2f {
	float x, y;
};

void move(struct esdb *db, struct vec2f **items) {
    struct vec2f *pos = items[0];
    struct vec2f *vel = items[1];
    pos->x += vel->x;
    pos->y += vel->y;

    float r = rand() / (float)RAND_MAX * 0.02f;

    vel->x = vel->x * cos(r) - vel->y * sin(r);
    vel->y = vel->x * sin(r) + vel->y * cos(r);

    if (pos->x < 1 && pos->x > -1 &&
        pos->y < 1 && pos->y > -1)
    glVertex2f(pos->x, pos->y);
}

int main(void) {
	GLFWwindow* window;

	srand(12);

	if (!glfwInit()) { return -1; }

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    struct esdb *db = make_esdb(1, 1024);
    int positions = register_component(db, sizeof(struct vec2f));
    int velocities = register_component(db, sizeof(struct vec2f));

	float lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_POINTS);
        
        int components[2] = { positions, velocities };
        esdb_act(db, (void (*)(struct esdb*, void **))move, 2, components);

		glEnd();

		if (!glfwGetKey(window, 'S')) {
            int i;
            for (i = 0; i < 10; ++i) {
                struct vec2f *newPosition = malloc(sizeof(struct vec2f));
                struct vec2f *newVelocity = malloc(sizeof(struct vec2f));
                int *ids = malloc(2 * sizeof(int));
                void **data = malloc(2 * sizeof(void*));
                ids[0] = positions;
                ids[1] = velocities;
                
                newPosition->x = 0;
                newPosition->y = 0;
                newVelocity->x = (rand()%1000)/100000.0f - 1000/100000.0f*0.5f;
                newVelocity->y = (rand()%1000)/100000.0f - 1000/100000.0f*0.5f;
                
                data[0] = newPosition;
                data[1] = newVelocity;
                queue_entity(db, 2, ids, data);
            }
		}

		float time = glfwGetTime();
		float delta = time - lastTime;
		lastTime = time;
		printf("%f\n", 1.0f/delta);

		glfwSwapBuffers(window);
		glfwPollEvents();
		flush_queues(db);
	}

	glfwTerminate();
	return 0;
}
