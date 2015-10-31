//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fĂźggvĂŠnyt kivĂŠve, a lefoglalt adat korrekt felszabadĂ­tĂĄsa nĂŠlkĂźl 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Czipó Bence
// Neptun : B462JZ
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

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

const float EPSILON = 0.001;
const float C = 1.0;
const int MAX_DEPTH = 3;

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y, z;

   Vector( ) { 
	x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) { 
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) const { 
	return Vector(x * a, y * a, z * a); 
   }
   Vector operator/(float a) const {
	return (*this) * (1.0 / a);
   }
   Vector operator+(const Vector& v) const {
 	return Vector(x + v.x, y + v.y, z + v.z); 
   }
   Vector operator-(const Vector& v) const {
 	return Vector(x - v.x, y - v.y, z - v.z); 
   }
   float operator*(const Vector& v) const { 	// dot product
	return (x * v.x + y * v.y + z * v.z); 
   }
   Vector operator%(const Vector& v) const { 	// cross product
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   bool operator==(const Vector& v) {
	return (*this - v).Length() < EPSILON;
   }
  
   float Length() { return sqrt(x * x + y * y + z * z); }
   float Dist(Vector v) { return (*this - v).Length(); }
   Vector norm() {return *this * (1 / this->Length()); }
   
};
 
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) { 
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) { 
	r = r0; g = g0; b = b0;
   }
   Color operator*(float a) { 
	return Color(r * a, g * a, b * a); 
   }
   Color operator*(const Color& c) { 
	return Color(r * c.r, g * c.g, b * c.b); 
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b); 
   }
};

const Color AMBIENT_LIGHT(.9,.1,.1);

struct Light {
	Vector pos,vel;
	Color color;
};



struct Material {
	Color n, kd;
	bool reflective,refractive;
	Material(Color nn, Color nkd,bool refl, bool refr) : n(nn), kd(nkd), reflective(refl), refractive(refr) {}
	Material() : n(Color(0,0,0)),kd(Color(0,0,0)), reflective(false), refractive(false) {}
	
	Color shade() {
		return Color(0,0,0);
	}
	
};

const Material GOLD(Color(0.17,0.35,1.5),Color(3.1,2.7,1.9),true,false);
const Material GLASS(Color(1.5,1.5,1.5),Color(0,0,0),true,false);


struct Intersection {
	bool real;
	Vector pos,n;
	float t;
	Material *material;
	Intersection () : real(false), pos(Vector()), n(Vector()), t(0) {}
	//WE GO BACK IN TIME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Intersection (Vector _v, Vector _n, float time, Material *mat) : real(true), pos(_v), n(_n), t(-1*time / C), material(mat) {}
};

struct Ray {
	Vector p0, dir;
	Ray() : p0(Vector()), dir(Vector()) {};
	Ray(Vector o, Vector d) : p0(o), dir(d.norm()) {}
	Vector getVec(float t) const {
		return p0 + dir *t;
	}
};


struct Object {
	Material *m;
	virtual Intersection intersect(const Ray& ray) = 0;
	virtual ~Object () {}
};

struct Plain : public Object {
	Vector p,n;
	Intersection intersect(const Ray& ray) {
		if (ray.dir * n < EPSILON) return Intersection();
		float intersection_param = ((p - ray.p0) * n)/(ray.dir * n);
		if (intersection_param < 0) return Intersection();
		
		return Intersection(ray.getVec(intersection_param),n,intersection_param,m);
	}
};

struct Paraboloid : public Object {
	Vector p;
	Plain sf;
};

struct Ellipsoid : public Object {
	float ax_a, ax_b, ax_c;
	
	Ellipsoid() {ax_a = ax_b = ax_c = 1;}
};

struct Room {
	long objectNumber;
	Object *objects;
	long lightNumber;
	Light *lights; 
	
	Room() : objectNumber(0), lightNumber(0) {}
	
	Intersection getFirstInter(Ray r) {
		Intersection closest, tmp;
		closest.t = 0;
		for (int i = 0; i< objectNumber; i++) {
			Intersection inter = objects[i].intersect(r);
			if (inter.real && inter.t < closest.t)
				inter = closest;
		}
		return closest;
	}
	
	Color traceRay(Ray ray, int depth = 0) {
		if (depth  >= MAX_DEPTH) return AMBIENT_LIGHT;
		Intersection hit = getFirstInter(ray);
		if (hit.t >= 0) return AMBIENT_LIGHT;
		Color outRadiance(0,0,0); //OR AMBIENT_LIGHT
		for (int i = 0; i< lightNumber; i++) {
			Intersection lightHit = getFirstInter(Ray(hit.pos + hit.n.norm()*EPSILON, lights[i].pos - hit.pos));
			if (lightHit.t >= 0 || lightHit.t < (lights[i].pos - hit.pos).Length()) 
				outRadiance = outRadiance + hit.material->shade();
		}
		if (hit.material->reflective) {
		
		}
		if (hit.material->refractive) {
		
		}
		return outRadiance;
	}
};

struct Screen {
	const static int WIDTH = 600;
	const static int HEIGHT = 600;
	
	Color image[WIDTH*HEIGHT];
	
	void draw() {
		glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_FLOAT, image);
	}
	
	Vector static getPixelPos(int x, int y) {
		float posX = (x + 0.5) / (float) Screen::WIDTH/2 - 1;
		float posY = (y + 0.5) / (float) Screen::HEIGHT/2 - 1; 
		return Vector (posX,posY);		
	}
	
};

struct Camera {
	Vector pos,dir,up, right;
	
	Camera() {
		*this = Camera(Vector(),Vector(), Vector());
	}
	
	Camera(Vector pos, Vector dir, Vector up) {
		this -> pos = pos;
		this -> dir = dir.norm();
		this -> right = (dir % up).norm();
		this -> up = (this-> right % this->dir).norm();
	}
	
	Ray getRay(int x, int y) {
		Vector hitScreen = Screen::getPixelPos(x,y);
		hitScreen = pos + dir + right * hitScreen.x + up * hitScreen.y;
		return Ray(pos,hitScreen-pos);
	}
	
};

struct World {
	Camera cam;
	Screen screen;
	Room room;
	
	void draw() {screen.draw();}
	
	void render() {
		glViewport(0, 0, screen.WIDTH, screen.HEIGHT);
		for(int Y = 0; Y < screen.HEIGHT; Y++)
			for(int X = 0; X < screen.WIDTH; X++)
				screen.image[Y*screen.WIDTH + X] = room.traceRay(cam.getRay(X,Y));
	}
};

World world;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	
	world.render();

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	world.draw();

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == ' ') {
		//long time = glutGet(GLUT_ELAPSED_TIME);
		//CALCULATE_IMAGE
		//calculateImage(time)
		glutPostRedisplay( );
	}

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
     		// program inditasa ota eltelt ido

}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}
