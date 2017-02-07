#define _USE_MATH_DEFINES

#include <math.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// Needed on MsWindows
#include <windows.h>

#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// Download glut from: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>

#include "Utility/float2.h"
#include "Utility/float3.h"
#include "Objects/Mesh.h"
#include "Materials/Material.h"
#include "Objects/General/Object.h"
#include "Materials/TexturedMaterial.h"
#include "Camera.h"
#include "Objects/General/ObjectCollection.h"
#include "Objects/General/MeshInstance.h"
#include "Lighting/PointLight.h"
#include "Lighting/DirectionalLight.h"
#include "Objects/Literals/Ground.h"
#include "Objects/Properties/Mover.h"
#include "Objects/Literals/Helicopter.h"
#include "Objects/Literals/UFO.h"
#include <vector>
#include <map>
#include <stdio.h>
#include <algorithm>
#include <time.h>

extern "C" unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);

float3 gravity = float3(0, -9.8, 0);

class Scene {
    Camera camera;
    int dog = (int) M_PI;
    std::vector<LightSource *> lightSources;
    std::vector<Object *> objects;
    std::vector<Material *> materials;
    std::vector<Billboard *> billboards;
    Helicopter *helicopter;
    HeliCam *activeHeliCam;
    Mesh* ufoMesh;
    Material * ufoMaterial;
    int heliCamIndex = 0;
    bool heliButtonReleased = true;
    double timeSinceUFOSpawn = 0;
    double ufoSpawnDelay = 10;
    double ufoSpawnDelayBase = 10;
    double ufoSpeed = 8;
    double ufosDestroyed = 0;


public:
    void initialize() {
        // BUILD YOUR SCENE HERE
        //region Light Sources
        lightSources.push_back(
                new DirectionalLight(
                        float3(0, 1, 0),
                        float3(1, 1, 1)));
        lightSources.push_back(
                new PointLight(
                        float3(-1, -1, 1),
                        float3(0.2, 0.1, 0.1)));
        //endregion

        //region Materials
        Material *yellowDiffuseMaterial = new Material();
        yellowDiffuseMaterial->kd = float3(1, .65, 0);

        Material* ufoSilver = new Material();
        ufoSilver->kd = float3(.6, .6, .6);
        ufoMaterial = ufoSilver;

        Material *grass = new Material();
        grass->kd = float3(0, .6, 0);

        materials.push_back(ufoSilver);
        materials.push_back(grass);
        materials.push_back(yellowDiffuseMaterial);
        //endregion

        TexturedMaterial *smoke = new TexturedMaterial("smoke1.png");
        Mesh *bodyMesh = new Mesh("heli.obj");
        Mesh *mainRotorMesh = new Mesh("mainrotor.obj");
        Mesh *tailRotorMesh = new Mesh("tailrotor.obj");
        helicopter = new Helicopter(yellowDiffuseMaterial, bodyMesh, mainRotorMesh, tailRotorMesh);
        helicopter->translate(float3(0, 10, 0));
        activeHeliCam = helicopter;
        heliCamIndex = 2;
        objects.push_back(helicopter);
        Ground *ground = new Ground(grass);
        objects.push_back(ground);

        ufoMesh = new Mesh("Flying Disk/flying Disk flying.obj");
        UFO* ufo = createUFO(float3(0, 150, 0));
        objects.push_back(ufo);
    }

    UFO* createUFO(float3 position) {
        UFO* ufo = new UFO(ufoMaterial, ufoMesh);
        //ufo->translate(float3(0, 150, 0));
        ufo->position = position;
        ufo->scale(float3(.05, .05, .05));
        ufo->setTarget(helicopter);
        return ufo;
    }

    void addObject(Object *object) {
        objects.push_back(object);
    }

    bool control(std::vector<bool> &keysPressed) {
        if (keysPressed.at('v')) {
            if (heliButtonReleased) {
                heliCamIndex++;
                heliButtonReleased = false;
            }
        } else if (keysPressed.at('c')) {
            if (heliButtonReleased) {
                heliCamIndex--;
                heliButtonReleased = false;
            }
        } else {
            heliButtonReleased = true;
        }
        std::vector<Object *> spawn = std::vector<Object *>();
        std::vector<Billboard *> spawnBillboard = std::vector<Billboard *>();
        std::vector<int> deadIndices = std::vector<int>();

        if(timeSinceUFOSpawn > ufoSpawnDelay) {
            ufoSpawnDelay = ufoSpawnDelayBase - sqrt(ufosDestroyed * 3);
            timeSinceUFOSpawn = 0;


            UFO * newUFO = createUFO(float3(rand() % 600 - 300, rand() % 150 + 20, rand() % 600 - 300));
            newUFO->setSpeed(ufoSpeed + sqrt(ufosDestroyed * 5));
            spawn.push_back(newUFO);
        }

        for (unsigned int i = 0; i < objects.size(); i++) {
            bool dead = objects.at(i)->control(keysPressed, spawn, objects, spawnBillboard);
            if(dead)
                deadIndices.push_back(i);
        }

        for (unsigned int i=0; i<deadIndices.size(); i++) {
            int index = deadIndices.at(i) - i;
            Object*o = objects.at(index);
            if(UFO * u = dynamic_cast<UFO*>(o)) {
                ufosDestroyed++;
            }
            delete objects.at(index);
            objects.erase(objects.begin() + index);
        }

        for (unsigned int i = 0; i < spawn.size(); i++) {
            objects.push_back(spawn.at(i));
        }

        for (unsigned int i = 0; i < spawnBillboard.size(); i++) {
            billboards.push_back(spawnBillboard.at(i));
        }
        return false;

    }

