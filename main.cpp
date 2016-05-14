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

#include "float2.h"
#include "float3.h"
#include "mesh.h"
#include <vector>
#include <map>
#include <stdio.h>
#include <algorithm>

extern "C" unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
float3 gravity = float3(0, -9.8, 0);


//region Light Sources

class LightSource {
public:
    virtual float3 getRadianceAt(float3 x) = 0;

    virtual float3 getLightDirAt(float3 x) = 0;

    virtual float getDistanceFrom(float3 x) = 0;

    virtual void apply(GLenum openglLightName) = 0;
};

class DirectionalLight : public LightSource {
    float3 dir;
    float3 radiance;
public:
    DirectionalLight(float3 dir, float3 radiance)
            : dir(dir), radiance(radiance) { }

    float3 getRadianceAt(float3 x) { return radiance; }

    float3 getLightDirAt(float3 x) { return dir; }

    float getDistanceFrom(float3 x) { return 900000000; }

    void apply(GLenum openglLightName) {
        float aglPos[] = {dir.x, dir.y, dir.z, 0.0f};
        glLightfv(openglLightName, GL_POSITION, aglPos);
        float aglZero[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glLightfv(openglLightName, GL_AMBIENT, aglZero);
        float aglIntensity[] = {radiance.x, radiance.y, radiance.z, 1.0f};
        glLightfv(openglLightName, GL_DIFFUSE, aglIntensity);
        glLightfv(openglLightName, GL_SPECULAR, aglIntensity);
        glLightf(openglLightName, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(openglLightName, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(openglLightName, GL_QUADRATIC_ATTENUATION, 0.0f);
    }
};

class PointLight : public LightSource {
    float3 pos;
    float3 power;
public:
    PointLight(float3 pos, float3 power)
            : pos(pos), power(power) { }

    float3 getRadianceAt(float3 x) { return power * (1 / (x - pos).norm2() * 4 * 3.14); }

    float3 getLightDirAt(float3 x) { return (pos - x).normalize(); }

    float getDistanceFrom(float3 x) { return (pos - x).norm(); }

    void apply(GLenum openglLightName) {
        float aglPos[] = {pos.x, pos.y, pos.z, 1.0f};
        glLightfv(openglLightName, GL_POSITION, aglPos);
        float aglZero[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glLightfv(openglLightName, GL_AMBIENT, aglZero);
        float aglIntensity[] = {power.x, power.y, power.z, 1.0f};
        glLightfv(openglLightName, GL_DIFFUSE, aglIntensity);
        glLightfv(openglLightName, GL_SPECULAR, aglIntensity);
        glLightf(openglLightName, GL_CONSTANT_ATTENUATION, 0.0f);
        glLightf(openglLightName, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(openglLightName, GL_QUADRATIC_ATTENUATION, 0.25f / 3.14f);
    }
};

//endregion

//region Materials

class Material {
public:
    float3 kd;            // diffuse reflection coefficient
    float3 ks;            // specular reflection coefficient
    float shininess;    // specular exponent
    Material() {
        kd = float3(0.5, 0.5, 0.5) + float3::random() * 0.5;
        ks = float3(1, 1, 1);
        shininess = 15;
    }

    virtual void apply() {
        float aglDiffuse[] = {kd.x, kd.y, kd.z, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, aglDiffuse);
        float aglSpecular[] = {kd.x, kd.y, kd.z, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, aglSpecular);
        if (shininess <= 128)
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        else
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
        glDisable(GL_TEXTURE_2D);
    }
};

class TexturedMaterial : public Material {
    GLuint texture;
public:
    TexturedMaterial(const char *imageName, GLint filtering = GL_LINEAR_MIPMAP_LINEAR) {
        unsigned char *data;
        int width;
        int height;
        int nComponents = 4;
        data = stbi_load(imageName, &width, &height, &nComponents, 0);
        if (data == NULL) {
            return;
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        if (nComponents == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else if (nComponents == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        delete data;
    }

    void apply() {
        Material::apply();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};

Material *smoke = new TexturedMaterial("smoke1.png");
Material *bullet = new TexturedMaterial("bullet2.png");
//endregion

class Camera {
    friend class Billboard;

    float3 eye;

    float3 lookAt;
    float3 right;
    float3 up;

    float fov;
    float aspect;

    float2 lastMousePos;
    float2 mouseDelta;

public:
    float3 getEye() {
        return eye;
    }

    Camera() {
        eye = float3(0, 0, -5);
        lookAt = float3(0, 0, 0);
        right = float3(1, 0, 0);
        up = float3(0, 1, 0);

        fov = 1.1;
        aspect = 1;
    }

    void apply() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fov / 3.14 * 180, aspect, 0.1, 500);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(eye.x, eye.y, eye.z, lookAt.x, lookAt.y, lookAt.z, 0.0, 1.0, 0.0);
    }

    void setAspectRatio(float ar) { aspect = ar; }

    void move(float dt, std::vector<bool> &keysPressed) {
        if (keysPressed.at('w'))
            eye += ahead * dt * 20;
        if (keysPressed.at('s'))
            eye -= ahead * dt * 20;
        if (keysPressed.at('a'))
            eye -= right * dt * 20;
        if (keysPressed.at('d'))
            eye += right * dt * 20;
        if (keysPressed.at('q'))
            eye -= float3(0, 1, 0) * dt * 20;
        if (keysPressed.at('e'))
            eye += float3(0, 1, 0) * dt * 20;

        float yaw = atan2f(ahead.x, ahead.z);
        float pitch = -atan2f(ahead.y, sqrtf(ahead.x * ahead.x + ahead.z * ahead.z));

        yaw -= mouseDelta.x * 0.02f;
        pitch += mouseDelta.y * 0.02f;
        if (pitch > 3.14 / 2) pitch = 3.14 / 2;
        if (pitch < -3.14 / 2) pitch = -3.14 / 2;

        mouseDelta = float2(0, 0);

        ahead = float3(sin(yaw) * cos(pitch), -sin(pitch), cos(yaw) * cos(pitch));
        right = ahead.cross(float3(0, 1, 0)).normalize();
        up = right.cross(ahead);

        lookAt = eye + ahead;
    }

    void startDrag(int x, int y) {
        lastMousePos = float2(x, y);
    }

    void drag(int x, int y) {
        float2 mousePos(x, y);
        mouseDelta = mousePos - lastMousePos;
        lastMousePos = mousePos;
    }

    void endDrag() {
        mouseDelta = float2(0, 0);
    }

    float3 ahead;
};

class Billboard {
protected:
    Material *material;
    double lifeSpan;
    double fadeOut;
    double transparency = 1;
    float3 position;
    float3 scaleFactor = float3(1, 1, 1);
    float3 velocity = float3();
public:
    Billboard(float3 position, Material *material) {
        this->position = position;
        this->material = material;
    }

    Billboard *translate(float3 offset) {
        position += offset;
        return this;
    }

    Billboard *scale(float3 factor) {
        scaleFactor *= factor;
        return this;
    }

    void setVelocity(float3 velocity) {
        this->velocity = velocity;
    }

    virtual void draw(Camera &camera) {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDepthMask(GL_FALSE);

        material->apply();

        glPushMatrix();

        glTranslatef(position.x, position.y, position.z);
        glScalef(scaleFactor.x, scaleFactor.y, scaleFactor.z);
        float camRotation[] = {
                camera.right.x, camera.right.y, camera.right.z, 0,
                camera.up.x, camera.up.y, camera.up.z, 0,
                camera.ahead.x, camera.ahead.y, camera.ahead.z, 0,
                0, 0, 0, 1
        };
        glMultMatrixf(camRotation);
        glScalef(4, 4, 4);
        glColor4d(1, 1, 1, transparency);
        glBegin(GL_QUADS);
        {
            glTexCoord3d(0, 0, 0);
            glVertex3d(-1, -1, 0);

            glTexCoord3d(1, 0, 0);
            glVertex3d(1, -1, 0);

            glTexCoord3d(1, 1, 0);
            glVertex3d(1, 1, 0);

            glTexCoord3d(0, 1, 0);
            glVertex3d(-1, 1, 0);
        }
        glEnd();
        glPopMatrix();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glEnable(GL_LIGHTING);
    }

    virtual bool move(double t, double dt) {
        translate(velocity * dt);
        lifeSpan -= dt;
        if(lifeSpan < fadeOut) {
            transparency = lifeSpan / fadeOut;
        }
        return lifeSpan < 0;
    }

    void setLifeSpan(double lifeSpan) {
        setLifeSpan(lifeSpan, 0);
    }

    void setLifeSpan(double lifeSpan, double transparency) {
        this->fadeOut = transparency;
        this->lifeSpan = lifeSpan;
    }

    float3 getPosition() {
        return this->position;
    }
};

//region Objects

class Object {
protected:
public:
    Material *material;
    float3 scaleFactor;
    float3 position;
    float3 orientationAxis;
    float orientationAngle;

    Object(Material *material) : material(material), orientationAngle(0.0f), scaleFactor(1.0, 1.0, 1.0),
                                 orientationAxis(0.0, 1.0, 0.0) { }

    virtual ~Object() { }

    //region Transformations

    Object *translate(float3 offset) {
        position += offset;
        return this;
    }

    Object *scale(float3 factor) {
        scaleFactor *= factor;
        return this;
    }

    Object *rotate(float angle) {
        orientationAngle += angle;
        return this;
    }

    //endregion

    virtual void applyTransformations() {
        glTranslatef(position.x, position.y, position.z);
        glRotatef(orientationAngle, orientationAxis.x, orientationAxis.y, orientationAxis.z);
        glScalef(scaleFactor.x, scaleFactor.y, scaleFactor.z);
    }

    virtual void draw() {
        material->apply();
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        {
            applyTransformations();
            drawModel();
        }
        glPopMatrix();
    }

    virtual void drawDirection() {
        float3 endArrow = position + orientationAxis * 20;
        float3 topArrow = position + orientationAxis * 10;
        topArrow.z += 1;
        topArrow.x += 1;
        topArrow.y += 1;
        glBegin(GL_TRIANGLE_FAN);
        {
            glVertex3d(position.x, position.y, position.z);
            glVertex3d(topArrow.x, topArrow.y, topArrow.z);
            glVertex3d(endArrow.x, endArrow.y, endArrow.z);

        }
        glEnd();

        endArrow = position + orientationAxis.cross(position).normalize() * 20;
        topArrow = (position + orientationAxis.cross(position).normalize() * 10) + 1;
        glBegin(GL_TRIANGLE_FAN);
        {
            glVertex3d(position.x, position.y, position.z);
            glVertex3d(topArrow.x, topArrow.y, topArrow.z);
            glVertex3d(endArrow.x, endArrow.y, endArrow.z);

        }
        glEnd();
    }

    virtual void drawModel() = 0;

    virtual void move(double t, double dt) { }

    virtual bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                         std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard) { return false; }

    virtual void drawShadow(float3 lightDir) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glTranslatef(0, .01, 0);
        glScalef(1, 0, 1);

        glTranslatef(position.x, position.y, position.z);
        glRotatef(orientationAngle, orientationAxis.x, orientationAxis.y, orientationAxis.z);
        glScalef(scaleFactor.x, scaleFactor.y, scaleFactor.z);

        drawModel();
        glPopMatrix();
    }
};

class Teapot : public Object {
public:
    Teapot(Material *material) : Object(material) { }

    void drawModel() {
        glutSolidTeapot(1.0f);
    }
};

class Ground : public Object {

public:
    Ground(Material *material) : Object(material) { }

    void drawModel() {
        material->apply();
        glBegin(GL_TRIANGLE_FAN);
        {
            glVertex3d(-100, 0, -100);
            glVertex3d(-100, 0, 100);
            glVertex3d(100, 0, 100);
            glVertex3d(100, 0, -100);
        }
        glEnd();
    }

    void drawShadow(float3 lightDir) {

    }
};

class MeshInstance : public Object {
    Mesh *mesh;
public:
    MeshInstance(Material *material, Mesh *mesh) : Object(material) {
        this->mesh = mesh;
    }

    void drawModel() {
        mesh->draw();
    }
};

class Bouncer {
protected:
    float3 velocity = float3(0, 0, 0);
    float3 acceleration = float3(0, 0, 0);
    float3 drag = float3(.6, .8, .6);
    float angularVelocity = 0.0;
    float angularAcceleration = 0.0;
    float restitution = .7;
public:

    void applyAcceleration(double t, double dt) {
        velocity += (acceleration + gravity) * dt;
        velocity *= drag.power(dt);
        angularVelocity += angularAcceleration * dt;
        angularVelocity *= pow(.8, dt);
    }

    float3 calcDeltaPosition(float3 oldPosition, double t, double dt) {
        if (oldPosition.y < 0 && velocity.y < 0) {
            velocity.y *= -restitution;
            if (abs(velocity.y) < .1) {
                velocity.y = 0;
            }
        }
        return velocity * dt;
    }

    float calcDeltaOrientation(float oldOrientation, double t, double dt) {
        return angularVelocity * dt;
    }

    bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                 std::vector<Object *> &objects) {


    }


};

class ObjectCollection : public Object {
    std::vector<Object *> objects;
public:
    ObjectCollection(Material *material) : Object(material) { }

    void draw() {
        material->apply();
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        {
            applyTransformations();
            for (unsigned int i = 0; i < objects.size(); i++) {
                objects.at(i)->draw();
            }
        }
        glPopMatrix();
    }

    void drawModel() {

    }


    void addObject(Object *newObject) {
        objects.push_back(newObject);
    }
};

class Helicopter : public ObjectCollection, public Bouncer {
protected:
    float3 mainRotorOffest = float3(0, 15, 4.5);

    MeshInstance *body;
    MeshInstance *mainRotor;
    MeshInstance *tailRotor;
    bool playingSound = false;
    bool engineOn = false;
    bool mayToggleEngine = true;

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

    void move(double t, double dt) {
        if (engineOn) {
            mainRotor->orientationAngle += 20;
            tailRotor->orientationAngle += 20;
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
            acceleration.x = (float) (sin(orientationAngle * M_PI / 180) * 10);
            acceleration.z = (float) (-cos(orientationAngle * M_PI / 180) * 10);
        } else if (keysPressed.at('j')) {
            acceleration.x = (float) (-sin(orientationAngle * M_PI / 180) * 10);
            acceleration.z = (float) (cos(orientationAngle * M_PI / 180) * 10);
        } else {
            acceleration.x = 0;
            acceleration.z = 0;
        }

        if (keysPressed.at(' ')) {
            Material *smoke = new TexturedMaterial("smoke1.png");
            for (double i = 0; i < M_PI * 2; i += .4) {
                Billboard *b = new Billboard(position + mainRotorOffest, smoke);
                b->setVelocity(float3(-cos(i) * 4, 0, sin(i) * 4));
                b->setLifeSpan(3, 2);
                spawnBillboard.push_back(b);
            }
        }
        return false;
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
        Material *tiggerTexture = new TexturedMaterial("tigger.png", GL_LINEAR);
        Material *basicMaterial = new Material();
        Material *grass = new Material();
        grass->kd = float3(0, .8, 0);

        materials.push_back(yellowDiffuseMaterial);
        materials.push_back(tiggerTexture);
        materials.push_back(basicMaterial);
        //endregion

        objects.push_back((new Teapot(yellowDiffuseMaterial))->translate(float3(0, -1, 0)));
        objects.push_back((new Teapot(basicMaterial))->translate(float3(0, 1.2, 0.5))->scale(float3(1.3, 1.3, 1.3)));
        objects.push_back((new Teapot(tiggerTexture))->translate(float3(0, -1, -2))->scale(float3(0.5, 0.5, 0.5)));
        objects.push_back((new Ground(grass)));

        Mesh *heliMesh = new Mesh("heli.obj");
        Mesh *mainRotorMesh = new Mesh("mainrotor.obj");
        Mesh *tailRotorMesh = new Mesh("tailrotor.obj");

        Object *heli = new MeshInstance(yellowDiffuseMaterial, heliMesh);
        Object *mainRotor = new MeshInstance(yellowDiffuseMaterial, heliMesh);
        Object *tailRotor = new MeshInstance(yellowDiffuseMaterial, heliMesh);

//        Object *bouncingHeli = new Bouncer(yellowDiffuseMaterial, heliMesh);
//        bouncingHeli->translate(float3(0, 15, 0));
//        objects.push_back(bouncingHeli);

        Material *asteroid = new TexturedMaterial("smoke1.png");
        billboards.push_back(new Billboard(float3(15, 10, 15), asteroid));
        billboards.push_back(new Billboard(float3(16, 12, 15), asteroid));
        billboards.push_back(new Billboard(float3(15, 8, 16), asteroid));
        billboards.push_back(new Billboard(float3(15, 16, 15), asteroid));

        objects.push_back(new Helicopter(yellowDiffuseMaterial));
//        objects.push_back(heli);
//        objects.push_back(mainRotor);
//        objects.push_back(tailRotor);
    }

    void addObject(Object *object) {
        objects.push_back(object);
    }

    bool control(std::vector<bool> &keysPressed) {
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
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

        camera.apply();
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

    Object *getPlane();
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

Object *Scene::getPlane() {
    return plane;
}
