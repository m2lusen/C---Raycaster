#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512
#define ANGLE_MODIFIER 2
#define MOVEMENT_MODIFIER 100

#define PI 3.1415926535
#define PID2 PI/2
#define PI3D2 3*PI/2
#define DR 0.0174533 //degree for using degrees instead of radians

int mapX=8, mapY=8, mapSize=64; // define instead
int mapW[]=
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,1,1,
	1,0,1,1,0,0,0,1,
	1,0,1,1,0,0,0,1,
	1,0,0,0,0,0,1,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};


// TO DO LIST
	// trextures
	// force screen dimenstions
	// sprites

float playerX, playerY, playerDX, playerDY, playerAngle = 0; // player position

int wPressed = 0, sPressed = 0, aPressed = 0, dPressed = 0; // key presses

int prevFrameTime = 0;

void drawPlayer2d() // For 2d map
{
	glColor3f(1,1,0); // yellow
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i (playerX, playerY); // get player position
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(playerX, playerY);
	glVertex2i(playerX+playerDX*1000, playerY+playerDY*1000);
	glEnd();
}

void updatePlayerPosition() // uses keyPressed and keyReleased, so that multiple keys can be used at the same time
{ 
	int currentFrameTime = glutGet(GLUT_ELAPSED_TIME);
	float deltaTime = (currentFrameTime - prevFrameTime) / 1000.0; // Convert to seconds
    prevFrameTime = currentFrameTime;

    if (aPressed)
    {
        playerAngle -= ANGLE_MODIFIER * deltaTime;
        if (playerAngle < 0) playerAngle += 2 * PI;
        playerDX = cos(playerAngle) * MOVEMENT_MODIFIER;
        playerDY = sin(playerAngle) * MOVEMENT_MODIFIER;
    }
    if (dPressed)
    {
        playerAngle += ANGLE_MODIFIER * deltaTime;
        if (playerAngle > 2 * PI) playerAngle -= 2 * PI;
        playerDX = cos(playerAngle) * MOVEMENT_MODIFIER;
        playerDY = sin(playerAngle) * MOVEMENT_MODIFIER;
    }

 	int xOffset=0; 
	if(playerDX<0){ xOffset=-20;} 
	else{ xOffset=20;}	
 	int yOffset=0; 
	if(playerDY<0){ yOffset=-20;} 
	else{ yOffset=20;}	 
 	int initialPlayerX=playerX/64.0, initialPlayerX_add_xOffset=(playerX+xOffset)/64.0, initialPlayerX_sub_xOffset=(playerX-xOffset)/64.0;	
 	int initialPlayerY=playerY/64.0, initialPlayerY_add_yOffset=(playerY+yOffset)/64.0, initialPlayerY_sub_yOffset=(playerY-yOffset)/64.0;

    if (wPressed) { 
    	if(mapW[initialPlayerY*mapX + initialPlayerX_add_xOffset]==0){
			playerX += playerDX * deltaTime;
		}
		if(mapW[initialPlayerY_add_yOffset*mapX + initialPlayerX]==0){
			playerY += playerDY * deltaTime; 
		}
	}
    if (sPressed) { 
        if(mapW[initialPlayerY*mapX + initialPlayerX_sub_xOffset]==0){
			playerX -= playerDX * deltaTime;
		}
		if(mapW[initialPlayerY_sub_yOffset*mapX + initialPlayerX]==0){
			playerY -= playerDY * deltaTime; 
		}
	}
}

void drawMap2d() // temporary (maybee?? may be helpfull, possibly keep and enable/disable in settings later
{
	int x, y, tileX, tileY;
	for(y=0;y<mapY;y++)
	{
		for(x=0;x<mapX;x++)
		{
			if(mapW[y*mapX+x]==1){
				glColor3f(1,1,1);
			} else {
				glColor3f(0,0,0);
			}
			tileX=x*mapSize; tileY=y*mapSize;
			glBegin(GL_QUADS);
			glVertex2i(tileX + 1, tileY + 1); // +1 and -1 to add borders
			glVertex2i(tileX + 1 , tileY + mapSize - 1);
			glVertex2i(tileX + mapSize - 1, tileY + mapSize - 1);
			glVertex2i(tileX + mapSize - 1, tileY + 1);
			glEnd();
		}	
	}
}

float dist(float ax, float ay, float bx, float by, float angle) // dont need angle
{
	return(sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)));
}

