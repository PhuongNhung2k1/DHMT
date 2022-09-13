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
//=== BANGV
GLdouble anpha, z_nk, tuGV_quay, mt_gap;
mat4 instance, instance_bangv, instance_tuGV, instance_nkGV, Monk_GV, Motu_GV, instance_nganbanGV, instance_maytinhGV, gapMT_GV;
mat4 ctm;
GLdouble anpha, z_nk, tuGV_quay, mt_gap;
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
// ====================ve ban giao vien
void banGV(GLfloat w, GLfloat h,GLfloat t) {
	instance = Scale(w,h,t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm*instance_bangv*instance);
	glDrawArrays(GL_TRIANGLES,0,NumPoints);
}
void nk_GV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm*Monk_GV* instance_nkGV* instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void hoptu_GV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm*instance_tuGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void nganBanGV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm*instance_nganbanGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void maytinh_GV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance_maytinhGV* instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void veBanGV() {
	// matban
	instance_bangv = Identity();
	banGV(1, 0.02, 0.6);
	//chan trc trai 
	instance_bangv = Translate(-0.48, -0.41, 0.28);
	banGV(0.04, 0.8, 0.04);
	// phai
	instance_bangv = Translate(-0.48, -0.41, -0.28);
	banGV(0.04, 0.8, 0.04);
	// sau trai
	instance_bangv = Translate(0.48, -0.41, 0.28);
	banGV(0.04, 0.8, 0.04);
	//phai 
	instance_bangv = Translate(0.48, -0.41, -0.28);
	banGV(0.04, 0.8, 0.04);
	// ========= Ve thanh ngang bao quanh ban GV
	// trai 
	instance_bangv = Translate(-0.5, -0.35, 0);
	banGV(0.02,0.7,0.6);
	// phai
	instance_bangv = Translate(0.5, -0.35, 0);
	banGV(0.02, 0.7, 0.6);
	//sau 
	instance_bangv = Translate(0, -0.35, 0.3);
	banGV(1, 0.7, 0.02);
	//========================== NK ban GV
	// trai
	instance_bangv = Translate(-0.14, 0, 0);
	nk_GV(0.02, 0.15, 0.3);
	// phai
	instance_bangv = Translate(0.14, 0, 0);
	nk_GV(0.02, 0.15, 0.3);
	// truoc
	instance_bangv = Translate(0, 0, -0.14);
	nk_GV(0.3,0.15,0.02);
	// sau
	instance_bangv = Translate(0, 0, 0.14);
	nk_GV(0.3, 0.15, 0.02);
	// day
	instance_bangv = Translate(0,-0.065,0);
	nk_GV(0.3, 0.02, 0.3);
	// num nk
	instance_bangv = Translate(0, 0, -0.16);
	nk_GV(0.08, 0.02, 0.02);
	//======================= HOp tu GV
	// trai
	instance_bangv = Translate(-0.14, 0, 0);
	hoptu_GV(0.02, 0.6, 0.3);
	// phai
	instance_bangv = Translate(0.14, 0, 0);
	hoptu_GV(0.02, 0.6, 0.3);
	// trc
	instance_bangv = Translate(0, 0, -0.14)* Motu_GV;
	hoptu_GV(0.3,0.6,0.02);
	// num 
	// 
	instance_bangv *= Translate(0.1, 0,-0.01);
	hoptu_GV(0.02, 0.08, 0.02);
	// sau
	instance_bangv = Translate(0,0,0.14);
	hoptu_GV(0.3, 0.6, 0.02);
	// day
	instance_bangv = Translate(0, -0.29, 0);
	hoptu_GV(0.3,0.02,0.3);
	// =============== ve ngan ban gv
	// trai
	instance_bangv = Translate(-0.29,0,0);
	nganBanGV(0.02,0.15,0.55);
	// phai
	instance_bangv = Translate(0.29, 0, 0);
	nganBanGV(0.02, 0.15, 0.55);
	// sau
	instance_bangv = Translate(0,0,0.275);
	nganBanGV(0.6,0.15,0.02);
	// day
	instance_bangv = Translate(0,-0.075,0);
	nganBanGV(0.6,0.02,0.55);

	//================ Ve may tinh tren ban
	// man hinh
	instance_bangv = gapMT_GV;
	maytinh_GV(0.4, 0.2, 0.02);
	//khung tren
	instance_bangv = Translate(0,0.1,0);
	maytinh_GV(0.4, 0.02, 0.02);
	//khung duoi
	instance_bangv = Translate(0, -0.1, 0);
	maytinh_GV(0.4, 0.02, 0.02);
	// trai 
	instance_bangv = Translate(-0.19,0,0);
	maytinh_GV(0.02,0.2,0.02);
	// phai 
	instance_bangv = Translate(0.19,0,0);
	maytinh_GV(0.02, 0.2, 0.02);
	// ban phim
	instance_bangv = Translate(0,-0.1,0);
	maytinh_GV(0.4,0.02,0.2);
}
void hienthi_BanGV(){
	ctm = RotateX(-30) * RotateY(anpha);
	// tam ngan keo dang o giua ban
	instance_nkGV = Translate(-0.31, -0.085, -0.15);
	// thu tu thuc hien tu phai sang
	Monk_GV = Translate(-0.15,-0.075,0)*Translate(0,0,z_nk)*Translate(0.15, 0.075, 0);
	instance_tuGV = Translate(-0.31, -0.45, -0.15);
	Motu_GV = Translate(-0.15, -0.15, 0)*RotateY(tuGV_quay)*Translate(0.15, 0.15, 0);
	// tac dong den ngan ban
	instance_nganbanGV = Translate(0.17, -0.075, 0);
	// mt
	instance_maytinhGV = Translate(0, 0.1, 0);
	gapMT_GV = RotateX(mt_gap)* Translate(0, 0.21, -0.1);
	veBanGV();
}
//ve dieu hoa
void dieuhoa(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

mat4 instance_dieuhoa;
void veDieuHoa() {
	//tren
	instance_dieuhoa = Translate(0,-0.15,0);
	dieuhoa(0.6, 0.3, 0.0);
	//duoi
	instance_dieuhoa = Translate(0, 0.15, 0);
	dieuhoa(0.6, 0.3, 0.04);
	// trai
	instance_dieuhoa = Translate(-0.3,0,0);
	dieuhoa(0.04,0.3,0.3);
	// phai
	instance_dieuhoa = Translate(0.3, 0, 0);
	dieuhoa(0.04, 0.3, 0.3);
}
void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );                
	//hienthi_BanGV();
	veDieuHoa();
	glutSwapBuffers();									   
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
	case 'q':
		anpha +=5;
		glutPostRedisplay();
		break;
	case '1':
		z_nk -= 0.5*0.1;
		if (z_nk < -0.29) z_nk += 0.3;
		glutPostRedisplay();
		break;
	case '2':
		tuGV_quay += 5;
		if (tuGV_quay >= 180) tuGV_quay -= 180;
		glutPostRedisplay();
		break;
	case 'g':
		mt_gap += 5;
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
