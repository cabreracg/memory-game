#include <cstdio>
#include <ctime>
#include <GL/glut.h>

#define MAX 6
#define TIEMPO_T 40

#define BG_COLOR 0.1, 0.1, 0.1
#define BLUE 0.2, 0.4, 1.0
#define GRAY 0.3, 0.3, 0.3
#define GREEN 0.2, 1.0, 0.3
#define LIGHT_BLUE 0.2, 0.7, 1.0
#define RED 0.8, 0.2, 0.2
#define YELLOW 1.0, 0.9, 0.3

enum Estados {
	INICIO,
	MEMORIZANDO,
	JUGANDO,
	TERMINANDO
};

struct vec2 {
	float x, y;
};

struct Fase {
	int filas, tiempo, ul_nivel;
};

Fase fases[] = {
	3, 35, 2,
	4, 45, 7,
	5, 55, 12,
	6, 70, 30
};
int fase_actual = 0;

float window_w, window_h;
float menor_d;
float l_cuadrado = 1.0;
float l_espacio = 0.2;
float fs = 1.0;

int tiempo_para_memorizar;
int tiempo_para_terminar = TIEMPO_T;
int estado = INICIO;
bool pasaste = false;
bool pausa = true;

bool sorteo[MAX][MAX];
bool jugada[MAX][MAX];
float l_tablero;

int nivel = 1;
int aciertos = 0;
int fallos = 0;
int vidas = 3;

void draw_square(bool sorteado, int i, int j) {
	glColor3f(BLUE);
	if (estado != INICIO) {
		switch (estado) {
			case MEMORIZANDO:
				if (sorteado) glColor3f(YELLOW);
				break;
			case TERMINANDO:
				if (pasaste) {
					if (jugada[i][j]) {
						if (sorteado) glColor3f(GREEN);
						else glColor3f(BG_COLOR);
					}
				} else {
					if (jugada[i][j]) {
						if (sorteado) glColor3f(YELLOW);
						else glColor3f(BG_COLOR);
					}
					else if (sorteado) {
						glColor3f(8.0, 0.6, 0.4);
					}
				}
				break;
			default:
				if (jugada[i][j]) {
					if (sorteado) glColor3f(YELLOW);
					else glColor3f(BG_COLOR);
				}
		}
	}
	
	glBegin(GL_QUADS);
		glVertex2f(0.0, 0.0);
		glVertex2f(0.0, l_cuadrado);
		glVertex2f(l_cuadrado, l_cuadrado);
		glVertex2f(l_cuadrado, 0.0);
	glEnd();
}

void draw_heart() {
	glColor3f(LIGHT_BLUE);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0.3, -0.1);
		glVertex2f(0.4, 0.0);
		glVertex2f(0.5, 0.0);
		glVertex2f(0.6, -0.1);
		glVertex2f(0.6, -0.3);
		glVertex2f(0.3, -0.6);
		glVertex2f(0.0, -0.3);
		glVertex2f(0.0, -0.1);
		glVertex2f(0.1, 0.0);
		glVertex2f(0.2, 0.0);
	glEnd();
}

void draw_vidas() {
	for (int i = 0; i < vidas; i++) {
		glPushMatrix();
			glTranslatef(-0.8 * i - 0.6, 0.0, 0.0);
			draw_heart();
		glPopMatrix();
	}
}

void draw_segment(float c) {
	glBegin(GL_QUADS);
		glColor3f(c, c, c);

		glVertex2f(0.0, 0.0);
		glVertex2f(0.1, 0.1);
		glVertex2f(0.5, 0.1);
		glVertex2f(0.6, 0.0);

		glVertex2f(0.0, 0.0);
		glVertex2f(0.6, 0.0);
		glVertex2f(0.5, -0.1);
		glVertex2f(0.1, -0.1);
	glEnd();
}

