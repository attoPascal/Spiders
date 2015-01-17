#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "freeglut.h"
#endif

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WORLD_WIDTH 200
#define WORLD_HEIGHT 150
#define BODY_RADIUS 6
#define HEAD_RADIUS 3
#define EYE_RADIUS 1
#define UPPER_LEG_WIDTH 5
#define UPPER_LEG_HEIGHT 1
#define LOWER_LEG_WIDTH 5
#define LOWER_LEG_HEIGHT 0.75
#define KNEE_ANGLE -20
#define ROTATION_ANGLE 15
#define BOUNDING_CIRCLE_RADIUS 16
#define ENEMY_SCALE_FACTOR 0.8
#define HARD_MODE_SCALING_FACTOR 0.6

#define ROTATION 0
#define X_OFFSET 1
#define Y_OFFSET 2
#define SPEED 3

#define BLACK 0
#define WHITE 1
#define GREY 2
#define RED 3
#define ORANGE 4
#define TEAL 5

// spiders: rotation, x offset, y offset, speed
int mySpider[] = { 0,   WORLD_WIDTH / 2,               BODY_RADIUS * 2,    3 };
int enemy1[]   = { 270, BODY_RADIUS * 2,               WORLD_HEIGHT * 7/8, 7 };
int enemy2[]   = { 90,  WORLD_WIDTH - BODY_RADIUS * 2, WORLD_HEIGHT * 5/8, 5 };
int enemy3[]   = { 270, BODY_RADIUS * 2,               WORLD_HEIGHT * 3/8, 2 };
int enemy4[]   = { 90,  WORLD_WIDTH - BODY_RADIUS * 2, WORLD_HEIGHT * 4/8, 4 };
int enemy5[]   = { 270, BODY_RADIUS * 2,               WORLD_HEIGHT * 6/8, 6 };

int *enemies[] = { enemy1, enemy2, enemy3, enemy4, enemy5 };

float colors[][3] = {
    { 0.0, 0.0, 0.0 }, // black
    { 1.0, 1.0, 1.0 }, // white
    { 0.7, 0.7, 0.7 }, // grey
    { 1.0, 0.0, 0.0 }, // red
    { 1.0, 0.5, 0.0 }, // orange
    { 0.0, 0.6, 0.6 }  // teal
};

int bgColor = WHITE;
int mySpiderColor = BLACK;
int enemyColor = RED;

bool showBoundingCircle = false;
bool hardModeEnabled = false;
bool gameOver = false;

