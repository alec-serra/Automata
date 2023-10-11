//Build with cc glut.c -o program -lGL -lGLU -lglut
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <stdbool.h>


void print_lattice();

/////// LATTICE INFO ///////
// 4 16 64 256 1024
#define LATTICE_WIDTH 100 //1024
#define LATTICE_HEIGHT 100 //1024

#define ALIVE_RED 1.0
#define ALIVE_GREEN 1.0
#define ALIVE_BLUE 1.0

#define DEAD_RED 0.0
#define DEAD_GREEN 0.0
#define DEAD_BLUE 0.0


int* lattice;
int* next_lattice;

/////// LATTICE INFO ///////



/////// WINDOW SETUP ///////

GLint WINDOW_WIDTH = 1000;
GLint WINDOW_HEIGHT = 1000;   /* window width and height */
int Window_Handle;

GLfloat seconds_per_frame = 1.0/10000.0;
int iterations_per_frame = 10;

#define BILLION  1000000000L;

GLfloat block_width = -1.0;
GLfloat block_height = -1.0;

/////// WINDOW SETUP ///////


/////// STATE SETUP ///////

bool First_Run = true;

/////// STATE SETUP ///////

bool PAUSE = true;

void display()
{
	
}


/* Called when window is resized,
   also when window is first created,
   before the first call to display(). */
void reshape(int w, int h)
{
  /* save new screen dimensions */
  WINDOW_WIDTH = w;
  WINDOW_HEIGHT = h;

  /* tell OpenGL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) WINDOW_WIDTH, (GLsizei) WINDOW_HEIGHT);

  /* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT, -1.f, 1.f);

  block_width = ((GLfloat)WINDOW_WIDTH)/((GLfloat)LATTICE_WIDTH);
  block_height = ((GLfloat)WINDOW_HEIGHT)/((GLfloat)LATTICE_HEIGHT);

  return;
}

void mouse_event(int button, int state,
				 int x, int y)
{
	
}

void key_event(unsigned char key, int x, int y)
{
  switch((char)key) {
  case 's':
  case 'S':
	  PAUSE = !PAUSE;
	  break;
  case 'q':
  case 27:    /* ESC */
    glutDestroyWindow(Window_Handle);
    exit(0);
  default:
    break;
  }

  return;
}

int lattice_access(int n1, int n2)
{
	
	switch(n1)
	{
	case -1:
		n1 = LATTICE_WIDTH-1;
		break;
	case LATTICE_WIDTH:
		n1 = 0;
	}
	switch(n2)
	{
	case -1:
		n2 = LATTICE_HEIGHT-1;
		break;
	case LATTICE_HEIGHT:
		n2 = 0;
	}
	
	return ((n1*LATTICE_WIDTH) + n2);
}

int count_neighbors(int ndx, int ndx2)
{
	int count = 0;

	count += lattice[lattice_access(ndx-1, ndx2-1)];
	count += lattice[lattice_access(ndx-1, ndx2)];
	count += lattice[lattice_access(ndx-1, ndx2+1)];

	count += lattice[lattice_access(ndx, ndx2-1)];
	count += lattice[lattice_access(ndx, ndx2+1)];

	count += lattice[lattice_access(ndx+1, ndx2-1)];
	count += lattice[lattice_access(ndx+1, ndx2)];
	count += lattice[lattice_access(ndx+1, ndx2+1)];	

	return count;
}

void paint_cell(int ndx, int ndx2, bool next_state)
{
	if(next_state)
	{
		glColor3f(ALIVE_RED, ALIVE_GREEN, ALIVE_BLUE);
	}
	else
	{
		glColor3f(DEAD_RED, DEAD_GREEN, DEAD_BLUE);
	}
	

	glBegin(GL_QUAD_STRIP);
	glVertex2f(ndx*block_width, (ndx2)*block_height);
	glVertex2f(ndx*block_width, (ndx2+1)*block_height);
	glVertex2f((ndx+1)*block_width, (ndx2)*block_height);
	glVertex2f((ndx+1)*block_width, (ndx2+1)*block_height);
	glEnd();
}

