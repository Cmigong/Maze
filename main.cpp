#include <algorithm>
#include <iostream>
#include <windows.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/GLAUX.H>
#include <math.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>

using namespace std;

struct walllist
{
    float point1x;
    float point1z;
    float point2x;
    float point2z;
};

static float angleX = 0.0, angleY = 0.0, rati;//angle绕y轴的旋转角，ratio窗口高宽比
static float mx = 50.0f, my = 2.6f, mz = 50.0f;//相机位置
static float lx = 0.0f, ly = 0.0f, lz = -1.0f,px=mx,pz=mz;//视线方向，初始设为沿着Z轴负方向
static GLint wall_display_list;
static POINT mousePos;
static int middleX = GetSystemMetrics(SM_CXSCREEN)/2;
static int middleY = GetSystemMetrics(SM_CYSCREEN)/2;
static float lastmx=middleX,lastmy=middleY;
static UINT g_cactus[16];
static int wallnum=0;
static walllist wall[1500];

void light0()
{
    GLfloat spot[] = {mx, my, mz,1};
    GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat spot_position[] = {mx+lx, my+ly, mz+lz};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,ambientLight);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
    glLightfv(GL_LIGHT0,GL_POSITION,spot);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,spot_position);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,10.0f);
    glEnable(GL_LIGHT0);
}
//定义观察方式
void changeSize(int w, int h)
{
	//除以0的情况
	if (h == 0)  h = 1;
	rati= 1.0f*w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//设置视口为整个窗口大小
	glViewport(0, 0, w, h);
	//设置可视空间
	gluPerspective(45, rati, 1, 3500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(mx, my, mz, mx + lx, my + ly, mz + lz, 0.0f, 1.0f, 0.0f);
}

void drawWall()
{
    glPushMatrix();
	//画立方体的4个面
	glBegin(GL_QUADS);
	glNormal3f(0.0F, 0.0F, 1.0F);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5, 5, 5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5, 5, 5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5, -5, 5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5, -5, 5);
	glEnd();
	//1----------------------------
	glBegin(GL_QUADS);
	glNormal3f(0.0F, 0.0F, -1.0F);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5, -5, -5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5, 5, -5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5, 5, -5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5, -5, -5);
	glEnd();
	//2----------------------------
	glBegin(GL_QUADS);
	glNormal3f(1.0F, 0.0F, 0.0F);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5 , 5 , 5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5, -5, 5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5, -5, -5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5, 5, -5);
	glEnd();
	//5----------------------------
	glBegin(GL_QUADS);
	glNormal3f(-1.0F, 0.0F, 0.0F);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5, -5, -5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5, -5, 5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5, 5, 5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5, 5, -5);
	glEnd();
	//6----------------------------*/
	glPopMatrix();
}

GLuint createDL()
{
	GLuint WallDL= glGenLists(1);
	//开始显示列表
	glNewList(WallDL, GL_COMPILE);
	drawWall();
	glEndList();
	return (WallDL);
}

bool LoadT81(char *filename, GLuint &texture)
{
    AUX_RGBImageRec *pImage = NULL;
	pImage = auxDIBImageLoad(filename);
	if(pImage == NULL)		return false;
	glGenTextures(1, &texture);
	glBindTexture    (GL_TEXTURE_2D,texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D,4, pImage->sizeX,pImage->sizeY,GL_RGB, GL_UNSIGNED_BYTE,pImage->data);
	return true;
}

void texture(UINT textur)
{
    glBindTexture(GL_TEXTURE_2D, textur);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
}

