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
#include "Objects/Literals/Teapot.h"
#include "Objects/Properties/Mover.h"
#include "Objects/Literals/Bullet.h"
#include "Objects/Properties/HeliCam.h"
#include <vector>
#include <map>
#include <stdio.h>
#include <algorithm>

extern "C" unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
float3 gravity = float3(0, -9.8, 0);

class Helicopter : public ObjectCollection, public Mover, public HeliCam {
protected:
    float3 mainRotorOffset = float3(0, 15, 4.5);

    MeshInstance *body;
    MeshInstance *mainRotor;
    MeshInstance *tailRotor;
    double forwardTilt;
    bool playingSound = false;
    bool engineOn = false;
    bool mayToggleEngine = true;
    bool soundPlaying = false;

public:
    Helicopter(Material *material) : ObjectCollection(material) {
        Mesh *bodyMesh = new Mesh("heli.obj");
        Mesh *mainRotorMesh = new Mesh("mainrotor.obj");
        Mesh *tailRotorMesh = new Mesh("tailrotor.obj");

        body = new MeshInstance(material, bodyMesh);
        mainRotor = new MeshInstance(material, mainRotorMesh);
        tailRotor = new MeshInstance(material, tailRotorMesh);

        mainRotor->translate(float3(0, 15, 4.5f));
        tailRotor->translate(float3(.5, 15, -36));
        tailRotor->orientationAxis = (float3(1, 0, 0));
        addObject(body);
        addObject(mainRotor);
        addObject(tailRotor);
    }

    virtual void draw() {
        glPushMatrix();
        {
            ObjectCollection::draw();
            orientationAxis = float3(1, 0, 0);
            glRotatef(forwardTilt, orientationAxis.x, orientationAxis.y, orientationAxis.z);
            orientationAxis = float3(0, 1, 0);
        }
        glPopMatrix();
     }

    void move(double t, double dt) {
        if (engineOn) {
            mainRotor->orientationAngle += 20;
            tailRotor->orientationAngle += 20;

            if(!soundPlaying) {
                PlaySound(TEXT("sounds/coploop5.wav"), NULL ,SND_FILENAME | SND_ASYNC | SND_LOOP);
                soundPlaying = true;
            }
        } else {
            if(soundPlaying) {
                PlaySound(NULL, 0, SND_ASYNC);
                soundPlaying = false;
            }
        }

        applyAcceleration(t, dt);
        float3 dPosition = calcDeltaPosition(position, t, dt);
        float dOrientationAngle = calcDeltaOrientation(orientationAngle, t, dt);
        translate(dPosition);
        rotate(dOrientationAngle);
    }

    virtual bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                         std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard) {
        if (keysPressed.at('p')) {
            if (!playingSound) {
//                PlaySound(TEXT("test.wav"), NULL, SND_FILENAME | SND_ASYNC);
//                //getchar();
//                playingSound = true;
            }
            if (mayToggleEngine) {
                engineOn = !engineOn;
                mayToggleEngine = false;
            }
        } else {
            mayToggleEngine = true;
        }

        if (engineOn) {
            if (keysPressed.at('y')) {
                acceleration.y = -gravity.y + 5;
            } else if (keysPressed.at('i')) {
                acceleration.y = -gravity.y - 5;
            } else {
                acceleration.y = -gravity.y;
            }
        } else {
            acceleration.y = 0;
        }

        //Rotate
        if (keysPressed.at('h')) {
            angularAcceleration = 100;
        } else if (keysPressed.at('k')) {
            angularAcceleration = -100;
        } else {
            angularAcceleration = 0;
        }

        //x and z controls
        if (keysPressed.at('u')) {
            acceleration.x = (float) (getXDir() * 10);
            acceleration.z = (float) (getZDir() * 10);
            forwardTilt = 20;
        } else if (keysPressed.at('j')) {
            acceleration.x = (float) (-getXDir() * 10);
            acceleration.z = (float) (-getZDir() * 10);
        } else {
            acceleration.x = 0;
            acceleration.z = 0;
        }

        if (keysPressed.at(' ')) {
            Material *smoke = new TexturedMaterial("smoke1.png");
            for (double i = 0; i < M_PI * 2; i += .4) {
                Billboard *b = new Billboard(position + mainRotorOffset, smoke);
                b->scale(float3(3, 3, 3));
                b->setVelocity(float3(-cos(i) * 4, 0, sin(i) * 4));
                b->setLifeSpan(3, 2);
                spawnBillboard.push_back(b);
            }
            PlaySound(TEXT("sounds/machgun2.wav"), NULL, SND_ASYNC);
        }
        return false;
    }

    double getXDir() {
        return sin(orientationAngle * M_PI / 180);
    }

    double getZDir() {
        return cos(orientationAngle * M_PI / 180);
    }
    void setCamera(Camera& camera, int camIndex) {
        camIndex = camIndex % 2;
        if(camIndex == 1) {
                float3 eye = position;
                eye.x += (float) getXDir() * 30;
                eye.y += 10;
                eye.z += (float) getZDir() * 30;
                camera.setEye(eye);

                float3 ahead = (float3((float) getXDir(), 0, (float) getZDir())).normalize();
                camera.setAhead(ahead);
        }
    }


};

