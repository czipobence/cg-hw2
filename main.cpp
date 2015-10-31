#define _USE_MATH_DEFINES
#include <math.h>
//#include <stdlib.h>
#include <stdio.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

///PLÁGIUM
struct Vector {
    float x, y, z, k;
    Vector() { x = y = z = k = 0; }
    Vector(float x0, float y0, float z0 = 0, float k0 = 0) { x = x0; y = y0; z = z0; k = k0;}
    Vector operator*(float a) { return Vector(x * a, y * a, z * a, k * a); }
    Vector operator+(const Vector& v) { return Vector(x + v.x, y + v.y, z + v.z, k + v.k); }
    Vector operator-(const Vector& v) { return Vector(x - v.x, y - v.y, z - v.z, k - v.k); }
    float operator*(const Vector& v) { return (x * v.x + y * v.y + z * v.z + k*v.k); }
    Vector operator%(const Vector& v) { return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x, 0); }
    float Length() { return sqrt(x * x + y * y + z * z); }
    Vector norm(){
        float h = this->Length();
        *this = (*this) * (1.0 / h);
        return *this;
    }
};
///PLÁGIUM
struct Color {
    float r, g, b;
    Color() { r = g = b = 0; }
    Color(float r0, float g0, float b0) { r = r0; g = g0; b = b0; }
    Color operator*(float a) { return Color(r * a, g * a, b * a); }
    Color operator*(const Color& c) { return Color(r * c.r, g * c.g, b * c.b); }
    Color operator+(const Color& c) { return Color(r + c.r, g + c.g, b + c.b); }
    Color operator-(const Color& c) { return Color(r - c.r, g - c.g, b - c.b); }
    Color operator/(const Color& c) {return Color(r / c.r, g / c.g, b / c.b);}
};

///PLÁGIUM
const int screenWidth = 600;
const int screenHeight = 600;
Color image[screenWidth][screenHeight];

class Ray{public: Vector kozep, irany;}; // hogy pont vagy vektor, azt a vektor osztaly negyedik valtozoja adja meg (1 pont 0 vektor)
///PLÁGIUM
class HitRec{
public:
    Vector mPont, fNorm;
    float t;
    int indx;

    HitRec(){
        t = indx = -1;
        mPont = Vector(-1,-1,-1);
        fNorm = Vector(-1,-1,-1);
        }
};
///PLÁGIUM
class Object{ // material is itt van megvalositva
public:
    Color kd, ks, n, k; // k: elnyelesi tenyezo, n: toresmutato, kd: anyag eredeti szine, ks: anyag spekularis szine (mennyire tukrozo a felulet)
    float shine;
    Color maxColor;
///PLÁGIUM
    bool toro, tukrozo; // arany ezust: csak tukrozo, uveg: toro, tukrozo
///PLÁGIUM
    Object(){toro = tukrozo = false; maxColor = Color(1,1,1);};
///PLÁGIUM
    virtual HitRec intersect(Ray r){}
///PLÁGIUM
    // beesesi szog meghatarozasa: -1 * beerkezo_sugar_iranyvektora * felulet_normalvektora
    // azert eleg osszeszorozni a kettot, mert mindketto egysegvektor
    Color fresnel(float s){ // beesesi szoget kapja parameterul, az elnyelodes merteket hatarozza meg
        float d = cos(s);
        Color F0 = ((n-maxColor)*(n-maxColor)+(k*k))/((n+maxColor)*(n+maxColor)+(k*k));
        return F0+(maxColor-F0)*pow(1-d,5.0);

    }
};
///PLÁGIUM
class Feny{
public:
    Vector p;
    Color c;
    float power;

    Feny(float power = 400){this->power = power;}
///PLÁGIUM
    Color getColor(Vector M){ // adott pontban mekkora a feny intenzitasa
        Vector l = M - p;
        return c * (1.0 / (l.Length() * l.Length())) * power; // negyzetesen csokkeno fugges
    }
};