void drawSquare(bool rotatable = true) {
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = rotatable ? -0.5 : 0.0;
    float y2 = rotatable ?  0.5 : 1.0;
    
    glBegin(GL_POLYGON);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawCircle(bool solid = true) {
    int numSegments = 24;
    
    glBegin(solid ? GL_POLYGON : GL_LINE_LOOP);
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float x = cosf(theta);
        float y = sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawHead(void) {
    // save matrix
    glPushMatrix();
    
    // draw head
    glScalef(HEAD_RADIUS, HEAD_RADIUS, 1);
    drawCircle();
    
    // reset
    glPopMatrix();
    glPushMatrix();
    
    // draw left eye
    glTranslatef(-HEAD_RADIUS * 0.6f, HEAD_RADIUS + EYE_RADIUS * 0.3f, 0);
    glScalef(EYE_RADIUS, EYE_RADIUS, 1);
    drawCircle();
    
    // reset
    glPopMatrix();
    glPushMatrix();
    
    // draw right eye
    glTranslatef(HEAD_RADIUS * 0.6f, HEAD_RADIUS + EYE_RADIUS * 0.3f, 0);
    glScalef(EYE_RADIUS, EYE_RADIUS, 1);
    drawCircle();
    
    // reset
    glPopMatrix();
}

void drawLeg(void) {
    // save matrix
    glPushMatrix();
    
    // draw hip joint
    glScalef(UPPER_LEG_HEIGHT / 2.0f, UPPER_LEG_HEIGHT / 2.0f, 1);
    drawCircle();
    
    // reset
    glPopMatrix();
    glPushMatrix();
    
    // draw upper leg
    glScalef(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, 1);
    drawSquare();
    
    // reset
    glPopMatrix();
    glPushMatrix();
    
    // draw knee
    glTranslatef(UPPER_LEG_WIDTH, 0, 0);
    glScalef(UPPER_LEG_HEIGHT / 2.0f, UPPER_LEG_HEIGHT / 2.0f, 1);
    drawCircle();
    
    // reset
    glPopMatrix();
    glPushMatrix();
    
    //draw lower leg
    glTranslatef(UPPER_LEG_WIDTH, 0, 0);
    glRotatef(KNEE_ANGLE, 0, 0, 1);
    glScalef(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, 1);
    drawSquare();
    
    // reset
    glPopMatrix();
}

void drawSpider(bool moveLegs = false) {
    // save matrix
    glPushMatrix();
    
    // draw body
    glScalef(BODY_RADIUS, BODY_RADIUS, 1);
    drawCircle();
    
    // reset
    glPopMatrix();
    glPushMatrix();
    
    // draw head
    glTranslatef(0, BODY_RADIUS + HEAD_RADIUS * 0.75f, 0);
    drawHead();
    
    // reset
    glPopMatrix();
    glPushMatrix();
    
    // draw legs
    for (int i = 0; i < 4; i++) {
        int legMovement1 = moveLegs ? rand() % 10 : 0;
        int legMovement2 = moveLegs ? rand() % 10 : 0;
        
        glPushMatrix();
        
        // right side
        glRotatef(45.0f - 30 * i + legMovement1, 0, 0, 1);
        glTranslatef(BODY_RADIUS, 0, 0);
        drawLeg();
        
        // reset
        glPopMatrix();
        glPushMatrix();
        
        // left side
        glScalef(-1, 1, 1);
        glRotatef(45.0f - 30 * i + legMovement2, 0, 0, 1);
        glTranslatef(BODY_RADIUS, 0, 0);
        drawLeg();
        
        // reset
        glPopMatrix();
    }
    
    if (showBoundingCircle) {
        glScalef(BOUNDING_CIRCLE_RADIUS, BOUNDING_CIRCLE_RADIUS, 1);
        drawCircle(false);
    }
    
    // reset
    glPopMatrix();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // draw background
    glColor3fv(colors[bgColor]);
    glScalef(WORLD_WIDTH, WORLD_HEIGHT, 1.0);
    drawSquare(false);
    
    // draw spider
    glLoadIdentity();
    glColor3fv(colors[mySpiderColor]);
    glTranslatef(mySpider[X_OFFSET], mySpider[Y_OFFSET], 0);
    glRotatef(mySpider[ROTATION], 0, 0, 1);
    if (hardModeEnabled) { glScalef(HARD_MODE_SCALING_FACTOR, HARD_MODE_SCALING_FACTOR, 1); }
    drawSpider();
    
    // draw enemies
    glColor3fv(colors[enemyColor]);
    
    if (!hardModeEnabled) {
        for (int i = 0; i < 3; i++) {
            int *enemy = enemies[i];
            
            glLoadIdentity();
            glTranslatef(enemy[X_OFFSET], enemy[Y_OFFSET], 0);
            glRotatef(enemy[ROTATION], 0, 0, 1);
            glScalef(ENEMY_SCALE_FACTOR, ENEMY_SCALE_FACTOR, 1);
            drawSpider(true);
        }
    }
    else {
        for (int i = 0; i < 5; i++) {
            int *enemy = enemies[i];
            
            glLoadIdentity();
            glTranslatef(enemy[X_OFFSET], enemy[Y_OFFSET], 0);
            glRotatef(enemy[ROTATION], 0, 0, 1);
            glScalef(HARD_MODE_SCALING_FACTOR, HARD_MODE_SCALING_FACTOR, 1);
            drawSpider(true);
        }
    }
    
    glFlush();
    glutSwapBuffers();
}

void move(int spider[]) {
    // calculate new coordinates
    int x = nearbyintf(spider[X_OFFSET] - sinf(3.1415926f * spider[ROTATION] / 180.0f) * spider[SPEED]);
    int y = nearbyintf(spider[Y_OFFSET] + cosf(3.1415926f * spider[ROTATION] / 180.0f) * spider[SPEED]);
    
    // check if inside borders
    if (x >= 0 && x <= WORLD_WIDTH && y >= 0 && y <= WORLD_HEIGHT) {
        // yes: move forward
        spider[X_OFFSET] = x;
        spider[Y_OFFSET] = y;
    }
    else {
        // no: reflect on border
        if (x < 0 || x > WORLD_WIDTH) {
            spider[ROTATION] = 360 - spider[ROTATION];
        } else {
            spider[ROTATION] = 180 - spider[ROTATION];
            
            if (y > WORLD_HEIGHT) {
                gameOver = true;
            }
        }
    }
    
    // check collision with player
    if (spider != mySpider && !(!hardModeEnabled && (spider == enemy4 || spider == enemy5))) {
        float distance = sqrt(powf(spider[X_OFFSET] - mySpider[X_OFFSET], 2) + powf(spider[Y_OFFSET] - mySpider[Y_OFFSET], 2));
        
        float allowedDistance;
        if (!hardModeEnabled) {
            allowedDistance = BOUNDING_CIRCLE_RADIUS * (1 + ENEMY_SCALE_FACTOR);
        } else {
            allowedDistance = 2 * BOUNDING_CIRCLE_RADIUS * HARD_MODE_SCALING_FACTOR;
        }
        
        if (distance < allowedDistance) {
            // reset mySpider to start position
            mySpider[X_OFFSET] = WORLD_WIDTH / 2;
            mySpider[Y_OFFSET] = BODY_RADIUS * 2;
            mySpider[ROTATION] = 0;
        }
    }
    
    glutPostRedisplay();
}

void moveEnemies(int value) {
    for (int i = 0; i < 5; i++) {
        int *enemy = enemies[i];
        
        if (!gameOver) {
            move(enemy);
        }
        else if (enemy[Y_OFFSET] > -BOUNDING_CIRCLE_RADIUS) {
            enemy[Y_OFFSET] -= 10;
            glutPostRedisplay();
        }
    }
    
    glutTimerFunc(50, moveEnemies, 0); // 25 fps
}

void keyboard(unsigned char key, int x, int y) {
    //printf("Characters Pressed: %c (%d)\n" , key, key);
    
    switch (key) {
        case 'a': // rotate left
            mySpider[ROTATION] += ROTATION_ANGLE;
            break;
        case 'd': // rotate right
            mySpider[ROTATION] -= ROTATION_ANGLE;
            break;
        case 'w':
            move(mySpider);
            break;
        case 'b': // toggle bounding circle
            showBoundingCircle = !showBoundingCircle;
            break;
        case 'h': // toggle hard mode
            hardModeEnabled = !hardModeEnabled;
            break;
        case '1':
            bgColor = WHITE;
            mySpiderColor = BLACK;
            break;
        case '2':
            bgColor = GREY;
            mySpiderColor = BLACK;
            break;
        case '3':
            bgColor = BLACK;
            mySpiderColor = WHITE;
            break;
        case '4':
            enemyColor = RED;
            break;
        case '5':
            enemyColor = ORANGE;
            break;
        case '6':
            enemyColor = TEAL;
            break;
        case 27: // escape
        case 'q':
            exit(0);
        default:
            break;
    }
    
    glutPostRedisplay();
}

void resize(int width, int height) {
    float worldProportion = float(WORLD_WIDTH) / float(WORLD_HEIGHT); // 4:3
    float windowProportion = float(width) / float(height);
    
    if (windowProportion < worldProportion) {
        // fit width
        float newHeight = width / worldProportion;
        glViewport( 0, height - newHeight, width, newHeight);
    } else {
        // fit height
        glViewport( 0, 0, height * worldProportion, height);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Spiders!");
    
    glClearColor(0.8, 0.8, 0.8, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WORLD_WIDTH, 0, WORLD_HEIGHT);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(50, moveEnemies, 0);
    glutReshapeFunc(resize);
    glutMainLoop();
    return 0;
}

