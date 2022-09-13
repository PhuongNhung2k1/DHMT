/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint model_loc;
mat4 model;
mat4 instance;
mat4 instance_room;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;
GLfloat mauAnhSang = 1.0;
GLfloat opacity = 1.0;
float Opacity = 1.0;
GLfloat choisang = 0.2;
GLfloat mauBD = 255;

mat4 ctm_phong;
GLdouble x_phong, y_phong, z_phong;
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
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
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
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	//color(0, 0, 0);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 1);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

void changeColor(GLfloat a, GLfloat b, GLfloat c) {
	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(1.0, 2.0,0.0, 1.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(mauAnhSang, mauAnhSang, mauAnhSang, 1.0);
	color4 light_specular(choisang, choisang, choisang, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(a / 255.0, b / 255.0, c / 255.0, 1.0);
	color4 material_specular(255.0 / 255.0, 255.0 / 255.0, 255 / 255.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
	glUniform1f(glGetUniformLocation(program, "Opacity"), Opacity);
}
//=====================TƯỜNG==================
void wall_room(GLfloat w, GLfloat l, GLfloat h) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void wall_build() {
	instance_room = Identity();
	//dựng nền căn phòng
	instance_room = Translate(0.0, -2.5, 0.0);
	//changeColor(225, 296, 155);
	changeColor(225, 239, 148);
	wall_room(9, 14, 0.05);
	//trần nhà
	instance_room = RotateZ(360) * Translate(0, 2.5, 0.0);
	changeColor(0, 0, 255);
	wall_room(9, 14, 0.05);
	//tường sau
	instance_room = Translate(0, 0, -7.0);
	changeColor(225, 296, 155);
	wall_room(9.0, 0.05, 5.0);
	changeColor(225, 296, 155);
	//mặt trc
	instance_room = Translate(0, 0, 7.0);
	changeColor(225, 296, 155);
	wall_room(9.0, 0.05, 5.0);

	//tường phải
	instance_room = Translate(-4.5, 0, 0.0);
	changeColor(225, 296, 155);
	wall_room(0.05, 14.0, 5.0);
	
	//tường trái
	instance_room = Translate(4.5, 2.0, 5.0); // mảnh trên cửa
	changeColor(225, 296, 155);
	wall_room(0.05, 2.0, 1.0);
	instance_room = Translate(4.5, 0, 6.5);		//giáp tủ
	changeColor(225, 296, 155);
	wall_room(0.05, 1.1, 5.0);

	instance_room = Translate(4.5, -0.1, 3.35);		//giữa cửa to
	changeColor(225, 296, 155);
	wall_room(0.05, 1.3, 1.9);
	instance_room = Translate(4.5, -0.18, -0.359);		//giữa cửa sổ
	changeColor(225, 296, 155);
	wall_room(0.05, 1.9, 2.0);
	instance_room = Translate(4.5, 1.62, -1.5);	// trên cửa sổ
	changeColor(225, 296, 155);
	wall_room(0.05, 11.0, 1.75);
	instance_room = Translate(4.5, -1.75, -1.5);	// dưới cửa sổ
	changeColor(225, 296, 155);
	wall_room(0.05, 11.0, 1.45);
	instance_room = Translate(4.5, -0.2, -6.55);		//cuối lớp
	changeColor(225, 296, 155);
	wall_room(0.05, 0.9, 2.0);

	//trụ
	instance_room = Translate(4.25, 0, -0.7);
	changeColor(225, 296, 155);
	wall_room(0.5, 0.5, 5.0);
	instance_room = Translate(-4.25, 0, -0.7);
	changeColor(225, 296, 155);
	wall_room(0.5, 0.5, 5.0);

}
// ======================VẼ MÁY TÍNH========================
float dcmay_tinh;
mat4 instance_maytinh;
float quayman_hinh;
float man = 0;
void khung_maytinh(float w, float l, float h) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_maytinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void man_hinh(float w, float l, float h) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_maytinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void man_hinh() {

	//khung trên
	instance_maytinh = Translate(dcmay_tinh, 0, 0) * Identity() * Translate(0.12, 0.08, -0.01);
	changeColor(-255, -255, -255);
	khung_maytinh(0.2, 0.01, 0.01);
	//khung dưới
	instance_maytinh = Translate(dcmay_tinh, 0, 0) * Translate(0.12, 0.2, -0.01);
	changeColor(-255, -255, -255);
	khung_maytinh(0.2, 0.01, 0.01);
	//khung trái
	instance_maytinh = Translate(dcmay_tinh, 0, 0) * Translate(0.22, 0.14, -0.01);
	changeColor(-255, -255, -255);
	khung_maytinh(0.01, 0.01, 0.13);
	//khung phải
	instance_maytinh = Translate(dcmay_tinh, 0, 0) * Translate(0.02, 0.14, -0.01);
	changeColor(-255, -255, -255);
	khung_maytinh(0.01, 0.01, 0.13);
	//khung sau
	instance_maytinh = Translate(dcmay_tinh, 0, 0) * Translate(0.12, 0.14, 0);
	changeColor(-255, -255, -255);
	khung_maytinh(0.2, 0.005, 0.12);
	//màn hình
	instance_maytinh = Translate(dcmay_tinh, 0, 0) * Translate(0.12, 0.13, -0.004);
	changeColor(-0, -0, -0);
	man_hinh(0.19, 0.005, 0.11);
	if (man == 1) {
		instance_maytinh = Translate(dcmay_tinh, 0, 0) * Translate(0.12, 0.13, -0.005);
		changeColor(1, 175, 248);
		man_hinh(0.19, 0.005, 0.11);
	}
}
void may_tinh() {
	//de may tính
	instance_maytinh = Identity() * Translate(dcmay_tinh, 0, 0) * Translate(0.12, 0.01, 0.0);
	changeColor(-255, -255, -255);
	khung_maytinh(0.1, 0.05, 0.01);
	//ke máy tính
	instance_maytinh = Translate(dcmay_tinh, 0, 0) * Translate(0.12, 0.05, 0.01) * RotateX(-10);
	changeColor(-255, -255, -255);
	khung_maytinh(0.05, 0.01, 0.08);
	// màn hình
	instance_maytinh = Translate(0, 0.1, 0) * Translate(0, -0.1, 0);
	man_hinh();
}
//=============================BÀN HỌC=======================
GLfloat zn, dcngan_keo;
GLfloat theta[] = { 0,0,0,0 };
mat4 instance_ban;

void matban(GLfloat w, GLfloat l, GLfloat h)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong* model * instance_room * instance_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void chanban(GLfloat w, GLfloat l, GLfloat h)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ngankeo(GLfloat w, GLfloat l, GLfloat h)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong* model * instance_room * instance_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tu_ban(GLfloat w, GLfloat l, GLfloat h)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong* model * instance_room * instance_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ban_hoc() {
	//mặt bàn
	instance_ban = Identity();
	changeColor(250, 200, 35);
	matban(0.6f, 0.4f, 0.01f);
	//chân trái trc
	instance_ban = Translate(-0.29, -0.155, -0.19);
	changeColor(128, 128, 128);
	chanban(0.02, 0.01, 0.3);
	//chân trái sau
	instance_ban = Translate(-0.29, -0.155, 0.19);
	changeColor(128, 128, 128);
	chanban(0.02, 0.01, 0.3);
	//chân phải trc
	instance_ban = Translate(0.29, -0.155, -0.19);
	changeColor(128, 128, 128);
	chanban(0.02, 0.01, 0.3);
	//chân phải sau
	instance_ban = Translate(0.29, -0.155, 0.19);
	changeColor(128, 128, 128);
	chanban(0.02, 0.01, 0.3);
	//thanh ngang
	instance_ban = Translate(-0.295, -0.25, 0);
	changeColor(128, 128, 128);
	chanban(0.01, 0.38, 0.01);
	instance_ban = Translate(0.295, -0.25, 0);
	changeColor(128, 128, 128);
	chanban(0.01, 0.38, 0.01);
	instance_ban = Translate(0, -0.25, 0);
	changeColor(128, 128, 128);
	chanban(0.58, 0.01, 0.01);
	//ngăn kéo
	instance_ban = Translate(0, -0.1, dcngan_keo);
	changeColor(250, 200, 35);
	ngankeo(0.56f, 0.4f, 0.01f);
	//ban phím
	instance_ban = instance_ban * Translate(0.15, 0, 0);
	changeColor(-50, -50, -50);
	ngankeo(0.2, 0.15, 0.02);
	// chuột
	instance_ban = instance_ban * Translate(-0.15, 0, 0);
	changeColor(-50, -50, -50);
	ngankeo(0.03, 0.07, 0.02);
	//tu_ban
	instance_ban = Translate(0.225, -0.25, 0);
	changeColor(250, 200, 35);
	tu_ban(0.15, 0.4, 0.01);
	instance_ban = Translate(0.15, -0.18, 0);
	changeColor(250, 200, 35);
	tu_ban(0.01, 0.4, 0.15);
	instance_ban = Translate(0.3, -0.18, 0);
	changeColor(250, 200, 35);
	tu_ban(0.01, 0.4, 0.15);
	instance_ban = Translate(0.3, -0.18, -0.195) * RotateY(theta[1]) * Translate(-0.075, 0, 0);
	changeColor(250, 200, 35);
	tu_ban(0.15, 0.01, 0.15);
	//máy tính
	instance_ban = Translate(-0.25-dcmay_tinh, 0, 0);
	may_tinh();
}

GLfloat heso = -0.8;
GLfloat dc = -4.8;
void dayban1() {
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(3.5, -1.6, dc - heso * i)* RotateY(90) * Scale(2,3,2);
		ban_hoc();
	}
}
void dayban2() {
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(0.375, -1.6, dc - heso * i) * RotateY(270) * Scale(2, 3, 2);
		ban_hoc();
	}
}
void dayban3() {
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(-0.425, -1.6, dc - heso * i) * RotateY(90) * Scale(2, 3, 2);
		ban_hoc();
	}
}
void dayban4() {
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(-3.5, -1.6, dc - heso * i) * RotateY(270) * Scale(2, 3, 2);
		ban_hoc();
	}
}