//LE NE MÁSOLD EZT A KAMERÁT, MERT TUTI, H PLÁGIUM!!!!!!!
///PLÁGIUM
class  Kamera{
public:
///PLÁGIUM
    Vector eye, lookat, up, right;
///PLÁGIUM
///PLÁGIUM
///PLÁGIUM
///PLÁGIUM
///PLÁGIUM
    Kamera(){

        eye = Vector(-1,11,-12); // y kulonbozik, hogy ferden nezzen az asztalra
        lookat = Vector(-1,9,-8); // z kulonbozik, hogy legyen tavolsag az eye es az ernyo kozott
        up = Vector(0,0,-1);
        Vector dir = lookat-eye;
        dir.norm();
        right = dir%up;
        right.norm();
        up=dir%right; // itt kapja meg az erteket az up
        up.norm();
///PLÁGIUM
        up = up*(5/up.Length()); // ernyo novelese
        right = right * (5/right.Length());
    }
///PLÁGIUM
    Ray getRay(int x, int y){ // x es y az image tomb indexelese. opengl oszlopfolytonosan tolti fel a tombot.
        Ray ray;

        float XM = 600.0;
        float YM = 600.0;

        Vector p = lookat + (right * (2*(x+0.5)/XM-1)) + // diasorbol. a +0.5 azert van, hogy a pixel kozepet celozza
                            (up * (2*(y+0.5)/YM-1));

        ray.irany = p - eye;
        ray.kozep = eye;
        ray.irany.norm();

        return ray;
    }
};

float epszilon = 0.1;
///PLÁGIUM
class Sik: public Object{
public:
    Vector pont, normal;

    Sik(){normal = Vector(0,1,0);}
///PLÁGIUM
    HitRec intersect(Ray r){
       HitRec dummy;
       dummy.t = -1;
       r.kozep.k = 1;
       r.irany.k = 0;

       double parhuzamosFutas = r.irany * normal;

       if(parhuzamosFutas == 0) return dummy; // nem lesz metszespont, mert parhuzamos a sikkal a sugar

       float t = (normal*(r.irany-pont))*(-1.0/parhuzamosFutas); // diasorbol

       if(t > epszilon){

            dummy.mPont = r.kozep + r.irany * t;

            if(dummy.mPont.x > 50 || dummy.mPont.x < -50 || dummy.mPont.z > 40 || dummy.mPont.z < -15)
                return dummy;

            dummy.fNorm = getNormal();

            dummy.t = t;

            return dummy;
       }
       return dummy;

    }
///PLÁGIUM
    Vector getNormal(){
        return normal.norm();
    }
};
///PLÁGIUM
class Scene{
public:
    Object* testek[50];
    int szam;
    Kamera kamera;
    Feny light1, light2, light3;

    Scene(){szam = 0;}