//endregion


class Scene {
    Camera camera;
    std::vector<LightSource *> lightSources;
    std::vector<Object *> objects;
    std::vector<Material *> materials;
    std::vector<Billboard *> billboards;
    Object *plane;
    HeliCam *activeHeliCam;
    int heliCamIndex = 0;


public:
    void initialize() {
        // BUILD YOUR SCENE HERE
        //region Light Sources
        lightSources.push_back(
                new DirectionalLight(
                        float3(0, 1, 0),
                        float3(1, 0.5, 1)));
        lightSources.push_back(
                new PointLight(
                        float3(-1, -1, 1),
                        float3(0.2, 0.1, 0.1)));
        //endregion

        //region Materials
        Material *yellowDiffuseMaterial = new Material();
        yellowDiffuseMaterial->kd = float3(1, 1, 0);
//        Material *tiggerTexture = new TexturedMaterial("tigger.png", GL_LINEAR);
        Material *basicMaterial = new Material();
        Material *grass = new Material();
        grass->kd = float3(0, .6, 0);

        materials.push_back(grass);
        materials.push_back(yellowDiffuseMaterial);
//        materials.push_back(tiggerTexture);
        materials.push_back(basicMaterial);
        //endregion

//        objects.push_back((new Teapot(yellowDiffuseMaterial))->translate(float3(0, -1, 0)));
//        objects.push_back((new Teapot(tiggerTexture))->translate(float3(0, -1, -2))->scale(float3(0.5, 0.5, 0.5)));
//        objects.push_back((new Teapot(basicMaterial))->translate(float3(0, 1.2, 0.5))->scale(float3(1.3, 1.3, 1.3)));
//
        TexturedMaterial* smoke = new TexturedMaterial("smoke.png");
        Helicopter* heli = new Helicopter(yellowDiffuseMaterial);
        activeHeliCam = heli;
        objects.push_back(heli);
        Bullet* bullet = new Bullet(yellowDiffuseMaterial, new Mesh("bullet.obj"));
        bullet->rotate(M_PI/2);
        bullet->setParticleMaterial(smoke);
        objects.push_back(bullet);
        Ground* ground = new Ground(grass);
        objects.push_back(ground);
    }

    void addObject(Object *object) {
        objects.push_back(object);
    }

    bool control(std::vector<bool> &keysPressed) {
        if(keysPressed.at('v'))
            heliCamIndex++;
        else if(keysPressed.at('c'))
            heliCamIndex--;

        std::vector<Object *> spawn = std::vector<Object *>();
        std::vector<Billboard *> spawnBillboard = std::vector<Billboard *>();

        for (int i = 0; i < objects.size(); i++) {
            objects.at(i)->control(keysPressed, spawn, objects, spawnBillboard);
        }

        for (int i = 0; i < spawnBillboard.size(); i++) {
            billboards.push_back(spawnBillboard.at(i));
        }
        return false;

    }

    void move(double t, double dt) {
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
        for(int i=0; i<toRemoveBillboard.size(); i++) {
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

        for (unsigned int iBill = 0; iBill < billboards.size(); iBill++)
            billboards.at(iBill)->draw(camera);
    }

};

Scene scene;
std::vector<bool> keysPressed;

void onDisplay() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
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