void table_build() {
	dayban1();
	dayban2();
	dayban3();
	dayban4();
}
// =============================CỬA RA VÀO==================
GLfloat mo_cua[] = { 0,0 };
mat4 instance_cuavao;
void canh_cua(GLfloat w, GLfloat l, GLfloat h)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_cuavao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tay_nam_cua(GLfloat w, GLfloat l, GLfloat h)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_cuavao * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void bo_cua() {
	instance_cuavao = Identity() * Translate(0,0,-1) * RotateY(mo_cua[0]) * Translate(0,0,0.5);
	changeColor(252.0, 245.0, 76.0);
	canh_cua(0.04, 1.0, 4.0);
	instance_cuavao = instance_cuavao * Translate(0, 0, 0.35);
	changeColor(0, 0, 0);
	tay_nam_cua(0.1, 0.06, 0.5);
	instance_cuavao = Translate(0, 0, 1) * RotateY(mo_cua[1]) * Translate(0, 0, -0.5);
	changeColor(252.0, 245.0, 76.0);
	canh_cua(0.04, 1.0, 4.0);
	instance_cuavao = instance_cuavao * Translate(0, 0, -0.35);
	changeColor(0, 0, 0);
	tay_nam_cua(0.1, 0.06, 0.5);
}
void cua_build() {
	instance_room = Identity() * Translate(4.5, -0.5, 5.0);
	bo_cua();
}
// ============================TỦ========================== 
mat4 instance_tu;
GLfloat theta1[] = { 0,0,0 };
GLfloat xt;