void drawRays2d()
{
	int ray,mx,my,mp,dof; 
	float rayX,rayY,rayAngle,xo,yo,disT; 
	rayAngle = playerAngle -DR*30;
	if(rayAngle<0){rayAngle+=2*PI;}
	if(rayAngle>2*PI){rayAngle-=2*PI;}
	for(ray=0;ray<60;ray++)
	{	
		//Horizontal
		dof=0;
		float distanceH=99999,horizontalX=playerX,horizontalY=playerY;
		float aTan=-1/tan(rayAngle); 
		if(rayAngle>PI)
		{
		rayY=(((int)playerY>>6)<<6)-0.0001;
			rayX=(playerY-rayY)*aTan+playerX;
			yo=-64;
			xo=-yo*aTan;
		}
		if(rayAngle<PI)
		{
			rayY=(((int)playerY>>6)<<6)+64;
			rayX=(playerY-rayY)*aTan+playerX;
			yo=64;
			xo=-yo*aTan;
		}
		if (rayAngle==0 || rayAngle==PI)//looking straight left or right
		{
			rayX=playerX;
			rayY=playerY;
			dof=8;
		}

		while(dof<8){ // causing a error when angle = PI or 0
			mx=(int)(rayX)>>6; // likely with these
			my=(int)(rayY)>>6;
			mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && mapW[mp]==1)
			{
				horizontalX=rayX;
				horizontalY=rayY;
				distanceH=dist(playerX,playerY,horizontalX,horizontalY,rayAngle);
				dof=8;
			}// hit wall
			else{ // next line
				rayX+=xo;
				rayY+=yo;
				dof+=1; 
			}
		}
		//Vertical
		dof=0;
		float distanceV=99999,verticalX=playerX,verticalY=playerY;
		float nTan=-tan(rayAngle); 
		if(rayAngle>PID2 && rayAngle<PI3D2)
		{
			rayX=(((int)playerX>>6)<<6)-0.0001;
			rayY=(playerX-rayX)*nTan+playerY;
			xo=-64;
			yo=-xo*nTan;
		}
		if(rayAngle<PID2 || rayAngle>PI3D2)
		{
			rayX=(((int)playerX>>6)<<6)+64;
			rayY=(playerX-rayX)*nTan+playerY;
			xo=64;
			yo=-xo*nTan;
		}
		if (rayAngle==0 || rayAngle==PI)//looking straight up or down
		{
			rayX=playerX;
			rayY=playerY;
			dof=8;
		}

		while(dof<8){ // causing a error when angle = PI or 0
			mx=(int)(rayX)>>6; // likely with these
			my=(int)(rayY)>>6;
			mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && mapW[mp]==1)
			{
				verticalX=rayX;
				verticalY=rayY;
				distanceV=dist(playerX,playerY,verticalX,verticalY,rayAngle);
				dof=8;
			}// hit wall
			else{ // next line
				rayX+=xo;
				rayY+=yo;
				dof+=1; 
			}
		}
		// Draw 2d lines
		if(distanceV<distanceH){rayX=verticalX; rayY=verticalY; disT=distanceV;}
		if(distanceH<distanceV){rayX=horizontalX; rayY=horizontalY; disT=distanceH;}
		glColor3f(1,0,0);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(playerX,playerY);
		glVertex2i(rayX,rayY);
		glEnd();
		// Draw 3d scene
		float cAngle=playerAngle-rayAngle;//cosine angle, used to stop fish eye
		if(cAngle<0){cAngle+=2*PI;}
		if(cAngle>2*PI){cAngle-=2*PI;}
		disT=disT*cos(cAngle); // stops fish eye
		float lineH=(mapSize*320)/disT; //line height
		if(lineH>320){lineH=320;} 
		float lineO=160-lineH/2; // line Offset
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(ray*8+530,lineO);
		glVertex2i(ray*8+530,lineH+lineO);
		glEnd();
		rayAngle+=DR; // increase angle
		if(rayAngle<0){rayAngle+=2*PI;}
		if(rayAngle>2*PI){rayAngle-=2*PI;}
	}
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2d();
	drawPlayer2d();
	drawRays2d();
	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			wPressed = 1;
			break;
		case 's':
			sPressed = 1;
			break;
		case 'a':
			aPressed = 1;
			break;
		case 'd':
			dPressed = 1;
			break;
	}
}

void keyReleased(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			wPressed = 0;
			break;
		case 's':
			sPressed = 0;
			break;
		case 'a':
			aPressed = 0;
			break;
		case 'd':
			dPressed = 0;
			break;
	}
}

void init()
{
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
	playerX=300; playerY=300; playerDX = cos(playerAngle) *MOVEMENT_MODIFIER; playerDY = sin(playerAngle) *MOVEMENT_MODIFIER;
}

void idle()
{
    updatePlayerPosition();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{ 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(SCREEN_WIDTH,SCREEN_HEIGHT);
	glutCreateWindow("Raycaster Project");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyReleased); // Handle key releases
    
	glutMainLoop();
}