void iterate_sim(bool capture_iteration)
{
	//Simulation Iteration
	int neighbor_count;
	bool alive;
	int current_ndx = -1;
	
	//Update and draw the next generation
	for(int ndx = 0; ndx < LATTICE_WIDTH; ndx++)
	{
		neighbor_count = 0;
		
		for(int ndx2 = 0; ndx2 < LATTICE_HEIGHT; ndx2++)
		{
			//printf("ndx: %d, ndx2: %d, lattice: %d \n", ndx, ndx2, lattice[access(ndx, ndx2)]);
			current_ndx = lattice_access(ndx, ndx2);

			
			alive = lattice[current_ndx];
			neighbor_count = count_neighbors(ndx, ndx2);

			if(neighbor_count > 8)
			{
				  perror("Invalid neighbor count \n");
				  exit( EXIT_FAILURE );
			}
			bool next_state = (alive && (neighbor_count == 2 || neighbor_count == 3))
				|| (!alive && neighbor_count == 3);
			next_lattice[current_ndx] = next_state;			
			if(capture_iteration)
			{
				paint_cell(ndx, ndx2, next_state);
			}
		}
	}
	lattice = next_lattice;

}

void idle_display()
{
	for(int ndx = 0; ndx < LATTICE_WIDTH; ndx++)
	{
		for(int ndx2 = 0; ndx2 < LATTICE_HEIGHT; ndx2++)
		{
			paint_cell(ndx, ndx2, lattice[ndx*LATTICE_WIDTH + ndx2]);
		}
	}
}
void iterate(bool capture_iteration)
{

	
	//Timing info for framerate/
	struct timespec start, stop;
    double iteration_time, delta;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
	if(! PAUSE)
		iterate_sim(capture_iteration);
	else
		idle_display();

	
	//Ensure framerate is const.
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }

    delta = ( stop.tv_sec - start.tv_sec )
		+ ( stop.tv_nsec - start.tv_nsec )/ BILLION;;

	double wait_time = seconds_per_frame - delta;

	//printf("%f \n", wait_time);
	
	if(wait_time > 0)
	{
		struct timespec wait_start, wait_end;
		wait_start.tv_sec = 0;
		wait_start.tv_nsec = wait_time*BILLION;

		
		nanosleep(&wait_start, &wait_end);
	}
	else
	{
		printf("Frame Lagged not poggers \n");
		//@TODO: Logging
	}
		
}


void print_lattice()
{
	printf("\n");
	for(int ndx = 0; ndx < LATTICE_WIDTH; ndx++)
	{
		for(int ndx2 = 0; ndx2 < LATTICE_HEIGHT; ndx2++)
		{
			printf(" %d ", lattice[ndx*LATTICE_WIDTH + ndx2]);
		}
		printf("\n");
	}
	printf("\n");
}

void setup_lattice()
{
	//Seed rand
	srand(69420);	
	lattice = calloc(LATTICE_WIDTH*LATTICE_HEIGHT, sizeof(int));
	next_lattice = calloc(LATTICE_WIDTH*LATTICE_HEIGHT, sizeof(int));

	//print_lattice();
	
	for(int ndx = 0; ndx < LATTICE_WIDTH; ndx++)
	{
		for(int ndx2 = 0; ndx2 < LATTICE_HEIGHT; ndx2++)
		{
			lattice[ndx*LATTICE_WIDTH + ndx2] = (ndx*LATTICE_WIDTH-ndx2) %2;
			paint_cell(ndx, ndx2, lattice[ndx*LATTICE_WIDTH + ndx2]);
		}
	}
}

void main_loop()
{
	if(First_Run)
	{
		First_Run = false;
		printf("No more init \n");

		block_width = ((GLfloat)WINDOW_WIDTH)/((GLfloat)LATTICE_WIDTH);
		
		block_height = ((GLfloat)WINDOW_HEIGHT)/((GLfloat)LATTICE_HEIGHT);

		setup_lattice();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(int iteration_ndx = 0; iteration_ndx < iterations_per_frame; iteration_ndx++)
	{
		iterate(iteration_ndx == iterations_per_frame-1);
	}
	glFlush();

	
	
	
}

int main(int argc, char* argv[])
{   
	
	glutInit(&argc, argv);

	/* specify the display to be single 
     buffered and color as RGBA values */
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize((int) WINDOW_WIDTH, (int) WINDOW_HEIGHT);

	Window_Handle = glutCreateWindow("Automata"); //Title
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key_event);
	glutMouseFunc(mouse_event);
	glutDisplayFunc(display);
	
	glutIdleFunc(main_loop);
	
	/* start the GLUT main loop */
	glutMainLoop();

	exit(0);
}

