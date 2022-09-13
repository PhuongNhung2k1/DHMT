﻿//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint model_view_loc;
GLuint projection_loc;
mat4 model_view;
mat4 instance, instance_cua;
GLdouble theta[] = { 0, 0,0 };
GLdouble anpha, xx = 0, yy = 0, zz = 0;
mat4 ctm; // curren : hinh anh hien thi ban dau ra man hinh
mat4 view_cua, lookat;
GLdouble l = -1.0, r = 0.6;
GLdouble bottom = -1.0, top = 0.5;
GLdouble zNear = 0.6, zFar = 3.0;
void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors); 
	
}


void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	model_view_loc = glGetUniformLocation(program, "model_view");
	projection_loc = glGetUniformLocation(program, "projection");
	glEnable(GL_DEPTH_TEST);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}
// ve canh cua
// khai bao tham so de phong to hinh anh so voi hinh anh ban dau
GLfloat wtd = 0.48, htd = 0.04, wl = 0.04;
GLfloat wtp = 0.04, htp = 0.6;
GLfloat wc = 0.4, hc = 0.6,wlc = 0.02;

void khungcua() {
	// tren
	instance = Translate(0,0.02,0)* Scale(wtd,htd,wl);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, view_cua *instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// duoi
	instance = Translate(0, -0.62, 0) * Scale(wtd, htd, wl);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, view_cua * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// trai
	instance = Translate(-0.22,-0.3,0) * Scale(wtp, htp, wl);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, view_cua *instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// phai
	instance = Translate(0.22, -0.3, 0) * Scale(wtp, htp, wl);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, view_cua * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
// mawt cua
void canhcua() {
	instance = Scale(wc,hc,wlc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, view_cua * instance_cua * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// ve ca khung cua
void vecua() {
	// thuc hien truoc trong code thi viet truoc
	view_cua = lookat * Translate(xx, 0, 0);

	instance_cua = RotateX(anpha)* Translate(0, -0.3, 0);
	canhcua();
	khungcua();
}
void display( void )
{
	point4 eye(-0.5, -0.5,1.0, 1.0);
	point4 at(1.0,1.0,0.5,1.0);
	vec4 up(0.0,1.0,0.0,1.0);
	lookat = LookAt(eye, at, up);
	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );    
	// tao hien thi ban dau ra man hinh , nghieng bao nhieu==> nhan voi tatca co instance
	//ctm = RotateX(-30) * RotateY(30);
	// dich chuyen cua sang trai , phai, ==> them thiet lap goc cam cho no
	vecua();
	glutSwapBuffers();									   
}

void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
	case 'm':
		anpha += 5;
		if (anpha >= 60) anpha -= 60;
		glutPostRedisplay();
		break;
	case 'd':
		anpha -= 5;
		if (anpha < 0) anpha += 60;
		glutPostRedisplay();
		break;
	case 'l':
		xx -= 0.1 * 0.48;
		glutPostRedisplay();
		break;
	case 'r':
		xx += 0.1*0.48;
		glutPostRedisplay();
		break;
    }
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a Cube" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  

	glutMainLoop();
    return 0;
}