void draw_display(int n) {
	char *segmentos;
	float c[7];
	switch(n) {
		case 0:
			segmentos = "1111110"; break;
		case 1:
			segmentos = "0110000"; break;
		case 2:
			segmentos = "1101101"; break;
		case 3:
			segmentos = "1111001"; break;
		case 4:
			segmentos = "0110011"; break;
		case 5:
			segmentos = "1011011"; break;
		case 6:
			segmentos = "1011111"; break;
		case 7:
			segmentos = "1110000"; break;
		case 8:
			segmentos = "1111111"; break;
		case 9:
			segmentos = "1111011"; break;
	}
	for (int i = 0; i < 7; i++) {
		if (segmentos[i] == '1') {
			c[i] = 1.0;
		} else {
			c[i] = 0.1;
		}
	}

	//glPushMatrix();
	//	glScalef(0.5, 0.5, 1.0);

		glPushMatrix();
			glTranslatef(0.1, 0.1, 0.0);
			if (c[3] == 1.0) draw_segment(c[3]);
			glTranslatef(0.0, 0.6, 0.0);
			if (c[6] == 1.0) draw_segment(c[6]);
			glTranslatef(0.0, 0.6, 0.0);
			if (c[0] == 1.0) draw_segment(c[0]);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.1, 0.1, 0.0);
			glRotatef(90, 0, 0, 1);
			if (c[4] == 1.0) draw_segment(c[4]);
			glTranslatef(0.6, 0.0, 0.0);
			if (c[5] == 1.0) draw_segment(c[5]);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.7, 0.1, 0.0);
			glRotatef(90, 0, 0, 1);
			if (c[2] == 1.0) draw_segment(c[2]);
			glTranslatef(0.6, 0.0, 0.0);
			if (c[1] == 1.0) draw_segment(c[1]);
		glPopMatrix();
	//glPopMatrix();
}

void verify_lengths() {
	if (menor_d * 0.01 < (1.2 * fases[fase_actual].filas - 0.2) * 1.4) {
		l_cuadrado = (0.25/7.0) * menor_d / (6.0 * fases[fase_actual].filas - 1.0);
		l_espacio = l_cuadrado * 0.2;
		fs = menor_d * 0.01 * (1.0/14.0) * (1.0/5.0) * 7.0 / 0.6;
	} else {
		l_cuadrado = 1.0;
		l_espacio = 0.2;
		fs = 1.0;
	}
}

void nuevo_tablero() {
	if (estado != INICIO)
		estado = MEMORIZANDO;

	if (nivel > fases[fase_actual].ul_nivel) {
		fase_actual++;
		verify_lengths();
	}

	tiempo_para_memorizar = fases[fase_actual].tiempo;
	l_tablero = (l_cuadrado + l_espacio) * fases[fase_actual].filas - l_espacio;

	for (int i = 0; i < fases[fase_actual].filas; i++) {
		for (int j = 0; j < fases[fase_actual].filas; j++) {
			sorteo[i][j] = false;
			jugada[i][j] = false;
		}
	}

	int c = nivel;
	while (c > 0) {
		int i = rand() % fases[fase_actual].filas;
		int j = rand() % fases[fase_actual].filas;
		if (sorteo[i][j] == false) {
			sorteo[i][j] = true;
			c--;
		}
	}
}

void display_callback(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();
		glTranslatef(window_w*0.005 - l_tablero*0.5, window_h*0.005 - l_tablero*0.4, 0.0);
		for (int i = 0; i < fases[fase_actual].filas; i++) {
			for (int j = 0; j < fases[fase_actual].filas; j++) {
				glPushMatrix();
					glTranslatef(j*(l_cuadrado + l_espacio), i*(l_cuadrado + l_espacio), 0.0);
					draw_square(sorteo[i][j], i, j);
				glPopMatrix();
			}
		}
	glPopMatrix();

	if (estado != INICIO) {

	glPushMatrix();
		glTranslatef(window_w*0.005 + l_tablero*0.5, window_h*0.005 - l_tablero*0.5, 0.0);
		glScalef(fs, fs, 1.0);
		draw_vidas();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(window_w*0.005 - l_tablero*0.5, window_h*0.005 - l_tablero*0.5 - 0.6*fs, 0.0);
		glScalef(fs*(3.0/7.0), fs*(3.0/7.0), 1.0);
		if (nivel < 10) {
			draw_display(nivel);
		} else {
			draw_display(nivel / 10);
			glPushMatrix();
				glTranslatef(1.0, 0.0, 0.0);
				draw_display(nivel % 10);
			glPopMatrix();
		}
	glPopMatrix();

	} else {
		glTranslatef(window_w*0.0035, window_h*0.0005, 0);
		glColor3f(LIGHT_BLUE);
		glBegin(GL_QUADS);
			glVertex2f(0.0, 0);
			glVertex2f(0.0, window_h*0.001);
			glVertex2f(window_w*0.003, window_h*0.001);
			glVertex2f(window_w*0.003, 0);
		glEnd();
		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);
			glVertex2f(window_w*0.0012, window_h*0.0002);
			glVertex2f(window_w*0.0012, window_h*0.0008);
			glVertex2f(window_w*0.0021, window_h*0.0005);
		glEnd();
	}

	glutSwapBuffers();
}