void than_tu(GLfloat w, GLfloat h, GLfloat l)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canh_tu(GLfloat w, GLfloat h, GLfloat l)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tu_build()
{
	instance_tu = Identity();
	// hàng 2 cột 3
	instance_tu = Translate(-0.15, 0, 0) * RotateY(theta1[1]) * Translate(0.15, 0, 0);
	//changeColor(250, 200, 35);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = instance_tu * Translate(0.1, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0, 0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.145, 0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.145, 0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0, 0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	//mat duoi
	instance_tu = Translate(0, -0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);

	// hàng 2 cột 4
	instance_tu = Translate(-0.3, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(-0.2, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(-0.3, 0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	////mat trai
	//instance_tu = Translate(0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.4, 0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(-0.3, 0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	//mat duoi
	instance_tu = Translate(-0.3, -0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);

	// hàng 2 cột 2
	instance_tu = Translate(0.3, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.4, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);;
	//mat sau
	instance_tu = Translate(0.3, 0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.4, 0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.3, 0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	//mat duoi
	instance_tu = Translate(0.3, -0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);

	// hàng 2 cột 1
	instance_tu = Translate(0.45, 0, 0) * RotateY(theta1[1]) * Translate(0.15, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = instance_tu * Translate(0.1, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.6, 0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.7, 0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.6, 0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	//mat duoi
	instance_tu = Translate(0.6, -0.245, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);

	// hàng 3 cột 3
	instance_tu = Translate(0, 0.5, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.1, 0.5, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0, 0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.145, 0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.145, 0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0, 0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 3 cột 4
	instance_tu = Translate(-0.45, 0.5, 0) * RotateY(theta1[1]) * Translate(0.15, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = instance_tu * Translate(0.1, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(-0.3, 0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	////mat trai
	//instance_tu = Translate(0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.4, 0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(-0.3, 0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(-0.3, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 3 cột 2
	instance_tu = Translate(0.15, 0.5, 0) * RotateY(theta1[1]) * Translate(0.15, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = instance_tu * Translate(0.1, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.3, 0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.4, 0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.3, 0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0.3, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 3 cột 1
	instance_tu = Translate(0.6, 0.5, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.7, 0.5, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.6, 0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.7, 0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.6, 0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0.6, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 1 cột 3
	instance_tu = Translate(0, -0.5, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.1, -0.5, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);;
	//mat sau
	instance_tu = Translate(0, -0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.145, -0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.145, -0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat tren
	//instance_tu = Translate(0, 0.7, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);
	//mat duoi
	instance_tu = Translate(0, -0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);

	// hàng 1 cột 4
	instance_tu = Translate(-0.45, -0.5, 0) * RotateY(theta1[1]) * Translate(0.15, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = instance_tu * Translate(0.1, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(-0.3, -0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	////mat trai
	//instance_tu = Translate(0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.4, -0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat tren
	//instance_tu = Translate(-0.3, 0.7, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);
	//mat duoi
	instance_tu = Translate(-0.3, -0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);

	// hàng 1 cột 2
	instance_tu = Translate(0.15, -0.5, 0) * RotateY(theta1[1]) * Translate(0.15, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = instance_tu * Translate(0.1, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.3, -0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.4, -0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat duoi
	instance_tu = Translate(0.3, -0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat tren
	//instance_tu = Translate(0.3, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 2 cột 1
	instance_tu = Translate(0.6, -0.5, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.7, -0.5, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.6, -0.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.7, -0.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat duói
	instance_tu = Translate(0.6, -0.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat tren
	//instance_tu = Translate(0.6, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 4 cột 3
	instance_tu = Translate(0, 1.0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.1, 1.0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0, 1.0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.145, 1.0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.145, 1.0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0, 1.2, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 4 cột 4
	instance_tu = Translate(-0.3, 1.0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(-0.2, 1.0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(-0.3, 1.0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	////mat trai
	//instance_tu = Translate(0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.4, 1.0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(-0.3, 1.2, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(-0.3, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 4 cột 2
	instance_tu = Translate(0.3, 1.0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.4, 1.0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.3, 1.0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.4, 1.0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.3, 1.2, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0.3, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 4 cột 1
	instance_tu = Translate(0.6, 1.0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.7, 1.0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.6, 1.0, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.7, 1.0, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.6, 1.2, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0.6, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 5 cột 3
	instance_tu = Translate(-0.15, 1.5, 0) * RotateY(theta1[1]) * Translate(0.15, 0, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = instance_tu * Translate(0.1, 0, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0, 1.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.145, 1.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.145, 1.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0, 1.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 5 cột 4
	instance_tu = Translate(-0.3, 1.5, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(-0.2, 1.5, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(-0.3, 1.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	////mat trai
	//instance_tu = Translate(0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat phai
	instance_tu = Translate(-0.4, 1.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(-0.3, 1.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(-0.3, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 5 cột 2
	instance_tu = Translate(0.3, 1.5, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.4, 1.5, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.3, 1.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.4, 1.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.3, 1.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0.3, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);

	// hàng 5 cột 1
	instance_tu = Translate(0.6, 1.5, 0);
	changeColor(160, 190, 150);
	canh_tu(0.28, 0.48, 0.01);
	//tay cam
	instance_tu = Translate(0.7, 1.5, 0);
	changeColor(0.7, 0.5, 1);
	than_tu(0.02, 0.1, 0.02);
	//mat sau
	instance_tu = Translate(0.6, 1.5, 0.295);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.5, 0.01);
	//mat trai
	instance_tu = Translate(0.7, 1.5, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.01, 0.5, 0.3);
	////mat phai
	//instance_tu = Translate(-0.4, 0, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.01, 0.5, 0.3);
	//mat tren
	instance_tu = Translate(0.6, 1.7, 0.145);
	changeColor(0.3, 0.7, 1);
	than_tu(0.3, 0.01, 0.3);
	////mat duoi
	//instance_tu = Translate(0.6, -0.245, 0.145);
	//changeColor(0.3, 0.7, 1);
	//than_tu(0.3, 0.01, 0.3);
}

// ============================BẢNG==========================
mat4 instance_bang;
void bang(GLfloat w, GLfloat h, GLfloat l)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_bang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void bang_build()
{
	instance_bang = Identity();
	changeColor(10, 100, 25);
	bang(3.5, 1.8, 0.02);
	instance_bang = Translate(1.65, 0, -0.25);
	changeColor(200, 296, 155);
	//changeColor(47, 79, 79);
	bang(0.5, 1.9, 0.5);
	instance_bang = Translate(-1.65, 0, -0.25);
	changeColor(200, 296, 155);
	bang(0.5, 1.9, 0.5);
	instance_bang = Translate(0, 1.15, -0.25);
	changeColor(200, 296, 155);
	bang(3.4, 0.5, 0.5);
	instance_bang = Translate(0, -1.15, -0.25);
	changeColor(200, 296, 155);
	bang(3.4, 0.5, 0.5);

}
// ==================================VẼ BÀN GV===================================//
mat4  instance_phong, view_vat;
mat4 instance_bangv, Monk_GV, vitri_vat, instance_nkGV, instance_tuGV;
mat4 instance_nganbanGV, instance_maytinhGV, gapMT_GV, Motu_GV;
GLdouble tuGV_quay, mt_gap, z_nk;
GLdouble num_tu, num_nk;
int r_mt = 139, g_mt = 119, b_mt = 193;
void banGV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*instance_room * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void nk_GV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*instance_room * Monk_GV * instance_nkGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void hoptu_GV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*instance_room * instance_tuGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void nganBanGV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*instance_room * instance_nganbanGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void maytinh_GV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*instance_room * instance_maytinhGV * instance_bangv * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void veBanGV() {
	// matban
//instance_bangv = Identity();
	changeColor(84, 255, 159);
	instance_bangv = Translate(0, 0, 0);
	banGV(1, 0.02, 0.6);
	//chan trc trai 
	changeColor(154, 255, 154);
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
	changeColor(151 ,255, 255);
	instance_bangv = Translate(-0.5, -0.35, 0);
	banGV(0.02, 0.7, 0.6);
	// phai
	instance_bangv = Translate(0.5, -0.35, 0);
	banGV(0.02, 0.7, 0.6);
	//sau 
	instance_bangv = Translate(0, -0.35, 0.3);
	banGV(1, 0.7, 0.02);
	//========================== NK ban GV
	// trai
	changeColor(255, 165, 79);
	
	instance_bangv = Translate(-0.14, 0, 0);
	nk_GV(0.02, 0.15, 0.3);
	// phai
	instance_bangv = Translate(0.14, 0, 0);
	nk_GV(0.02, 0.15, 0.3);
	// truoc
	instance_bangv = Translate(0, 0, -0.14);
	nk_GV(0.3, 0.15, 0.02);
	// sau
	instance_bangv = Translate(0, 0, 0.14);
	nk_GV(0.3, 0.15, 0.02);
	// day
	instance_bangv = Translate(0, -0.065, 0);
	nk_GV(0.3, 0.02, 0.3);
	// num nk
	changeColor(255,0,0);
	instance_bangv = RotateZ(num_nk) *Translate(0, 0, -0.16);
	nk_GV(0.08, 0.02, 0.02);
	//======================= HOp tu GV
	// trai
	changeColor(0, 254, 0);
	instance_bangv = Translate(-0.14, 0, 0);
	hoptu_GV(0.02, 0.6, 0.3);
	// phai
	instance_bangv = Translate(0.14, 0, 0);
	hoptu_GV(0.02, 0.6, 0.3);
	// trc
	instance_bangv = Translate(0, 0, -0.14) * Motu_GV;
	hoptu_GV(0.3, 0.6, 0.02);
	// num 
	changeColor(255, 127, 0);
	instance_bangv *= Translate(0.1, 0, -0.01);
	instance_bangv *= RotateZ(num_tu);
	hoptu_GV(0.02, 0.08, 0.02);
	// sau
	instance_bangv = Translate(0, 0, 0.14);
	hoptu_GV(0.3, 0.6, 0.02);
	// day
	instance_bangv = Translate(0, -0.29, 0);
	hoptu_GV(0.3, 0.02, 0.3);
	// =============== ve ngan ban gv
	// trai
	changeColor(162, 205, 90);
	instance_bangv = Translate(-0.29, 0, 0);
	nganBanGV(0.02, 0.15, 0.55);
	// phai
	instance_bangv = Translate(0.29, 0, 0);
	nganBanGV(0.02, 0.15, 0.55);
	// sau
	instance_bangv = Translate(0, 0, 0.275);
	nganBanGV(0.6, 0.15, 0.02);
	// day
	instance_bangv = Translate(0, -0.075, 0);
	nganBanGV(0.6, 0.02, 0.55);

	//================ Ve may tinh tren ban
	// man hinh
	changeColor(r_mt, g_mt, b_mt);
	instance_bangv = gapMT_GV*Translate(0,0,0);
	maytinh_GV(0.4, 0.2, 0.02);
	//khung tren
	instance_bangv = gapMT_GV * Translate(0, 0.1, 0);
	maytinh_GV(0.4, 0.02, 0.02);
	//khung duoi
	instance_bangv = gapMT_GV * Translate(0, -0.1, 0);
	maytinh_GV(0.4, 0.02, 0.02);
	// trai 
	instance_bangv = gapMT_GV * Translate(-0.19, 0, 0);
	maytinh_GV(0.02, 0.2, 0.02);
	// phai 
	instance_bangv = gapMT_GV * Translate(0.19, 0, 0);
	maytinh_GV(0.02, 0.2, 0.02);
	// ban phim
	changeColor(139, 131, 134);
	instance_bangv = Translate(0, -0.1, -0.1);
	maytinh_GV(0.4, 0.02, 0.2);
}


void BanGV_buil() {
		// tam ngan keo dang o giua ban
	instance_nkGV = Translate(-0.31, -0.085, -0.15);
	//ttuTh tu phai sang
	Monk_GV =  Translate(0, 0, z_nk);
	instance_tuGV = Translate(-0.31, -0.45, -0.15);
	Motu_GV = Translate(-0.15, -0.15, 0) * RotateY(tuGV_quay) * Translate(0.15, 0.15, 0);
	// tac dong den ngan ban
	instance_nganbanGV = Translate(0.17, -0.075, 0);
	// mt
	instance_maytinhGV = Translate(0, 0.11, 0);
	// phai sang -> quay lai ban dau
	gapMT_GV = Translate(0, -0.1, 0) * RotateX(mt_gap) * Translate(0, 0.1, 0);
	veBanGV();
}
// =============== VẼ ĐIỀU HÒA=================
mat4 instance_dieuhoa, mo_dh;
GLdouble anpha, dh;
//ve dieu hoa
void dieuhoa(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*instance_room* instance_dieuhoa * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void veDH() {
	//tren
	changeColor(255 ,255, 255);
	instance_dieuhoa = Translate(0, -0.15, 0);
	dieuhoa(0.64, 0.04, 0.3);
	//duoi
	instance_dieuhoa = Translate(0, 0.15, 0);
	dieuhoa(0.64, 0.04, 0.3);
	// trai
	instance_dieuhoa = Translate(-0.3, 0, 0);
	dieuhoa(0.04, 0.3, 0.3);
	// phai
	instance_dieuhoa = Translate(0.3, 0, 0);
	dieuhoa(0.04, 0.3, 0.3);
	// sau
	instance_dieuhoa = Translate(0, 0, 0.15);
	dieuhoa(0.6, 0.34, 0.04);
	// giua
	instance_dieuhoa = RotateX(-10);
	dieuhoa(0.55, 0.03, 0.29);
	// truoc tren
	instance_dieuhoa = Translate(0, 0.09, -0.16);
	dieuhoa(0.64, 0.18, 0.03);
	// duoi
	mo_dh = Translate(0, -0.15, -0.16) * RotateX(dh) * Translate(0, 0.15, 0.16);
	instance_dieuhoa = mo_dh * Translate(0, -0.13, -0.16);
	dieuhoa(0.64, 0.08, 0.03);

}
void DieuHoa_buil() {
	for (int i = 0; i <= 6; i+=5) {
		instance_room = Translate(4.25, 2, -4.5+i) * RotateY(90);
		veDH();
		instance_room = Translate(-4.25, 2, -4.5 + i) * RotateY(-90);
		veDH();
	}
}
//=============== VẼ ĐÈN=======================
mat4 instance_den;
bool bat_den = true;
bool view_banGV = true;
void den(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*instance_room * instance_den * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void veDen() {
	// tren
	changeColor(255, 215, 0);
	instance_den = Translate(0, 0.03, 0);
	den(0.64, 0.04, 0.15);
	//truoc 
	instance_den = Translate(0, 0, -0.075);
	den(0.6, 0.06, 0.02);
	//sau 
	// vi l < -> TT
	instance_den = Translate(0, 0, 0.075);
	den(0.6, 0.06, 0.02);
	//trai
	instance_den = Translate(-0.3, 0, 0);
	den(0.04, 0.06, 0.15);
	//phai
	instance_den = Translate(0.3, 0, 0);
	den(0.04, 0.06, 0.15);
	// bong den
	changeColor(mauBD, mauBD, mauBD);
	instance_den = Translate(0, -0.03, 0);
	den(0.5, 0.06, 0.1);
}

void Den_buil() {
	for (int i = 0; i <= 12; i+=2) {
		instance_room = Translate(1.3, 2.2, -6.5 + i);
		veDen();
		instance_room = Translate(-1.3, 2.2, -6.5 + i);
		veDen();
	}

}
// =========================VẼ GHẾ GV===================
mat4 instance_gheGV, dc_ghe;
GLfloat x_gheGV =0, y_gheGV=0, z_gheGV;

void ghe_GV(GLfloat w, GLfloat h, GLfloat t) {
	instance = Scale(w, h, t);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong* dc_ghe*instance_room * instance_gheGV * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void veGheGV() {
	dc_ghe = Translate(x_gheGV, y_gheGV, z_gheGV);
	// mat ghe
	changeColor(238 ,0 ,0);
	instance_gheGV = Identity()*Translate(0,0,0);
	ghe_GV(0.3, 0.04, 0.2);
	changeColor(255, 165, 0);
	//chan truoc
	instance_gheGV = Translate(-0.14, -0.25, -0.1);
	ghe_GV(0.02, 0.5, 0.02);
	//chan truoc
	instance_gheGV = Translate(0.14, -0.25, -0.1);
	ghe_GV(0.02, 0.5, 0.02);
	//chan truoc
	instance_gheGV = Translate(0.14, -0.25, 0.1);
	ghe_GV(0.02, 0.5, 0.02);
	//chan truoc
	instance_gheGV = Translate(-0.14, -0.25, 0.1);
	ghe_GV(0.02, 0.5, 0.02);
	changeColor(255, 165, 0);
	//lung ghe
	instance_gheGV = Translate(0, 0.5, -0.1);
	ghe_GV(0.3,0.1,0.02);
	//lung gh 2
	instance_gheGV = Translate(0, 0.25, -0.1);
	ghe_GV(0.3, 0.05, 0.02);
	//thanh lung trai
	instance_gheGV = Translate(-0.14, 0.25, -0.1);
	ghe_GV(0.02,0.5,0.02);
	//thanh lung phai
	instance_gheGV = Translate(0.14, 0.25, -0.1);
	ghe_GV(0.02, 0.5, 0.02);
}
// ==============RÈM CỬA=======================
mat4 instance_rem;
float chieucao_rem = 1;
float cuon_rem;
void khung_rem(float w, float h, float l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_rem * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void rem(float w, float h, float l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_rem * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void rem_build() {
	instance_rem = Identity() * Translate(0, 0.4, 0.08);
	khung_rem(0.5, 0.02, 0.004);
	instance_rem = instance_rem * Translate(0, cuon_rem, 0) * Translate(0, -0.51, 0.1);
	changeColor(0, 255, 0);
	rem(0.5, chieucao_rem, 0.002);
}
// ==============MÁY CHIẾU====================
mat4 instance_maychieu;
float chieu_dai = 1;
float cuon_may;
void may_chieu(float w, float h, float l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_maychieu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void man_maychieu(float w, float h, float l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_maychieu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void khung_maychieu(float w, float h, float l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_maychieu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void maychieu_build() {
	instance_maychieu = Identity();
	changeColor(192, 192, 192);
	may_chieu(0.01, 0.2, 0.01);
	instance_maychieu = instance_maychieu * Translate(0, -0.15, 0);
	may_chieu(0.2, 0.1, 0.1);
}
void manmc_build() {
	changeColor(0, 0, 0);
	instance_maychieu = Identity() * Translate(0, 0.4, 0);
	khung_maychieu(0.5, 0.02, 0.01);
	instance_maychieu = instance_maychieu * Translate(0, cuon_may, 0) * Translate(0, -0.5, 0);
	changeColor(255, 255, 255);
	man_maychieu(0.5, chieu_dai, 0.002);
}
// ==========================GHẾ HS===================
mat4 instance_ghe;
void matGhe(GLfloat w, GLfloat h, GLfloat l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void chanGhe(GLfloat w, GLfloat h, GLfloat l)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong* model * instance_room * instance_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void boGhe() {
	instance_ghe = Identity();
	changeColor(250, 200, 35);
	matGhe(0.4, 0.02, 0.4);		// mặt ghế
	instance_ghe = Translate(0, 0.2, 0.19) * RotateX(-90);	// thành ghế
	matGhe(0.4, 0.02, 0.4);
	instance_ghe = Translate(-0.185, -0.16, -0.185);	// trái trc
	chanGhe(0.03, 0.3, 0.03);
	instance_ghe = Translate(0.185, -0.16, -0.185);	// phải trc
	chanGhe(0.03, 0.3, 0.03);
	instance_ghe = Translate(-0.185, -0.16, 0.185);	// trái sau
	chanGhe(0.03, 0.3, 0.03);
	instance_ghe = Translate(0.185, -0.16, 0.185);	// phải sau
	chanGhe(0.03, 0.3, 0.03);
}
GLfloat HS = -0.8;
GLfloat bd = -4.8;
GLfloat x_ghe = 0;
GLfloat z_ghe = 0;
void ghe() {
	//instance_room = Identity() * Translate(3.0, -2.0, -4.8) * RotateY(-90) * Scale(0.9, 1.5, 1);
	//boGhe();

	//dãy ghế trái
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(2.5, -2.0, bd - HS * i) * RotateY(-90) * Scale(0.8, 1.5, 0.95);
		boGhe();
	}
	// dãy ghế giữa
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(-1.5, -2.0, bd - HS * i) * RotateY(270) * Scale(0.8, 1.5, 0.95);
		boGhe();
	}
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(x_ghe, 0, z_ghe) * Translate(1.5, -2.0, bd - HS * i) * RotateY(90) * Scale(0.8, 1.5, 0.95);
		boGhe();
	}
	//dãy ghế phải
	for (int i = 0; i < 11; i++) {
		instance_room = Translate(-2.5, -2.0, bd - HS * i) * RotateY(-270) * Scale(0.8, 1.5, 0.95);
		boGhe();
	}
}

// =================CỬA SỔ======================
mat4 instance_cuaso;
void khung(GLfloat w, GLfloat h, GLfloat l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_cuaso * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canh(GLfloat w, GLfloat h, GLfloat l)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_cuaso * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tayNam(GLfloat w, GLfloat h, GLfloat l)
{
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong*model * instance_room * instance_cuaso * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat xCuaSo1 = 0;
GLfloat xCuaSo2 = 0;
GLfloat quayTayNam1 = 0, quayTayNam2 = 0;

void bo_cuaSo_trai() {
	instance_cuaso = Identity();
	//instance_cuaso = Translate();
	changeColor(222.0, 245.0, 76.0);
	khung(1.2, 0.05, 0.05);			// khung trên
	instance_cuaso = Translate(0, -0.55, 0);	// khung dưới
	khung(1.2, 0.05, 0.05);
	instance_cuaso = Translate(-0.575, -0.275, 0);  // khung trái
	khung(0.05, 0.5, 0.05);
	instance_cuaso = Translate(0.575, -0.275, 0);	// khung phải
	khung(0.05, 0.5, 0.05);

	instance_cuaso = Translate(xCuaSo2, 0, 0) * Translate(0.25, -0.275, 0.015);	// cánh trái
	changeColor(272.0, 280.0, 66.0);
	canh(0.6, 0.5, 0.01);
	changeColor(10, 120, 25);
	instance_cuaso = instance_cuaso * Translate(0.26, -0.1, -0.015) * Translate(0.01, 0, 0) * RotateZ(quayTayNam1);
	tayNam(0.02, 0.01, 0.02);
	instance_cuaso = instance_cuaso * Translate(0, -0.045, -0.02);
	tayNam(0.02, 0.1, 0.02);

	instance_cuaso = Translate(xCuaSo1, 0, 0) * Translate(-0.25, -0.275, 0);	// cánh phải
	changeColor(272.0, 290.0, 66.0);
	canh(0.6, 0.5, 0.01);
	instance_cuaso = instance_cuaso * Translate(-0.26, -0.1, -0.015) * Translate(0.01, 0, 0) * RotateZ(quayTayNam2);
	changeColor(10, 120, 25);
	tayNam(0.02, 0.01, 0.02);
	instance_cuaso = instance_cuaso * Translate(0, -0.045, -0.02);
	tayNam(0.02, 0.1, 0.02);
}
void cuaSo_trai() {
	//instance_room = Identity() * RotateY(90) * Scale(0.6, 0.35, 0.05) * Translate(-0.5, -0.2, 0);
	instance_room = Identity() * Translate(4.4, 0.7, -5.55) * RotateY(90) * Scale(1, 3.0, 0.05);
	bo_cuaSo_trai();
	instance_room = instance_room * Translate(-1.2, 0, 0);
	bo_cuaSo_trai();
	instance_room = instance_room * Translate(-1.2, 0, 0);
	bo_cuaSo_trai();
	instance_room = instance_room * Translate(-1.2, 0, 0);
	bo_cuaSo_trai();
	instance_room = instance_room * Translate(-3.0, 0, 0);
	bo_cuaSo_trai();
	instance_room = instance_room * Translate(-1.2, 0, 0);
	bo_cuaSo_trai();
}
mat4 instance_cuaso1;

void cuaPhai(GLfloat w, GLfloat h, GLfloat l) {
	instance = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_phong * model * instance_room * instance_cuaso1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat quayCuaSoPhai = 0;
GLfloat quayTayNam = 0;
void bo_cuaSo_phai() {
	instance_cuaso1 = Identity();
	changeColor(222.0, 245.0, 76.0);
	cuaPhai(0.7, 0.05, 0.05);			// khung trên
	instance_cuaso1 = Translate(0, -0.55, 0);	// khung dưới
	cuaPhai(0.7, 0.05, 0.05);
	instance_cuaso1 = Translate(-0.325, -0.275, 0);  // khung trái
	cuaPhai(0.05, 0.5, 0.05);
	instance_cuaso1 = Translate(0.325, -0.275, 0);	// khung phải
	cuaPhai(0.05, 0.5, 0.05);
	instance_cuaso1 = RotateX(quayCuaSoPhai) * Translate(0, -0.025, 0) * Translate(0, -0.25, 0);	//  kính
	changeColor(272.0, 278.0, 66.0);
	//changeColor(252.0, 245.0, 76.0);
	cuaPhai(0.6, 0.5, 0.03);
	instance_cuaso1 = instance_cuaso1 * Translate(-0.285, 0, -0.015) * Translate(0.005, 0, 0) * RotateZ(quayTayNam);	// tay nắm
	changeColor(10, 120, 25);
	cuaPhai(0.01, 0.02, 0.02);
	instance_cuaso1 = instance_cuaso1 * Translate(0.045, 0, -0.02);	// tay nắm
	cuaPhai(0.1, 0.02, 0.02);
}
void cuaSo_phai() {
	instance_room = Identity() * Translate(-4.45, 0.7, -5.2) * RotateY(-90) * Scale(0.6, 2.85, 0.05);
	//instance_room = identity();
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();

	instance_room = instance_room * Translate(2.5, 0, 0);
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();

	instance_room = instance_room * Translate(4.2, 0, 0);
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();

	instance_room = instance_room * Translate(2.5, 0, 0);
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();
	instance_room = instance_room * Translate(0.7, 0, 0);
	bo_cuaSo_phai();
}
//========================================HIỂN THỊ PHÒNG============================
void room_build() {
	wall_build();
	table_build();
	cua_build();
	instance_room = Translate(0, 0, 6.8) * Scale(1.3, 1.5, 1.5);
	bang_build();
	instance_room = Translate(0, 0, xt) * Translate(3.25, -1.4, 6.0) * Scale(1.25, 1.4, 1.3);
	tu_build();
	instance_room = Translate(-2.8, -1.2, 4.5)*RotateY(180);
	BanGV_buil();
	Den_buil();
	DieuHoa_buil();
	instance_room = Translate(-2.8, -1.3, 5.1)*RotateY(180);
	veGheGV();
	instance_room = Translate(-4.7, 0.5, -3.5) * RotateY(90) * Scale(8, 3.5, 3);
	rem_build();
	instance_room = Translate(-4.7, 0.5, 2) * RotateY(90) * Scale(8, 3.5, 3);
	rem_build();
	instance_room = Translate(-0.7, 2, 0) * RotateY(90) * Scale(1, 3, 3);
	maychieu_build();
	instance_room = Translate(-1, 0.2, 5.9) * Scale(3, 3, 3);
	manmc_build();
	ghe();
	cuaSo_trai();
	cuaSo_phai();
}/**/

GLfloat l = -2.5, r = 2.5;
GLfloat bottom = -2.5, top = 2.5;
GLfloat zNear = 1, zFar = 17;
GLfloat z_eye = -7.0, y_eye = 0.5;
GLdouble xx_p, yy_ph, zz_p;
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const vec3 viewer_pos(0.0, y_eye, z_eye);  /*Trùng với eye của camera*/

	ctm_phong = Translate(x_phong, y_phong, z_phong) * RotateY(yy_ph);
	room_build();
	vec4 eye(0.0, y_eye, z_eye, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);
	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
	glutSwapBuffers();
}

void reshape(int width, int height)
{

	glViewport(0, 0, width, height);
}
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
		// mở đóng tủ bàn học
	case 't':
		theta[1] += -10;
		if (theta[1] <= -90) theta[1] -= (theta[1] + 90);
		break;
	case 'T':
		theta[1] = 0;
		break;
		//mở đóng cửa ra vào
	case 'c':
		mo_cua[0] += 5;
		mo_cua[1] += -5;
		if (mo_cua[0] >= 178) { mo_cua[0] -= (mo_cua[0] - 178); }
		if (mo_cua[1] <= -178) { mo_cua[1] -= (mo_cua[1] + 178); }
		break;
	case 'C':
		mo_cua[0] = 0;
		mo_cua[1] = 0;
		break;
		//kéo đóng ngăn kéo
	case 'k':
		dcngan_keo += -0.1;
		if (dcngan_keo <= -0.3) dcngan_keo -= (dcngan_keo + 0.3);
		break;
	case 'K':
		dcngan_keo = 0;
		break;
		//bật tắt màn hình
	case 's':
		man = 1;
		break;
	case 'S':
		man = 0;
		break;
		//dịch chuyển máy tính
	case 'i':
		dcmay_tinh += 0.01;
		if (dcmay_tinh >= 0.05) dcmay_tinh -= (dcmay_tinh - 0.05);
		break;
	case 'I':
		dcmay_tinh += -0.01;
		if (dcmay_tinh <= -0.05) dcmay_tinh -= (dcmay_tinh + 0.05);
		break;
		// kéo rèm
	case 'r':
		chieucao_rem -= 0.1;
		cuon_rem += 0.05;
		if (chieucao_rem <= 0) {
			chieucao_rem += 0.1;
			cuon_rem -= 0.05;
		}
		break;
	case 'R':
		chieucao_rem += 0.1;
		cuon_rem -= 0.05;
		if (chieucao_rem >= 1) {
			chieucao_rem -= 0.1;
			cuon_rem += 0.05;
		}
		break;
		//cuộn màn máy chiếu
	case 'p':
		chieu_dai -= 0.1;
		cuon_may += 0.05;
		if (chieu_dai <= 0) {
			chieu_dai += 0.1;
			cuon_may -= 0.05;
		}
		break;
	case 'P':
		chieu_dai += 0.1;
		cuon_may -= 0.05;
		if (chieu_dai >= 1) {
			chieu_dai -= 0.1;
			cuon_may += 0.05;
		}
		break;
		//dịch chuyển tủ
	case 'n':
		xt = 0;
		break;
	case 'N':
		xt -= 0.1;
		if (xt < -3) xt += 0.5;
		break;
		//đóng mở cửa tủ
	case 'm':
		theta1[1] += 5;
		if (theta1[1] > 90) theta1[1] -= 5;
		break;
	case 'M':
		theta1[1] -= 5;
		if (theta1[1] < 0) theta1[1] += 5;
		break;
		// bat den
	case 'd':
		if (bat_den) {
			mauBD = 100;
			mauAnhSang = 0.2;
			bat_den = false;
		}
		else {
			mauBD = 255;
			mauAnhSang = 1;
			bat_den = true;
		}
		glutPostRedisplay();
		break;
		// dịch chuyển phòng sag phải, trái
	case '1':
		x_phong += 0.3 * 0.5;
		glutPostRedisplay();
		break;
	case '-':
		x_phong -= 0.3 * 0.5;
		glutPostRedisplay();
		break;
	case '2':
		yy_ph += 8;
		if (yy_ph > 360) yy_ph -= 8;
		glutPostRedisplay();
		break;
		// dịch chuyển phòng lên, xuống 
	case '3':
		y_phong += 0.3 * 0.5;
		if (y_phong > 360) y_phong -= 0.3 * 0.5;
		glutPostRedisplay();
		break;
	case '=':
		y_phong -= 0.3 * 0.5;
		if (y_phong > 360) y_phong -= 0.3 * 0.5;
		glutPostRedisplay();
		break;
	// thay đổi góc nhìn eye khu vuc ban giao vien
	case '5':
		if (view_banGV) {
			z_eye = 7;
			zNear = 2;
			break;
	case '6':
		z_eye = -7;
		zNear = 1.5;
		view_banGV = true;
		glutPostRedisplay();
		break;
		// mo dieu hoa
	case 'h':
		dh -= 5;
		if (dh < -180) dh += 180;
		glutPostRedisplay();
		break;
		// Mo ngan keo ban giao vien	
	case ';':
		z_nk -= 0.5 * 0.1;
		if (z_nk < -0.29) z_nk += 0.3;
		glutPostRedisplay();
		break;
		// mo cua tu ban giao vien
	case 'u':
		tuGV_quay += 5;
		if (tuGV_quay >= 180) tuGV_quay -= 180;
		glutPostRedisplay();
		break;
		// xoay num tu
	case 'U':
		num_tu += 5;
		glutPostRedisplay();
		break;
		// xoay num ngan keo
	case 'l':
		num_nk += 5;
		glutPostRedisplay();
		break;
		//di chuyển ghế giao vien
	case 'L':
		z_gheGV -= 0.5 * 0.1;
		if (z_gheGV <= -0.3) z_gheGV += 0.3;
		glutPostRedisplay();
		break;
		//mo may tinh
	case 'w':
		mt_gap += 5;
		if (mt_gap > 90) mt_gap -= 180;
		glutPostRedisplay();
		break;
		// gap may tinh
	case 'W':
		mt_gap -= 5;
		if (mt_gap < -90) {
			mt_gap += 120;
			r_mt = 193, g_mt = 255, b_mt = 193;
		}
		glutPostRedisplay();
		break;
		view_banGV = false;
		}
		// di chuyen ghe SV
	case 'g':
		x_ghe += 0.1;
		if (x_ghe > 0.4) {
			x_ghe -= x_ghe - 0.4;
		}
		break;
	case 'G':
		x_ghe -= 0.1;
		if (x_ghe < -0.2) {
			x_ghe -= x_ghe + 0.2;
		}
		break;
		// mở, đóng cửa sổ trái
	case 'o':
		quayTayNam1 -= 1;
		if (quayTayNam1 <- 90) {
			quayTayNam1 += 90;
		}
		glutPostRedisplay();
		break;
	case 'O':
		if (quayTayNam1 > 0) {
			quayTayNam1 -= 2;
		}
		break;
	case 'b':
		xCuaSo2 -= 0.1;
		if (xCuaSo2 < -0.45) {
			xCuaSo2 -= xCuaSo2 + 0.45;
		}
		break;
	case 'B':
		xCuaSo2 += 0.1;
		if (xCuaSo2 > 0)
			xCuaSo2 -= xCuaSo2;
		break;

	case 'x': l *= 1.1; r *= 1.1; break;
	case 'X': l *= 0.9; r *= 0.9; break;
	case 'y': bottom *= 1.1; top *= 1.1; break;
	case 'Y': bottom *= 0.9; top *= 0.9; break;
	case 'z': zNear *= 1.1; zFar *= 1.1; break;
	case 'Z': zNear *= 0.9; zFar *= 0.9; break;
	case 0:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	}
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000, 720);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("A Cube is rotated by keyboard and shaded");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