    void move(double t, double dt) {
        timeSinceUFOSpawn += dt;
        for (int i = 0; i < objects.size(); i++) {
            objects.at(i)->move(t, dt);
        }
        std::vector<int> toRemoveBillboard = std::vector<int>();
        for (int i = 0; i < billboards.size(); i++) {
            bool remove = billboards.at(i)->move(t, dt);
            if (remove) {
                toRemoveBillboard.push_back(i);
            }
        }
        for (int i = 0; i < toRemoveBillboard.size(); i++) {
            int index = toRemoveBillboard.at(i) - i;
            delete billboards.at(index);
            billboards.erase(billboards.begin() + index);
        }

        struct CameraDepthComparator {
            float3 ahead;

            bool operator()
                    (Billboard *a, Billboard *b) {
                return ((a->getPosition().dot(ahead)) > (b->getPosition().dot(ahead) + 0.01));
            }
        } comp = {camera.ahead};
        std::sort(billboards.begin(), billboards.end(), comp);
    }

    ~Scene() {
        for (std::vector<LightSource *>::iterator iLightSource = lightSources.begin();
             iLightSource != lightSources.end(); ++iLightSource)
            delete *iLightSource;
        for (std::vector<Material *>::iterator iMaterial = materials.begin(); iMaterial != materials.end(); ++iMaterial)
            delete *iMaterial;
        for (std::vector<Object *>::iterator iObject = objects.begin(); iObject != objects.end(); ++iObject)
            delete *iObject;
    }

public:
    Camera &getCamera() {
        return camera;
    }

    void draw() {
        camera.apply();
        activeHeliCam->setCamera(camera, heliCamIndex);

        unsigned int iLightSource = 0;
        for (; iLightSource < lightSources.size(); iLightSource++) {
            glEnable(GL_LIGHT0 + iLightSource);
            lightSources.at(iLightSource)->apply(GL_LIGHT0 + iLightSource);
        }
        for (; iLightSource < GL_MAX_LIGHTS; iLightSource++)
            glDisable(GL_LIGHT0 + iLightSource);

        for (unsigned int iObject = 0; iObject < objects.size(); iObject++)
            objects.at(iObject)->draw();

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3d(0.0, 0.0, 0.0);
        for (unsigned int iObject = 0; iObject < objects.size(); iObject++) {
            objects.at(iObject)->drawShadow(float3());
        }
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

        for (unsigned int iBill = 0; iBill < billboards.size(); iBill++) {
            billboards.at(iBill)->draw(camera);
        }
    }

};

Scene scene;
std::vector<bool> keysPressed;

void onDisplay() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
//    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen

    scene.draw();

    glutSwapBuffers(); // drawing finished
}

void onIdle() {
    double t = glutGet(GLUT_ELAPSED_TIME) * 0.001;            // time elapsed since starting this program in msec
    static double lastTime = 0.0;
    double dt = t - lastTime;
    lastTime = t;

    scene.getCamera().move(dt, keysPressed);
    scene.control(keysPressed);
    scene.move(t, dt);
    glutPostRedisplay();
}

//region Input Events

void onKeyboard(unsigned char key, int x, int y) {
    keysPressed.at(key) = true;
}

void onKeyboardUp(unsigned char key, int x, int y) {
    keysPressed.at(key) = false;
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) if (state == GLUT_DOWN)
        scene.getCamera().startDrag(x, y);
    else
        scene.getCamera().endDrag();
}

void onMouseMotion(int x, int y) {
    scene.getCamera().drag(x, y);
}
//endregion

void onReshape(int winWidth, int winHeight) {
    glViewport(0, 0, winWidth, winHeight);
    scene.getCamera().setAspectRatio((float) winWidth / winHeight);
}

int main(int argc, char **argv) {
    srand (time(NULL));

    glutInit(&argc, argv);                        // initialize GLUT
    glutInitWindowSize(600, 600);                // startup window size
    glutInitWindowPosition(100, 100);           // where to put window on screen
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // 8 bit R,G,B,A + double buffer + depth buffer

    glutCreateWindow("OpenGL teapots");                // application window is created and displayed

    glViewport(0, 0, 600, 600);

    glutDisplayFunc(onDisplay);                    // register callback
    glutIdleFunc(onIdle);                        // register callback
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMouseMotion);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    scene.initialize();
    for (int i = 0; i < 256; i++)
        keysPressed.push_back(false);

    glutMainLoop();                                // launch event handling loop

    return 0;
}