void reshape_callback(int w, int h) {
	window_w = w;
	window_h = h;
	glViewport(0, 0, GLsizei(w), GLsizei(h));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, double(w)*0.01, 0.0, double(h)*0.01);
	glMatrixMode(GL_MODELVIEW);

	menor_d = window_w;
	if (h < menor_d) menor_d = window_h;
	verify_lengths();
}

bool detect_collision_point_rectangle(vec2 point, vec2 origen_r, float w_r, float h_r) {
	if (point.x < origen_r.x) return false;
	if (point.y < origen_r.y) return false;
	if (point.x > origen_r.x + w_r) return false;
	if (point.y > origen_r.y + h_r) return false;

	return true;
}

void timer_callback(int data);

void mouse_button_callback(int button, int state, int x, int y) {
	if (estado == JUGANDO) {
		if (button == 0 && state == GLUT_DOWN) {
			vec2 mouse_ndc = {
				float(x)/window_w,
				float(y)/window_h
			};
			vec2 mouse_wc = {
				mouse_ndc.x * window_w * 0.01,
				(1 - mouse_ndc.y) * window_h * 0.01,
			};
			vec2 origen = {window_w*0.005 - l_tablero*0.5, window_h*0.005 - l_tablero*0.4};
			for (int i = 0; i < fases[fase_actual].filas; i++) {
				for (int j = 0; j < fases[fase_actual].filas; j++) {
					vec2 pos = {origen.x + (l_cuadrado + l_espacio)*j, origen.y + (l_cuadrado + l_espacio)*i};
					if (detect_collision_point_rectangle(
								mouse_wc, pos, l_cuadrado, l_cuadrado)) {
						if (!jugada[i][j]) {
							if (sorteo[i][j]) aciertos++;
							else fallos++;
							jugada[i][j] = true;
						}
					}
				}
				if (aciertos == nivel) {
					pasaste = true;
					estado = TERMINANDO;
					tiempo_para_terminar = TIEMPO_T;
					break;
				}
				if (fallos == 3) {
					pasaste = false;
					estado = TERMINANDO;
					tiempo_para_terminar = TIEMPO_T + 15;
					break;
				}
			}
		}
	}
	else if (estado == INICIO) {
		if (button == 0 && state == GLUT_DOWN) {
			vec2 mouse_ndc = {
				float(x)/window_w,
				float(y)/window_h
			};
			vec2 mouse_wc = {
				mouse_ndc.x * window_w * 0.01,
				(1 - mouse_ndc.y) * window_h * 0.01,
			};
			vec2 pos = {window_w*0.0035, window_h*0.0005};
			if (detect_collision_point_rectangle(
				mouse_wc, pos, window_w*0.003, window_h*0.001)) {
				pausa = false;
				estado = MEMORIZANDO;
				glutTimerFunc(0, timer_callback, 1);
			}
		}
	}
}

void timer_callback(int data) {
	if (!pausa) {
		switch (estado) {
			case MEMORIZANDO:
				tiempo_para_memorizar--;
				if (tiempo_para_memorizar == 0) estado = JUGANDO;
				break;
			case TERMINANDO:
				tiempo_para_terminar--;
				if (tiempo_para_terminar == 0) {
					if (pasaste) {
						nivel++;
					} else {
						vidas--;
						if (vidas == 0) {
							nivel = 1;
							vidas = 3;
							fase_actual = 0;
							verify_lengths();
						}
					}
					aciertos = 0;
					fallos = 0;
					nuevo_tablero();
				}
				break;
		}
		glutPostRedisplay();
		glutTimerFunc(17, timer_callback, 1);
	}
}

void keyboard_callback(unsigned char key, int x, int y) {
	switch (key) {
		case 27: // ESC
			exit(0);
		case 32: // Space
			if (estado == INICIO) {
				pausa = false;
				estado = MEMORIZANDO;
				glutTimerFunc(0, timer_callback, 1);
			}
			break;
		/*case 'n':
			if (nivel < 30) {
				nivel++;
				nuevo_tablero();
				glutPostRedisplay();
			}
			break;
		case 'm':
			if (nivel > 1) {
				nivel--;
				nuevo_tablero();
				glutPostRedisplay();
			}
			break;*/
	}
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(680, 680);
	glutCreateWindow("Memory game");

	glClearColor(0.1, 0.1, 0.1, 1);

	glutDisplayFunc(display_callback);
	glutKeyboardFunc(keyboard_callback);
	glutMouseFunc(mouse_button_callback);
	glutReshapeFunc(reshape_callback);
	glutTimerFunc(0, timer_callback, 1);

	srand(time(NULL));
	nuevo_tablero();

	glutMainLoop();

	return 0;
}
