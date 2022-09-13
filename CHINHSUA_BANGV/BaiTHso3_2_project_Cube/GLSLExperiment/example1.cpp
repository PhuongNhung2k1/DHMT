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
GLuint model_loc, projection_loc;

mat4 Model;

mat4 instance, instance_bangv, instance_ghegv, instance_mtgv;
GLdouble anpha = 0, num_nk = 0, num_tu, ghe_quay;
GLdouble xx = 0, yy = 0, zz = 0, mo_tu = 0, gap_mt = 0;
mat4 view_mt, dong_mt, view_bangv, ctm, ctm_bangv, keo_nk;
mat4 keo_cuatu, mo_nkGV, view_gheGV;
GLdouble  x_nk, y_nk, z_nk, x_gheGV, y_gheGV, z_gheGV;
// phong
mat4 instance_room, ctm, view_vat;


GLfloat l = -0.6, r = 0.6;
GLfloat bottom = -1.0, top = 0.7;
GLfloat zNear = 0.5, zFar = 1.5;

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

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "projection");
	glEnable(GL_DEPTH_TEST);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}


// ve ban giao vien
void bangv() {
	// mat ban
	//toMau(30, 220, 103);
	instance = Scale(1.0, 0.02, 0.60);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban trai truoc
	instance = Translate(-0.475, -0.46, -0.275) * Scale(0.05, 0.9, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban phai truoc
	instance = Translate(0.475, -0.46, -0.275) * Scale(0.05, 0.9, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban trai sau
	instance = Translate(-0.475, -0.46, 0.275) * Scale(0.05, 0.9, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban phai sau
	instance = Translate(0.475, -0.46, 0.275) * Scale(0.05, 0.9, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// ve cac thanh chan xung quanh ban
	// mat trai
	instance = Translate(-0.49, -0.35, 0) * Scale(0.02, 0.7, 0.6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// mat phai
	instance = Translate(0.5, -0.35, 0) * Scale(0.02, 0.7, 0.6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// mat sau
	instance = Translate(0, -0.35, 0.29) * Scale(1.0, 0.7, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
// ====================ve ngan keo ban
// tam nam chinh giua  z
void ngankeogv() {
	// trai
	instance = Translate(-0.14, 0, 0) * Scale(0.02, 0.15, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * mo_nkGV* instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// phai
	instance = Translate(0.14, 0, 0) * Scale(0.02, 0.15, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * mo_nkGV* instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// truoc
	// keo ngan keo ra
	instance = Translate(0, 0, 0.14) * Scale(0.3, 0.15, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * mo_nkGV*instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// day
	instance = Translate(0,-0.075,0) * Scale(0.3,0.02, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * mo_nkGV* instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	//cua nk
	instance = Translate(0, 0, -0.14) * Scale(0.3, 0.15, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * mo_nkGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// num tay cam
	// tac dong lam num tay cam quay
	instance = Translate(0, 0, -0.16) * RotateZ(num_nk) * Scale(0.05, 0.02, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * mo_nkGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

// hop tu ban giao vien
void tubangv() {
	
	instance = Translate(-0.14, 0, 0) * Scale(0.02, 0.6, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// phai
	instance = Translate(0.14, 0, 0) * Scale(0.02, 0.6, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// sau
	instance = Translate(0, 0, 0.14) * Scale(0.3, 0.6, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// day
	instance = Translate(0, -0.29, 0) * Scale(0.3, 0.02, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cuatu_gv() {
	// cua truoc
	instance = Scale(0.3, 0.6, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// num tay cam
	// tac dong lam cho num quay
	instance =  Translate(0.12,0,-0.01)*RotateZ(num_tu) *Scale(0.02, 0.07, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv  * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// ve gam ban
void gambangv() {
	// trai
	instance = Translate(-0.29, 0.11, 0) * Scale(0.02, 0.2, 0.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// phai
	instance = Translate(0.29, 0.11, 0) * Scale(0.02, 0.2, 0.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// sau
	instance = Translate(0, 0.11, 0.29) * Scale(0.5, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// day
	instance = Scale(0.6, 0.02, 0.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// ve ban gv hoan chinh
void vebangv() {
	//instance_bangv = Identity();
	bangv();
	// 0.5 - 0.05 - 0.15(ban)
	// keo ngan keo ra
	keo_nk = Translate(xx, yy, zz);
	instance_bangv = Translate(-0.3, -0.075, -0.15);
	ngankeogv();
	//instance_bangv = Translate(-0.45, -0.075, -0.16) * RotateY(mo_tu) * Translate(0.15, 0, 0);
	mo_nkGV = Translate(x_nk,y_nk,z_nk);
	
	// tt xuong 60/2 = 30 , tren -16
	instance_bangv = Translate(-0.3, -0.46, -0.15);
	tubangv();
	instance_bangv = Translate(-0.45, -0.46, -0.3)* RotateY(mo_tu)* Translate(0.15, 0, 0);
	//keo_cuatu = RotateY(mo_tu);
	cuatu_gv();
	// vi tam gam dang ow day nen cho xuong va di sang phai
	instance_bangv = Translate(0.15, -0.21, 0);
	gambangv();
}
// ve ghe
void ghegv() {
	// matghe
	instance = Scale(0.6, 0.04, 0.6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv * ctm_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban trai truoc
	instance = Translate(-0.275, -0.32, -0.275) * Scale(0.04, 0.6, 0.04);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv* ctm_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban phai truoc
	instance = Translate(0.275, -0.32, -0.275) * Scale(0.04, 0.6, 0.04);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv * ctm_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban trai sau
	instance = Translate(-0.275, -0.32, 0.275) * Scale(0.04, 0.6, 0.04);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// chan ban phai sau
	instance = Translate(0.275, -0.32, 0.275) * Scale(0.04, 0.6, 0.04);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv * ctm_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void thanhghegv() {
	// thanh ghe
	// mat trai
	instance = Translate(-0.28, 0.35, 0) * Scale(0.04, 0.7, 0.04);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv * ctm_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// mat phai
	instance = Translate(0.28, 0.35, 0) * Scale(0.04, 0.7, 0.04);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv* ctm_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// mat tren
	instance = Translate(0, 0.45, 0.0) * Scale(0.56, 0.5, 0.04);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_bangv* ctm_bangv * instance_ghegv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// ve ghe gv hoan chinh
void veghegv() {
	view_gheGV = Translate(x_gheGV, y_gheGV, z_gheGV);
	instance_ghegv = Translate(0, 0, -0.4) * RotateY(ghe_quay);
	ghegv();
	// dich chuyen thanh ve phia truoc, cach xa ban 1 khoang cacsh
	instance_ghegv = Translate(0, 0, -0.4) * RotateY(ghe_quay) * Translate(0, 0, -0.28);
	thanhghegv();

}
// ve may tinh
void manhinhmt_gv() {
	// man hinh
	instance = Scale(0.4, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * view_mt * dong_mt * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// le trai
	instance = Translate(-0.19, 0, 0) * Scale(0.02, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * view_mt * dong_mt * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// le phai
	instance = Translate(0.19, 0, 0) * Scale(0.02, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * view_mt * dong_mt * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// le tren
	instance = Translate(0, 0.11, 0) * Scale(0.4, 0.02, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * view_mt * dong_mt * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// le duoi
	// co gap may tinh
	instance = Translate(0, -0.11, 0) * Scale(0.4, 0.02, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * view_mt * dong_mt * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


}
// mat ban phim
void banphim_gv() {
	instance = Scale(0.4, 0.04, 0.24);// cong them 0.04 do day khung man hinh
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bangv * view_mt * instance_mtgv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// ve may tinh gv
void vemt_gv() {
	// hien thi trang thai ban dau cua mnt lech cho de quan sat
	// cho may tinh len tren mat ban va lech sang trai
	view_mt = Translate(-0.1, 0.12, 0);
	// thu tu thuc hien phep toan tu ben phai sang
	dong_mt = Translate(0, -0.1, 0.0) * RotateX(gap_mt) * Translate(0, 0.1, 0);
	manhinhmt_gv();
	instance_mtgv = Translate(0, -0.11, -0.1);
	//banphim_gv();
}
// ve va tinh tien khu vuc ban giao vien
// tam dang nam o mat ban
void display_bangv() {
	ctm_bangv = RotateX(-30) * RotateY(20);
	view_bangv = Translate(0, 0, -0.4);
	vebangv();
	view_bangv = Translate(0.15, -0.4, -0.2);
	//veghegv();
	vemt_gv();
}

void wall_room(GLfloat w, GLfloat l, GLfloat h) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, view_vat * instance_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void room_build() {
	instance_room = Identity();
	//instance_room = Translate(0, -0.25, 0);
	wall_room(1.5, 1.5, 0.02);
	instance_room = Translate(0, 0.375, 0.75);
	wall_room(1.5, 0.02, 0.75);
	instance_room = Translate(-0.75, 0.375, 0);
	wall_room(0.02, 1.5, 0.75);
}

// ve phong vfa tinh tien vat
void phongTH() {
	view_vat = Translate(0, 0, 3);

}

mat4 vitri_vat_Phong, model;
// ve dieu hoa demo 
void dieuhoa() {
	//tam chinh no
	// tren
	instance = Translate(0, 0.065, 0) * Scale(0.3, 0.02, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, vitri_vat_Phong * model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// tren
	instance = Translate(0, -0.065, 0) * Scale(0.3, 0.02, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, vitri_vat_Phong * model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// tren
	instance = Translate(-0.14, 0, 0) * Scale(0.02, 0.15, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, vitri_vat_Phong * model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// tren
	instance = Translate(0.14, 0, 0) * Scale(0.02, 0.15, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, vitri_vat_Phong * model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void display( void )
{
	point4 eye(0.5, 2, 1.5, 1);
	point4 at(0.0, 0.0, -0.2, 1.0);
	vec4 up(0.0, 1.0, 0.0, 1.0);

	mat4 mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, mv);

	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);


    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );  
	view_vat = Translate(0, -0.25, 0) * RotateX(-30) * RotateY(30);
	ctm = Translate(0, -0.25, 0) * RotateX(-30) * RotateY(30);
	//room_build();
	//display_bangv();
	dieuhoa();
	//veghegv();
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
		anpha += 5;
		glutPostRedisplay();
		break;
	case 'n':
		num_nk += 5;
		glutPostRedisplay();
		break;
	case 'N':
		num_tu += 5;
		glutPostRedisplay();
		break;
		// mo cua hop tu
	case 'k':
		mo_tu += 5;
		if (mo_tu >=180) mo_tu -= 180;
		glutPostRedisplay();
		break;
	case 'd':
		mo_tu -= 5;
		if (mo_tu < 0) mo_tu += 180;
		glutPostRedisplay();
		break;
	case 'K':
		yy += 0.1 * 0.6;
		glutPostRedisplay();
		break;
		// mo may tinh
	case 'g':
		gap_mt -= 5;
		if (gap_mt < -90) gap_mt += 180;
		glutPostRedisplay();
		break;
	case 'm':
		gap_mt += 5;
		if (gap_mt > 90) gap_mt -= 180;
		glutPostRedisplay();
		break;
	case '1':
		ghe_quay += 5;
		glutPostRedisplay();
		break;
		// mo ngan keo
	case '8':
		z_nk -= 0.1*0.6;
		if (z_nk < -0.3) z_nk =0;
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
