#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

typedef struct {
	double x, y, z, w;
} TVector4d;

typedef struct {
	int x, y;
} TVector2i;

typedef double TMatrix4d[4][4];

TMatrix4d projection_matrix;
TMatrix4d transform_matrix;
TVector4d *vertices_ptr = NULL;
unsigned int screen_width, screen_height;
char* color_buffer = NULL;

void viewport(unsigned int w, unsigned int h) {
	screen_width = w;
	screen_height = h;
	color_buffer = realloc(color_buffer, w * h);
	if (NULL != color_buffer)
		memset(color_buffer, ' ', w * h);
}

void perspective(double a, double n, double f, double r) {
#define m projection_matrix
	double e = 1.0 / tan(a * 0.5 * 0.0174533);
	memset(m, 0, sizeof(TMatrix4d));
	m[0][0] = e;
			m[1][1] = e / r;
					m[2][2] = -(f + n) / (f - n); m[2][3] = -2 * f * n / (f - n);
					m[3][2] = -1;			      m[3][3] = 0;
#undef m
}

void mul_m4(TMatrix4d a, TMatrix4d b, TMatrix4d c) {
#define cell(i,j) c[i][j] = a[i][0]*b[0][j] + a[i][1]*b[1][j] + a[i][2]*b[2][j] + a[i][3]*b[3][j]
#define row(i) cell(i,0); cell(i,1); cell(i,2); cell(i,3);
	row(0)
	row(1)
	row(2)
	row(3)
}

void mul_v4(TMatrix4d a, TVector4d b, TVector4d *c) {
	c->x = a[0][0] * b.x + a[0][1] * b.y + a[0][2] * b.z + a[0][3] * b.w;
	c->y = a[1][0] * b.x + a[1][1] * b.y + a[1][2] * b.z + a[1][3] * b.w;
	c->z = a[2][0] * b.x + a[2][1] * b.y + a[2][2] * b.z + a[2][3] * b.w;
	c->w = a[3][0] * b.x + a[3][1] * b.y + a[3][2] * b.z + a[3][3] * b.w;

	c->x /= c->w;
	c->y /= c->w;
	c->z /= c->w;
	c->w /= c->w;
}

void identity() {
	memset(transform_matrix, 0, sizeof(TMatrix4d));
	transform_matrix[0][0] = 1;
	transform_matrix[1][1] = 1;
	transform_matrix[2][2] = 1;
	transform_matrix[3][3] = 1;
}

void translate(double x, double y, double z) {
	TMatrix4d m, t;
	memset(m, 0, sizeof(TMatrix4d));
	m[0][0] = 1;						m[0][3] = x;
				m[1][1] = 1;			m[1][3] = y;
							m[2][2] = 1;m[2][3] = z;
										m[3][3] = 1;
	mul_m4(transform_matrix, m, t);
	memcpy(transform_matrix, t, sizeof(TMatrix4d));
}

void scale(double x, double y, double z) {
	TMatrix4d m, t;
	memset(m, 0, sizeof(TMatrix4d));
	m[0][0] = x;
				m[1][1] = y;
							m[2][2] = z;
										m[3][3] = 1;
	mul_m4(transform_matrix, m, t);
	memcpy(transform_matrix, t, sizeof(TMatrix4d));
}

void rotate_z(double a) {
	TMatrix4d m, t;
	double c = cos(a * 0.0174533);
	double s = sin(a * 0.0174533);
	memset(m, 0, sizeof(TMatrix4d));
	m[0][0] = c; m[0][1] = -s;
	m[1][0] = s; m[1][1] = c;
								m[2][2] = 1;
											m[3][3] = 1;
	mul_m4(transform_matrix, m, t);
	memcpy(transform_matrix, t, sizeof(TMatrix4d));
}

void rotate_y(double a) {
	TMatrix4d m, t;
	double c = cos(a * 0.0174533);
	double s = sin(a * 0.0174533);
	memset(m, 0, sizeof(TMatrix4d));

	m[0][0] = c;			   m[0][2] = s;
				  m[1][1] = 1;
	m[2][0] = -s;			   m[2][2] = c;
										    m[3][3] = 1;
	
	mul_m4(transform_matrix, m, t);
	memcpy(transform_matrix, t, sizeof(TMatrix4d));
}

void vertices(TVector4d* v) {
	vertices_ptr = v;
}

double a;

void line(int x0, int y0, int x1, int y1) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;

	for (;;) {
		if (x0 >= 0 && x0 < screen_width && y0 >= 0 && y0 < screen_height)
			color_buffer[(unsigned int)(y0 * screen_width + x0)] = '*';

		if (x0 == x1 && y0 == y1)
			break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}

}

void quadv(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3) {
	TMatrix4d m;
	TVector4d v0, v1, v2, v3; //3d точки после преобразований
	TVector2i s0, s1, s2, s3; //2d точки на плоскости проецирования

	mul_m4(projection_matrix, transform_matrix, m);

	mul_v4(m, vertices_ptr[i0], &v0);
	mul_v4(m, vertices_ptr[i1], &v1);
	mul_v4(m, vertices_ptr[i2], &v2);
	mul_v4(m, vertices_ptr[i3], &v3);

	s0.x = screen_width / 2+ v0.x * screen_width;
	s0.y = screen_height / 2 + v0.y * screen_height;
	s1.x = screen_width / 2 + v1.x * screen_width;
	s1.y = screen_height / 2 + v1.y * screen_height;
	s2.x = screen_width / 2 + v2.x * screen_width;
	s2.y = screen_height / 2 + v2.y * screen_height;
	s3.x = screen_width / 2 + v3.x * screen_width;
	s3.y = screen_height / 2 + v3.y * screen_height;

	line(s0.x, s0.y, s1.x, s1.y);
	line(s1.x, s1.y, s2.x, s2.y);
	line(s2.x, s2.y, s3.x, s3.y);
	line(s3.x, s3.y, s0.x, s0.y);
}

void clrscr() {
	COORD cursorPosition;
	cursorPosition.X = 0;
	cursorPosition.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void render(unsigned int w, unsigned int h) {
	char fmt[10];

	clrscr();
	sprintf(fmt, "%%.%ds\n", w);
	for (size_t i = 0; i < screen_height; i++) {
		printf(fmt, color_buffer + (screen_width * i));
	}
}

void draw(){
	unsigned int w, h;

	w = 120;
	h = 60;

	TVector4d v[] = {
		{-0.5, 0.5, -0.5, 1}, //0
		{-0.5, 0.5, 0.5 ,1}, //1			    1---2
		{0.5, 0.5, 0.5 ,1}, //2				   /|  /|
		{0.5, 0.5, -0.5, 1}, //3			  0-+-3 |
		{-0.5, -0.5, -0.5, 1}, //4			  | 5-+-6
		{-0.5, -0.5, 0.5 ,1}, //5			  |/  |/
		{0.5, -0.5, 0.5 ,1}, //6			  4---7
		{0.5, -0.5, -0.5, 1}, //7
	};

	perspective(90, 0.1, 100, (double)h/w);

	identity();
	scale(2, 1, 2);
	translate(0, 0, 3);
	rotate_y(a);
	rotate_z(a);

	viewport(w, h);

	vertices(v);

	quadv(0, 1, 2, 3);
	quadv(7, 3, 2, 6);
	quadv(0, 3, 7, 4);
	quadv(5, 1, 0, 4);
	quadv(6, 2, 1, 5);
	quadv(4, 7, 6, 5);

	render(w, h);
}

int main(int argc, char** argv) {
	while (1) {
		draw();
		a += 4;
		Sleep(40);
	}
		
	return 0;
}