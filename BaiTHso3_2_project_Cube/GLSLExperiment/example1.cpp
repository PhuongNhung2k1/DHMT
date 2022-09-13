//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

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


GLuint model_loc;
mat4 instance;
mat4 model;
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

	model_loc = glGetUniformLocation(program, "model");
	glEnable(GL_DEPTH_TEST);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}

mat4 instance_dieuhoa, ctm, mo_dh;
GLdouble anpha, dh;
//ve dieu hoa
void dieuhoa(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm*instance_dieuhoa*instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void veDieuHoa() {
	//tren
	instance_dieuhoa = Translate(0, -0.15, 0);
	dieuhoa(0.64,0.04,0.3);
	//duoi
	instance_dieuhoa = Translate(0, 0.15, 0);
	dieuhoa(0.64, 0.04,0.3);
	// trai
	instance_dieuhoa = Translate(-0.3, 0, 0);
	dieuhoa(0.04, 0.3, 0.3);
	// phai
	instance_dieuhoa = Translate(0.3, 0, 0);
	dieuhoa(0.04, 0.3, 0.3);
	// sau
	instance_dieuhoa = Translate(0,0,0.15);
	dieuhoa(0.6,0.34,0.04);
	// giua
	instance_dieuhoa = RotateX(-20);
	dieuhoa(0.55,0.03,0.29);
	// truoc tren
	instance_dieuhoa = Translate(0,0.09,-0.16);
	dieuhoa(0.64,0.18,0.03);
	// duoi
	mo_dh = Translate(0, -0.15, -0.16) * RotateX(dh) * Translate(0, 0.15, 0.16);
	instance_dieuhoa = mo_dh*Translate(0, -0.13, -0.16);
	dieuhoa(0.64, 0.08, 0.03);

}
// ve den
mat4 instance_den;
void den(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm* instance_den*instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void veDen() {
	// tren
	instance_den = Translate(0, 0.05, 0);
	den(0.64, 0.08, 0.15);
	//truoc 
	instance_den = Translate(0,0,-0.075);
	den(0.6, 0.1, 0.02);
	//sau 
	// vi l < -> TT
	instance_den = Translate(0, 0, 0.075);
	den(0.6, 0.1, 0.02);
	//trai
	instance_den = Translate(-0.3,0,0);
	den(0.04,0.1,0.15);
	//phai
	instance_den = Translate(0.3, 0, 0);
	den(0.04, 0.1, 0.15);
	// bong den
	instance_den = Translate(0,-0.02,0);
	den(0.5,0.15,0.1);
}

// ve cua so
mat4 instance_cuaso, instance_canhCS;
GLfloat mo_cs;
void khungCuaSo(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance_cuaso * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhCuaSo() {
	instance =  RotateY(mo_cs) * (0.3, 0, 0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance_cuaso * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = RotateY(mo_cs) * (-0.3, 0, 0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance_cuaso * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void cuaSo() {
	// tren
	instance_cuaso = Translate(0, 0.59, 0);
	khungCuaSo(1, 0.04, 0.04);
	// duoi
	instance_cuaso = Translate(0, -0.59, 0);
	khungCuaSo(1, 0.04, 0.04);
	// trai
	instance_cuaso = Translate(-0.49, 0, 0);
	khungCuaSo(0.04, 1.2, 0.04);
	// phai
	instance_cuaso = Translate(0.49, 0, 0);
	khungCuaSo(0.04, 1.2, 0.04);
	//cua trai
	instance_cuaso = Translate(0, 0, 0);
	canhCuaSo();
}
// ve quat tran
mat4 instance_quat, quay_canhq;
GLdouble q;
GLboolean quay_q = true;
void quatTran(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * quay_canhq*RotateX(-20)*RotateZ(90) * instance_quat * instance);
	//glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm*instance_quat * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void veQuat() {
	quay_canhq = RotateY(q);
	instance_quat = Translate(0,0 ,-0.25);
	quatTran(0.02, 0.2, 0.5);

	instance_quat = RotateX(120)* instance_quat;
	quatTran(0.02, 0.2, 0.5);

	instance_quat = RotateX(120)* instance_quat;
	quatTran(0.02, 0.2, 0.5);

	instance_quat = Translate(0.1,0, 0);
	quatTran(0.2,0.05,0.05);
}
// ham quay tu dong


// ve nhap matcong
void demo() {
	instance = Scale(2, 2, 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 60, 123);
}
void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT ); 
	ctm = RotateY(anpha);
	//veDieuHoa();
	//veDen();
	//tu();
	veQuat();
	//demo();
	glutSwapBuffers();									   
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
	case 'q' :
		anpha += 5;
		glutPostRedisplay();
		break;
	case '1':
		mo_cs -= 0.1 * 0.6;
		glutPostRedisplay();
		break;
	case 'm':
		dh -= 5;
		if (dh < -180) dh += 180;
		glutPostRedisplay();
		break;
	case 't':
		glutPostRedisplay();
		
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