    float myMAX(float a, float b){
        return (a > b) ? a : b;
    }
///PLÁGIUM
///PLÁGIUM
///PLÁGIUM
///PLÁGIUM
///PLÁGIUM
    Color directLightSource(Feny l, Ray r, HitRec h){ // arnyekot vet vagy színt szamol
        Ray shadow;
        shadow.irany = l.p - h.mPont;
        shadow.irany.norm();
        shadow.kozep = h.mPont + shadow.irany*epszilon;

        Vector V = r.irany * (-1); // kameraba mutato vektor, beerkezo sugar -1 szerese
        V.norm();
        Vector L = shadow.irany; // feny fele mutato vektor
        L.norm();
        Vector H = (L + V); // V es L kozotti felezovektor
        H.norm();

        HitRec sM = intersectAll(shadow);

        Vector tav_feny_metszpont = h.mPont - l.p;
        Vector tav_2_metszespont = h.mPont - sM.mPont;

        if(h.indx == 0){ // ha az asztalt metszettuk akkor a mintazatot itt allitjuk be. ez lesz a proceduralis textura
            if(abs(int(h.mPont.x)%11) > 0 && abs(int(h.mPont.x)%10) < 3.4) {
                    testek[h.indx]->kd = Color(0.0,0.1,0.5);
            } else {
                    testek[h.indx]->kd = Color(1.0,0.88,0.0);
            }
        }

        if(sM.indx < 0 || (tav_2_metszespont.Length() + epszilon) > tav_feny_metszpont.Length()){
            return l.getColor(h.mPont) * (testek[h.indx]->kd * myMAX(0, L * h.fNorm) + testek[h.indx]->ks * pow(myMAX(0, H * h.fNorm), testek[h.indx]->shine));
            // fenyintenzitas a metszespontban
        }
        else
            return Color(0,0,0);
    }
///PLÁGIUM - jó, ezt nagyon nem lehet máshogy xd xd
    void render(){  // feltolti az image tombot a pixelenkenti szinnel
        for (int x = 0; x < 600; x++)
            for (int y = 0; y < 600; y++) {
                Ray ray = kamera.getRay(y,x);
                image[x][y] = trace(ray, 0);
        }
    }
///PLÁGIUM
    HitRec intersectAll(Ray ray){ //
        HitRec dummy;
        dummy.t = 1000000; // azert ekkora, mert mivel en terveztem a szinteret, tudom, hogy ennel nagyobb nem lesz. ennel kisebb is lehetne
        HitRec m;
///PLÁGIUM
        for(int i=0; i<szam; i++){
            m = testek[i]->intersect(ray);
            if((m.t > epszilon) && (m.t < dummy.t)){
                    dummy = m;
                    dummy.indx = i;
            }
        }
        return dummy;
    }
///PLÁGIUM
    Ray reflactSugar(Ray ray, HitRec dummy){
        Ray r;
        r.irany = dummy.fNorm*( -2.0 * (ray.irany * dummy.fNorm)) + ray.irany;
        r.irany.norm();
        r.kozep = dummy.mPont + r.irany*epszilon;
        return r;
    }
///PLÁGIUM
    Ray refract(Vector I, Vector N, float n) {
        N = N.norm();
        I = I.norm();
        Ray r;
        r.irany = Vector(0,0,0);
        float s = (N*I)*(-1);

        if(s < epszilon) { // testen belul van
            s = -s;
            N = N*(-1);
            n = 1 / n;
        }

        float teljesVissza = 1 - (1 - s * s/n/n);

        if (teljesVissza < 0) // ha negativ akkor teljes visszaverodes van
            return r;

        r.irany = (I*(1/n))+  (N  * (s / n - sqrt(teljesVissza))); // ha nincs teljes visszaverodes akkor a tort sugar iranyat kell meghatarozni
        r.irany.norm();

        return r;
    }
///PLÁGIUM
    Color trace(Ray& ray, int d){
        Color color(0,0,0);

        if(d > 4)
            return color;

        d++;
///PLÁGIUM
        HitRec Near = intersectAll(ray);
        if(Near.indx < 0)  return Color(0.255,0.89,0.98); // az eget talaltuk el, kekkel terunk vissza


        color = color + directLightSource(light1,ray,Near) + directLightSource(light2,ray,Near) + directLightSource(light3,ray,Near); // arnyek szamitas

        if(testek[Near.indx]->tukrozo){  // tukrozodes szamitas
            Ray refRay = reflactSugar(ray,Near);
            color = color + testek[Near.indx]->fresnel(-1.0 * (ray.irany.norm()*Near.fNorm.norm())) * trace(refRay,d); // fresnel megmondja mennyi nyelodik el a becsapodas soran, tehat mennyivel csokken az intenzitas
        }

///PLÁGIUM
        if(testek[Near.indx]->toro){ // tores szamiztas, az uveg miatt eleg egyszer. ha mas anyag lenne r, g, b komponensenkent kellene
            float n = testek[Near.indx]->n.r;
            Ray refractR = refract(ray.irany,Near.fNorm,n); // a megtort sugarat adja vissza, azonban ha a sugarnak az iranyvektora csupa 0, akkor teljes visszaverodes tortent

            if(refractR.irany.Length() > epszilon){ // ezert csak akkor szamolunk torest, ha az iranvvektor hossza nagyobb mint epszilon
                refractR.kozep = Near.mPont + refractR.irany * epszilon;
                color = color + (Color(1,1,1)-testek[Near.indx]->fresnel(-1.0 * (ray.irany.norm()*Near.fNorm.norm()))) * trace(refractR,d);
            }
        }
        return color;
    }
///PLÁGIUM

///PLÁGIUM

///PLÁGIUM

///PLÁGIUM
    void fenyek(){
        light1.power = 459;
        light3.power = 463;
        light2.power = 358;

        light1.p = Vector(5,25,5);
        light2.p = Vector(-15,15,15);
        light3.p = Vector(9,10,-10);

        light1.c = Color(0.8,0,0);
        light2.c = Color(0,0.9,0);
        light3.c = Color(0,0,0.85);

    }

///PLÁGIUM
    void build(){
        fenyek();
        Sik sik;

        sik.kd = Color(0.1,0.4,0.05);
        sik.shine = 0.1;
        sik.pont = Vector(0,-11,0);
        sik.tukrozo = false;
        sik.toro = false;
        sik.ks = Color(0.3, 0.5, 0.1);
        testek[szam++] = &sik;
///PLÁGIUM
///PLÁGIUM
        render();
    }

};



Scene s; // szinter peldanya
void onInitialization(){
    glViewport(0, 0,screenWidth, screenHeight);

    s.build();
}


void onDisplay() {
    glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image); // itt rajzolja ki a kepet a tracelt pixelekbol

    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {}
void onKeyboardUp(unsigned char key, int x, int y) {}
void onMouse(int button, int state, int x, int y) {}
void onMouseMotion(int x, int y){}
void onIdle() {}
// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv);                 // GLUT inicializalasa
    glutInitWindowSize(600, 600);            // Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);            // Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");        // Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);                // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);            // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();                    // Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);                // Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();                    // Esemenykezelo hurok

    return 0;
}
