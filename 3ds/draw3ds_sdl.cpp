/*
Copyright (c) 2002, Lev Povalahev
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution.
    * The name of the author may not be used to endorse or promote products 
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
THE POSSIBILITY OF SUCH DAMAGE.
*/

// http://www.uni-karlsruhe.de/~uli2

#include "../l3dsv2/l3ds.h"

#ifdef _WIN32

#pragma comment (linker, "/ENTRY:mainCRTStartup")

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "sdl.lib")
#pragma comment (lib, "sdlmain.lib")
#include <windows.h>
#endif // _WIN32

#include <SDL/SDL.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>


#define WIDTH 800
#define HEIGHT 600
#define BPP 0
//#define FULLSCREEN SDL_FULLSCREEN
#define FULLSCREEN 0

L3DS scene;

float angle;

float distance;

uint ticks;

uint frame;

float speed;

bool wireframe;

uint tris;

uint time;

void InitGL(int Width, int Height)	        
{
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)Width/(GLfloat)Height, 4.0, 5000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLfloat glfLightAmbient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat glfLightDiffuse[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat glfLightSpecular[] = {0.6f, 0.6f, 0.3f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, glfLightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, glfLightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, glfLightSpecular);
    glEnable(GL_LIGHT0);    

    glClearColor(0.5, 0.5, 0.5, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT_AND_BACK);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    //glColor3f(0.5, 0, 0);
    distance = -200;
    angle = 0;
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
}


void DrawGLScene()
{
    uint temp = SDL_GetTicks();
    frame = temp - ticks;
    ticks = temp;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    GLfloat glfLightPosition[] = {0.0, 0.0, 1.0, 0.0};
    
    glLightfv(GL_LIGHT0, GL_POSITION, glfLightPosition);

    
    distance += speed*frame;
    glTranslatef(0, 0, distance);
    glRotatef(-90, 1.0, 0.0, 0.0);
    angle += frame*.01f;
    glRotatef(angle, 0.0, 1.0, 1.0);

    angle += 0.2f;
    for (uint i= 0; i<scene.GetMeshCount(); i++)
    {
        LMesh &mesh = scene.GetMesh(i);

        glVertexPointer(4, GL_FLOAT, 0, &mesh.GetVertex(0));
        glNormalPointer(GL_FLOAT, 0, &mesh.GetNormal(0));
        glColorPointer(3, GL_FLOAT, 0, &mesh.GetBinormal(0));
        glDrawElements(GL_TRIANGLES, mesh.GetTriangleCount()*3, 
                        GL_UNSIGNED_SHORT, &mesh.GetTriangle(0));

    }
}

void ReadInput(bool &done)
{
    SDL_Event event;

    while ( SDL_PollEvent(&event) ) 
    {
        if ( event.type == SDL_QUIT ) 
        {
            done = true;
        }
        if ( event.type == SDL_KEYDOWN ) 
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                done = true;
                break;
            case SDLK_a:
                speed = 0.1f;
                break;
            case SDLK_z:
                speed = -0.1f;
                break;
            case SDLK_w:
                wireframe =! wireframe;
                if (wireframe)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDisable(GL_CULL_FACE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glEnable(GL_CULL_FACE);
                }
            default:
                break;
            }
        }
        if ( event.type == SDL_KEYUP )
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                done = true;
                break;
            case SDLK_a:
                speed = 0;
                break;
            case SDLK_z:
                speed = 0;
                break;
            default:
                break;
            }
        }
    }
}
  
int main(int argc, char *argv[]) 
{                   
    if (argc > 1)
        if (!scene.LoadFile(argv[1]))
        {
            fprintf(stderr, "Unable to open file %s\n", argv[1]);
            return -1;
        } else;
    else
    {
        fprintf(stderr, "The program requires 1 command line argument\n");
        return -2;
    } 

    //return 1;

    bool done;
    // Initialize SDL for video output 

    
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
    {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, BPP/3);
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, BPP/3);
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, BPP/3); 
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, BPP );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    if ( SDL_SetVideoMode(WIDTH, HEIGHT, BPP, SDL_OPENGL | FULLSCREEN) == NULL ) 
    {
        fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }

    SDL_WM_SetCaption("L3DS example application", NULL);

    SDL_ShowCursor(0);

    // Loop, drawing and checking events 
    InitGL(WIDTH, HEIGHT);

    done = false;
    while ( ! done ) 
    {
        DrawGLScene();
        ReadInput(done);
        SDL_GL_SwapBuffers();
    }
    SDL_Quit();
    return 0;
}