void CreateSkyBox()
{
    float MAP=32*12;
    int a=4,wi=4,he=2,le=4;
    float width =MAP*wi;
	float height=MAP*he;
	float length=MAP*le;
	float x=MAP  -width /2;
	float y=MAP/a-height/2;
	float z=-MAP -length/2;
///////////////////////////////////////////////////////////////////////////////
	texture(g_cactus[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f,0.0f); glVertex3f(x+width,y,		 z);
		glTexCoord2f(1.0f,1.0f); glVertex3f(x+width,y+height,z);
		glTexCoord2f(0.0f,1.0f); glVertex3f(x,		y+height,z);
		glTexCoord2f(0.0f,0.0f); glVertex3f(x,		y,		 z);
	glEnd();
	texture(g_cactus[3]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f,0.0f); glVertex3f(x,		y,		 z+length);
		glTexCoord2f(1.0f,1.0f); glVertex3f(x,		y+height,z+length);
		glTexCoord2f(0.0f,1.0f); glVertex3f(x+width,y+height,z+length);
		glTexCoord2f(0.0f,0.0f); glVertex3f(x+width,y,		 z+length);
	glEnd();
	texture(g_cactus[4]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,1.0f); glVertex3f(x+width,y+height,z);
		glTexCoord2f(0.0f,0.0f); glVertex3f(x+width,y+height,z+length);
		glTexCoord2f(1.0f,0.0f); glVertex3f(x,		y+height,z+length);
		glTexCoord2f(1.0f,1.0f); glVertex3f(x,		y+height,z);
	glEnd();
	texture(g_cactus[5]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f,1.0f); glVertex3f(x,		y+height,z);
		glTexCoord2f(0.0f,1.0f); glVertex3f(x,		y+height,z+length);
		glTexCoord2f(0.0f,0.0f); glVertex3f(x,		y,		 z+length);
		glTexCoord2f(1.0f,0.0f); glVertex3f(x,		y,		 z);
	glEnd();
	texture(g_cactus[6]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex3f(x+width,y,		 z);
		glTexCoord2f(1.0f,0.0f); glVertex3f(x+width,y,		 z+length);
		glTexCoord2f(1.0f,1.0f); glVertex3f(x+width,y+height,z+length);
		glTexCoord2f(0.0f,1.0f); glVertex3f(x+width,y+height,z);
	glEnd();
}

void setWall()
{
    texture(g_cactus[1]);
	for (int i= -3; i< 3; i++)
    {
        for (int j = -3; j< 3; j++)
        {
			glPushMatrix();
			glTranslatef(i*11, 0, j*11);
			glCallList(wall_display_list);
			glPopMatrix();
		}
    }
}

void recordWall()
{
    for(int i=-3;i<3;i++)
    {
        for(int j=-3;j<3;j++)
        {
            wall[wallnum].point1x=(i*11-5);
			wall[wallnum].point1z=(i*11-5);
			wall[wallnum].point2x=(i*11+5);
			wall[wallnum].point2z=(i*11+5);
			wallnum++;
        }
    }
}

void initScenne()
{
    char str[]="data/images/sand0.bmp";
    LoadT81(str,	 g_cactus[0]);
    char str1[]="data/images/0RBack.bmp";
    LoadT81(str1, g_cactus[2]);
    char str2[]="data/images/0Front.bmp";
	LoadT81(str2, g_cactus[3]);
	char str3[]="data/images/0Top.bmp";
	LoadT81(str3,	 g_cactus[4]);
	char str4[]="data/images/0Left.bmp";
	LoadT81(str4,  g_cactus[5]);
	char str5[]="data/images/0Right.bmp";
	LoadT81(str5, g_cactus[6]);
	char str6[]="data/images/sand2.bmp";
	LoadT81(str6, g_cactus[1]);
    glEnable(GL_TEXTURE_2D);
    SetCursorPos(middleX, middleY);
    ShowCursor(false);
	glEnable(GL_DEPTH_TEST);
	wall_display_list = createDL();
	glEnable(GL_LIGHTING);
	GLfloat ambientLight[] = {10.9f, 10.9f, 10.9f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
	recordWall();
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	light0();
    CreateSkyBox();
    //画地面
	texture(g_cactus[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);glVertex3f(-100.0f, 0.0f, -100.0f);
	glTexCoord2f(1.0f,0.0f);glVertex3f(-100.0f, 0.0f, 100.0f);
	glTexCoord2f(1.0f,1.0f);glVertex3f(100.0f, 0.0f, 100.0f);
	glTexCoord2f(0.0f,1.0f);glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	setWall();
    glutSwapBuffers();
}

bool isWall(float x,float z)
{
    for(int i=0;i<wallnum;i++)
    {
        if(wall[i].point1x-x<1.3&&wall[i].point2x-x>-1.3&&z-wall[i].point1z>-1.3&&wall[i].point2z-z>-1.3) return false;
    }
    return true;
}

void kickWall(float x,float z)
{
    int t;
    if(!isWall(mx,mz))
    {
        t=mx;
        mx=x;
        if(isWall(mx,mz)) return;
        else mx=t;
        t=mz;
        mz=z;
        if(isWall(mx,mz)) return;
        else mz=t;
    }
}

//移动相机
void moveMeFlat(int direction)
{
    px=mx;
    pz=mz;
    if(direction==1||direction==-1)
    {
        mx+=(float)direction*cos(3.14*angleX/180.0f)*0.2;
        mz+=(float)direction*sin(3.14*angleX/180.0f)*0.2;
        glLoadIdentity();
        kickWall(px,pz);
        gluLookAt(mx, my, mz, mx + lx, my + ly, mz + lz, 0.0f, 1.0f, 0.0f);
    }
    else if(direction==2||direction==-2)
    {
        mx+=(float)direction/2*cos(3.14*(angleX-90)/180.0f)*0.2;
        mz+=(float)direction/2*sin(3.14*(angleX-90)/180.0f)*0.2;
        glLoadIdentity();
        kickWall(px,pz);
        gluLookAt(mx, my, mz, mx + lx, my + ly, mz + lz, 0.0f, 1.0f, 0.0f);
    }
}

void mousemove(int x,int y)
{
    GetCursorPos(&mousePos);
    angleX-=(float)(lastmx - mousePos.x)*0.17f;
    angleY+=(float)(lastmy - mousePos.y)*0.17f;
    lastmx=mousePos.x;
    lastmy=mousePos.y;
    if(mousePos.x==0||mousePos.x>800)
    {
        lastmx=middleX;
        SetCursorPos(middleX, lastmy);
    }
    if(angleY<-360) angleY=-360;
    if(angleY> 360) angleY= 360;
    lx=100*cos(3.13149*angleX/180.0f);
	ly=angleY;
	lz=100*sin(3.13149*angleX/180.0f);
    glLoadIdentity();
	gluLookAt(mx, my, mz, mx+lx, my+ly, mz+lz, 0.0f, 1.0f, 0.0f);

}

void keyboredmove(unsigned char key,int x,int y)
{
    if(key=='w')
    {
        moveMeFlat(1);
    }
    else if(key=='d')
    {
        moveMeFlat(-2);
    }
    else if(key=='a')
    {
        moveMeFlat(2);
    }
    else if(key=='s')
    {
        moveMeFlat(-1);
    }
    else if(key==27) exit(0);
}

int main(int argc,char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(GetSystemMetrics(SM_CXSCREEN)-100, GetSystemMetrics(SM_CYSCREEN)-100);
	glutCreateWindow("");
	//glutFullScreen();

	initScenne();
	glutKeyboardFunc(keyboredmove);
	glutPassiveMotionFunc(mousemove);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMainLoop();
    return 0;
}
