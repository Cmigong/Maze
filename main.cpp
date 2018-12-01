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
#include <mmsystem.h>

using namespace std;

struct walllist
{
    float point1x;
    float point1z;
    float point2x;
    float point2z;
};

static float angleX = 0.0, angleY = 0.0, rati;
static float mx = 7.0f, my = 2.5f, mz = 0.0f,lx = 0.0f, ly = 0.0f, lz = -1.0f,px=mx,pz=mz;
static GLfloat sun=0.9;
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
    GLfloat spot[] = {mx, my,mz,1};
    GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat spot_position[] = {mx+lx, my+ly, mz+lz};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,ambientLight);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
    glLightfv(GL_LIGHT0,GL_POSITION,spot);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,spot_position);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,90.0f);
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,80);
    glEnable(GL_LIGHT0);
}

void changeSize(int w, int h)
{
	if(h==0)  h=1;
	rati=1.0f*w/h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
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
	//3----------------------------
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
	//4----------------------------
	glPopMatrix();
}

GLuint createDL()
{
	GLuint WallDL=glGenLists(1);
	glNewList(WallDL, GL_COMPILE);
	drawWall();
	glEndList();
	return (WallDL);
}

bool LoadT81(char *filename, GLuint &texture)
{
    AUX_RGBImageRec *pImage=NULL;
	pImage=auxDIBImageLoad(filename);
	if(pImage == NULL) return false;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D,texture);
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
			glTranslatef(i*14, 0, j*14);
			glCallList(wall_display_list);
			glPopMatrix();
		}
    }
    for(float i=-55;i<=35;i+=5)
    {
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);glVertex3f(-50, 0, i);
        glTexCoord2f(1.0f,0.0f);glVertex3f(-50, 5, i);
        glTexCoord2f(1.0f,1.0f);glVertex3f(-50, 5, i+5);
        glTexCoord2f(0.0f,1.0f);glVertex3f(-50, 0, i+5);
        glEnd();
    }
    for(float i=-55;i<=35;i+=5)
    {
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);glVertex3f(36, 0, i);
        glTexCoord2f(1.0f,0.0f);glVertex3f(36, 5, i);
        glTexCoord2f(1.0f,1.0f);glVertex3f(36, 5, i+5);
        glTexCoord2f(0.0f,1.0f);glVertex3f(36, 0, i+5);
        glEnd();
    }
    for(float i=-55;i<=35;i+=5)
    {
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);glVertex3f(i, 0,-50);
        glTexCoord2f(1.0f,0.0f);glVertex3f(i, 5,-50);
        glTexCoord2f(1.0f,1.0f);glVertex3f(i+5, 5,-50);
        glTexCoord2f(0.0f,1.0f);glVertex3f(i+5, 0,-50);
        glEnd();
    }
    for(float i=-55;i<=35;i+=5)
    {
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);glVertex3f(i, 0, 36);
        glTexCoord2f(1.0f,0.0f);glVertex3f(i, 5,36);
        glTexCoord2f(1.0f,1.0f);glVertex3f(i+5, 5,36);
        glTexCoord2f(0.0f,1.0f);glVertex3f(i+5, 0,36);
        glEnd();
    }
}

void recordWall()
{
    for(int i=-3;i<3;i++)
    {
        for(int j=-3;j<3;j++)
        {
            wall[wallnum].point1x=(i*14-5);
			wall[wallnum].point1z=(j*14-5);
			wall[wallnum].point2x=(i*14+5);
			wall[wallnum].point2z=(j*14+5);
			wallnum++;
        }
    }
}

void initScenne()
{
    char str[]="data/images/road.bmp";
    LoadT81(str,g_cactus[0]);
    char str1[]="data/images/0RBack.bmp";
    LoadT81(str1,g_cactus[2]);
    char str2[]="data/images/0Front.bmp";
	LoadT81(str2,g_cactus[3]);
	char str3[]="data/images/0Top.bmp";
	LoadT81(str3,g_cactus[4]);
	char str4[]="data/images/0Left.bmp";
	LoadT81(str4,g_cactus[5]);
	char str5[]="data/images/0Right.bmp";
	LoadT81(str5,g_cactus[6]);
	char str6[]="data/images/wall.bmp";
	LoadT81(str6,g_cactus[1]);
	char str7[]="data/images/ceiling.bmp";
	LoadT81(str7,g_cactus[7]);
    glEnable(GL_TEXTURE_2D);
    SetCursorPos(middleX, middleY);
    ShowCursor(false);
	glEnable(GL_DEPTH_TEST);
	wall_display_list=createDL();
	glEnable(GL_LIGHTING);
	GLfloat ambientLight[]={sun, sun, sun, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
	recordWall();
	sndPlaySound("data/images/background.wav",SND_ASYNC||SND_LOOP);
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	light0();
    CreateSkyBox();
    texture(g_cactus[0]);
    for(float i=-60;i<=60;i+=10)
    {
        for(float j=-60;j<=60;j+=10)
        {
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);glVertex3f(i, 0.0f, j);
            glTexCoord2f(1.0f,0.0f);glVertex3f(i, 0.0f, j+10);
            glTexCoord2f(1.0f,1.0f);glVertex3f(i+10, 0.0f, j+10);
            glTexCoord2f(0.0f,1.0f);glVertex3f(i+10, 0.0f, j);
            glEnd();
        }
    }
    texture(g_cactus[7]);
    for(float i=-60;i<=60;i+=10)
    {
        for(float j=-60;j<=60;j+=10)
        {
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);glVertex3f(i, 5.0f, j);
            glTexCoord2f(1.0f,0.0f);glVertex3f(i, 5.0f, j+10);
            glTexCoord2f(1.0f,1.0f);glVertex3f(i+10, 5.0f, j+10);
            glTexCoord2f(0.0f,1.0f);glVertex3f(i+10, 5.0f, j);
            glEnd();
        }
    }
	setWall();
    glutSwapBuffers();
}

bool isWall(float x,float z)
{
    for(int i=0;i<wallnum;i++)
    {
        if(wall[i].point1x-x<1.3&&wall[i].point2x-x>-1.3&&z-wall[i].point1z>-1.3&&wall[i].point2z-z>-1.3) return true;
    }
    if(x<-48.7||x>34.7||z<-48.7||z>34.7) return true;
    return false;
}

void kickWall(float x,float z)
{
    float t;
    if(isWall(mx,mz))
    {
        t=mx;
        mx=x;
        if(!isWall(mx,mz)) return;
        else if(t>-48.7&&t<34.7) mx=t;
        t=mz;
        mz=z;
        if(!isWall(mx,mz)) return;
        else if(t>-48.7&&t<34.7) mz=t;
    }
}

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
    //sndPlaySound("data/images/selffoot.wav",SND_ASYNC);
}

void mousemove(int x,int y)
{
    GetCursorPos(&mousePos);
    angleX-=(float)(lastmx - mousePos.x)*0.17f;
    angleY+=(float)(lastmy - mousePos.y)*0.17f;
    lastmx=mousePos.x;
    lastmy=mousePos.y;
    if(mousePos.x<200||mousePos.x>800)
    {
        lastmx=middleX;
        SetCursorPos(middleX, lastmy);
    }
    if(mousePos.y<200||mousePos.y>400)
    {
        lastmy=middleY;
        SetCursorPos(lastmx, middleY);
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
    if(key=='w'||key=='W')
    {
        moveMeFlat(1);
    }
    else if(key=='d'||key=='D')
    {
        moveMeFlat(-2);
    }
    else if(key=='a'||key=='A')
    {
        moveMeFlat(2);
    }
    else if(key=='s'||key=='S')
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
