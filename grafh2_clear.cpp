//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül 
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
// Nev    : Czip� Bence
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
const float STEP_EPSILON = 0.001;
const float L_SP = 1.0;
const int MAX_DEPTH = 5;
const float T_MAX = 100;
float GLOBAL_TIME = 0;

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
  
   float Length() const { return sqrt(x * x + y * y + z * z); }
   float Dist(Vector v) const { return (*this - v).Length(); }
   Vector norm() const {return *this * (1 / this->Length()); }
   
};

struct Matrix_4_4{
	float matr[4][4];
	
	Matrix_4_4() {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matr[i][j] = 0;
	}
	
	Matrix_4_4(float ma[4][4]) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matr[i][j] = ma[i][j];
	} 
	
	Matrix_4_4 operator *(const Matrix_4_4 & m) const {
		float ma[4][4];
		
		for (int i= 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				ma[i][j] = 0;
				for (int k = 0; k<4;k++)
					ma[i][j] += matr[i][k] * m.matr[k][j];
			}
		}
		
		return Matrix_4_4(ma);
		
	}
	
	static Matrix_4_4 scale(const Vector& sc) {
		Matrix_4_4 tmp = eye();
		tmp.matr[0][0] = 1.0 / sc.x;
		tmp.matr[1][1] = 1.0 / sc.y;
		tmp.matr[2][2] = 1.0 / sc.z;
		return tmp;
	}
	
	
	static Matrix_4_4 translate(const Vector& trans) {
		Matrix_4_4 tmp = eye();

		tmp.matr[0][3] = -1 * trans.x;
		tmp.matr[1][3] = -1 * trans.y;
		tmp.matr[2][3] = -1 * trans.z;
		

		return tmp;
	}
	
	static Matrix_4_4 rotateX(const float alpha) {
		float a = -alpha;
		Matrix_4_4 tmp = eye();
		tmp.matr[1][1] = cos(a);
		tmp.matr[1][2] = sin(a);
		tmp.matr[2][1] = -1 * sin(a);
		tmp.matr[2][2] = cos(a);
		
		return tmp;
	}
	
	static Matrix_4_4 rotateY(const float alpha) {
		float a = -alpha;
		Matrix_4_4 tmp = eye();
		tmp.matr[0][0] = cos(a);
		tmp.matr[0][2] = -1 * sin(a);
		tmp.matr[2][0] = sin(a);
		tmp.matr[2][2] = cos(a);
		
		return tmp;
	}
	
	static Matrix_4_4 rotateZ(const float alpha) {
		float a = -alpha;
		Matrix_4_4 tmp = eye();
		tmp.matr[0][0] = cos(a);
		tmp.matr[0][1] = sin(a);
		tmp.matr[1][0] = -1 * sin(a);
		tmp.matr[1][1] = cos(a);
		
		return tmp;
	}
	
	static Matrix_4_4 eye() {
		Matrix_4_4 tmp;

		tmp.matr[0][0] = 1;
		tmp.matr[1][1] = 1;
		tmp.matr[2][2] = 1;
		tmp.matr[3][3] = 1;
		

		return tmp;
	}
	
	Matrix_4_4 T () {
		float ma[4][4];
		
		for (int i= 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				ma[i][j] = matr[j][i];
			}
		}
		return Matrix_4_4(ma);
	}
	
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
   Color operator*(float a) const { 
	return Color(r * a, g * a, b * a); 
   }
   Color operator*(const Color& c) const { 
	return Color(r * c.r, g * c.g, b * c.b); 
   }
   Color operator/(const Color& c) const { 
	return Color(r / c.r, g / c.g, b / c.b); 
   }
   Color operator+(const Color& c) const {
 	return Color(r + c.r, g + c.g, b + c.b); 
   }
   Color operator-(const Color& c) const {
 	return Color(r - c.r, g - c.g, b - c.b); 
   }
};

const Color AMBIENT_LIGHT(.7,.7,.7);

struct LightInfo {
	Vector dir;
	float time;
	Color rad;
	bool valid;
	
	LightInfo() : valid(false) {}
	
	LightInfo(Vector _dir, float _t, Color _r): dir(_dir), time(_t), rad(_r), valid(true) {}
	
};

struct Light {
	Vector pos,vel;
	Color color;
	float power;

	Light() : pos(Vector()), vel(Vector()),color(Color()),power(0) {}
	Light (Vector _p, Vector _v, Color _c, float _pow) : pos(_p), vel(_v), color(_c), power(_pow) {}
	virtual Vector getPos(float t) { return pos + vel*t;}
	virtual LightInfo getInfo(Vector intPos, float timeElapsed) = 0;
	
	virtual ~Light() {}

};

struct PointLight: public Light {
	PointLight (Vector _p, Vector _v, Color _c, float _pow) : Light(_p, _v, _c, _pow) {}
	
	
	LightInfo getInfo(Vector intPos, float time_elapsed) {
		Vector d = getPos(time_elapsed) - intPos;
		
		float disc = 4 * (vel * d) * (vel *d) - (d *d) * (vel * vel) + (d*d) + L_SP * L_SP;
		if (disc < 0) {
			return LightInfo();
		}
		disc = sqrtf(disc);
		float t1 = (vel * d * 2.0 + disc) / 2.0 / (vel * vel - L_SP * L_SP);
		float t2 = (vel * d * 2.0 - disc) / 2.0 / (vel * vel - L_SP * L_SP);
		
		float collTime;
		if (t1 > 0) {
			if (t2 > 0 && t2 < t1) collTime = t2;
			else collTime = t1;
		} else if (t2 > 0) collTime = t2;
		else return LightInfo();
		
		Vector lumPos = getPos(time_elapsed - collTime);
		Vector dV = lumPos - intPos;
		Color radOut = color *( 1.0 / dV.Length() / dV.Length() * power);
		
		return LightInfo(dV.norm(), collTime, radOut);
	}
	
};

struct Material;

struct Intersection {
	bool real;
	Vector pos,n;
	float t;
	const Material *material;
	Intersection () : real(false), pos(Vector()), n(Vector()), t(0), material(NULL) {}
	Intersection (Vector _v, Vector _n, float time, const Material *mat) : real(true), pos(_v), n(_n), t(time), material(mat) {}
};

struct Ray {
	Vector p0, dir;
	float shootTime;
	Ray(Vector o = Vector(), Vector d = Vector(), float shoot_t = GLOBAL_TIME) : p0(o), dir(d.norm()), shootTime(shoot_t) {}
	Vector getVec(float t) const {
		return p0 + dir *t * L_SP;
	}
};

struct Pattern {
	virtual Color getPattern(const Vector& pos, const Color& col) const = 0;
	virtual ~Pattern() {}
};

struct SimplePattern : public Pattern {
	Color (*pattern_func)(const Vector&);
	
	SimplePattern(Color (*_pf)(const Vector&)) : pattern_func(_pf) {}
	
	virtual Color getPattern(const Vector& pos, const Color& col) const {
		return pattern_func(pos);
	}
};

struct ShadowPattern : public Pattern {
	Color (*pattern_func)(const Vector&);
	const Color intensity;
	ShadowPattern(Color (*_pf)(const Vector&), float _in = 0.7f) : pattern_func(_pf), intensity(Color(_in,_in,_in)) {}
	ShadowPattern(Color (*_pf)(const Vector&), const Color& _in) : pattern_func(_pf), intensity(_in) {}
	
	virtual Color getPattern(const Vector& pos, const Color& col) const {
		return col * (intensity + (Color(1,1,1) -intensity) * pattern_func(pos));
	}
};

struct TwoColoredPattern : public Pattern {
	Color (*pattern_func)(const Vector&);
	Color secondary;
	TwoColoredPattern(Color (*_pf)(const Vector&), Color _sec) : pattern_func(_pf), secondary(_sec) {}
	
	virtual Color getPattern(const Vector& pos, const Color& col) const {
		return col * pattern_func(pos) + secondary * (Color(1,1,1) - pattern_func(pos));
	}

};

struct Material {
	Color kd ,ks;
	Color n, F0; 
	bool reflective,refractive;
	float shin;
	Material(Color _kd, Color _ks, Color _n, Color _k, bool refl, bool refr, float _s) :
	 kd(_kd), ks(_ks), n(_n), reflective(refl), refractive(refr), shin(_s) {		
		this->F0 = (n - Color(1, 1, 1) * (n - Color(1, 1, 1)) + _k*_k) / ((n + Color(1, 1, 1))*(n + Color(1, 1, 1)) + _k*_k);
	}
	Material(Color c) {
		*this = Material();
		kd = c;
	}
	Material() {
		kd = Color();
		ks= Color();
		n= Color();
		F0 = Color();
		reflective = refractive = false;
		shin = 0;
	}
	
	virtual Color get_kd(const Vector& pos) const {
		return kd;
	}
	
	virtual Color get_ks(const Vector& pos) const {
		return ks;
	}
	
	virtual float get_shin(const Vector& pos) const {
		return shin;
	}
	
	Color shade(const Vector & normal, const Vector& viewDir, const Vector& pos, const LightInfo& li) const {
		Vector lDir = li.dir;
		Color radIn = li.rad;
		
		Color radOut = Color();
		float cosTheta = normal * lDir;
		if (cosTheta <= 0)return radOut;
		radOut = radIn *  cosTheta * get_kd(pos);
		float cosDelta = normal * ((viewDir + lDir).norm());
		if (cosDelta < 0) return radOut;
		
		return radOut + radIn * get_ks(pos) * pow(cosDelta,get_shin(pos));
	}
	
	static Vector reflect(const Vector & normal, const Vector & viewIn) {
		return viewIn  - normal * (normal * viewIn) * 2.0 ;
	}
	
	Vector refract(const Vector & normal, const Vector & viewIn) const {
		float cosalpha = -1 * (normal * viewIn);
		float rn = (n.r + n.g + n.b) / 3.0;
		Vector norm = normal;
		if (cosalpha < 0) {
			cosalpha *= -1;
			norm = normal * -1;
			rn = 1.0/rn;
		
		}
		
		float disc = 1 - (1 - cosalpha * cosalpha) / rn /rn;
		if (disc < 0) { return reflect(norm,viewIn); }
		return (viewIn / rn + norm * (cosalpha / rn - sqrt(disc))).norm();
		
	}
	
	Color fresnel(const Vector & normal, const Vector & viewIn) const {
		float cosalfa = fabs(normal * viewIn); 
		return F0 + (Color(1,1,1) - F0 ) * powf(1 - cosalfa ,5);
	}
	
	virtual ~Material() {}
	
};

struct PatternedMaterial : public Material {
	const Pattern* pattern;
	
	PatternedMaterial(Color _kd, Color _ks, Color _n, Color _k, bool refl, bool refr, float _s, const Pattern* _pattern) :
	Material(_kd,_ks,_n,_k,refl,refr,_s), pattern(_pattern) {}
	PatternedMaterial(Color c, const Pattern* _pattern) : Material(c), pattern(_pattern) {}
	PatternedMaterial(const Pattern* _pattern) : Material(), pattern(_pattern) {}
	
	virtual Color get_kd(const Vector& pos) const {
		return pattern->getPattern(pos,kd);
	}
};


Color stripes(const Vector & pos) {
		float magic = ((int)(fabs(pos.x)+ fabs(pos.y-5) + fabs(pos.z+5)) * 3) % 2 == 0 ? 1 : 0;
		return Color(magic,magic,magic);
}

ShadowPattern STRIPES_SHAD(&stripes);
TwoColoredPattern STRIPES_TWO(&stripes, Color(.5,.1,.4));

const Material GOLD(Color(), Color(), Color(0.17,0.35,1.5),Color(3.1,2.7,1.9),true,false,0);
const Material GLASS(Color(), Color(), Color(1.5,1.5,1.5),Color(0,0,0),true,true,0);
const PatternedMaterial SIMPLE(Color(.2,.5,.1), Color(0,0,0), Color(),Color(),false,false,0, &STRIPES_TWO);
const PatternedMaterial SIMPLE2(Color(.7,.8,.5), &STRIPES_SHAD);


struct Object {
	const Material *m;
	Vector vel;
	
	Object (const Material *mat, const Vector& _v = Vector(0,0,0)) : m(mat), vel(_v) {}
	
	virtual Intersection intersect(const Ray& ray) = 0;
	virtual ~Object () {}
};

struct Plane : public Object {
	Vector p,n;
	
	Plane(const Material* m , const Vector& _p, const Vector& _n, const Vector& _v = Vector()) :Object(m, _v), p(_p), n(_n.norm()) {}
	
	Intersection intersect(const Ray& ray) {
		if (fabs(ray.dir * n) < EPSILON) return Intersection();
		float intersection_param = ((p - ray.p0) * n)/(ray.dir * n);
		if (intersection_param < 0) return Intersection();
		return Intersection(ray.getVec(intersection_param),n,intersection_param,m);
	}
};

struct QuadricShape : public Object {
		float A,B,C,D,E,F,G,H,I,J;
		
		Vector getNormalAt(Vector inter, Vector dir) {
			float nx = 2 * A * inter.x + D * inter.y + E * inter.z + G;
			float ny = 2 * B * inter.y + D * inter.x + F * inter.z + H;
			float nz = 2 * C * inter.z + E * inter.x + F * inter.y + I;
			
			Vector n = Vector(nx,ny,nz);
			
			n = n.norm();
			
			
			return n;
			
		}
		
		Intersection intersect(const Ray& ray) {
			Vector r0 = ray.p0 - (vel * ray.shootTime);
			Vector rd = (ray.dir * L_SP) + vel;
			
			float x0 = r0.x;
			float y0 = r0.y;
			float z0 = r0.z;
			float xd = rd.x;
			float yd = rd.y;
			float zd = rd.z;
			
			
			float Av = A * xd *xd + B * yd *yd + C * zd * zd + D * xd * yd + E * xd *zd + F * yd * zd;
			float Bv = 2 * (A * x0 * xd + B * y0*yd + C * z0*zd) + D * (x0*yd + xd*y0) +
			E * (x0 * zd + xd * z0) + F * (y0*zd+ yd* z0) + G * xd + H * yd + I * zd;
			float Cv = A * x0 * x0 + B * y0 * y0 + C * z0 * z0 + D * x0*y0 + E * x0 *z0 + F * y0*z0 + 
			G*x0+ H *y0+ I * z0+ J;
			
			if (Bv * Bv < 4 * Av * Cv) return Intersection();
			float Dv = sqrt(Bv * Bv - 4 * Av * Cv);
			
			float param;
			
			if (fabs(Av) < EPSILON) {
				if (fabs(Bv) < EPSILON) return Intersection();
				param = -Cv / Bv;
				if (param < 0) {
					return Intersection();
				}
			} else {
				float t0 = (Dv - Bv) / 2 / Av;
				float t1 = (-1 * Dv - Bv) /2 /Av;
			
				if (t0 <0 && t1 < 0) return Intersection();
				if (t0 > 0 && t1 > 0) param = t0 > t1 ? t1 : t0;
				else param = t0 > t1 ? t0 : t1;
			}
			
			Vector inter = ray.getVec(param);
			
			Vector n = getNormalAt(inter,ray.dir);
			
			return Intersection(inter,n,param,m);
			
		}
		
		void setParams(Matrix_4_4 m) {
			A = m.matr[0][0];
			B = m.matr[1][1];
			C = m.matr[2][2];
			D = m.matr[0][1] * 2.0;
			E = m.matr[0][2] * 2.0;
			F = m.matr[1][2] * 2.0;
			G = m.matr[0][3] * 2.0;
			H = m.matr[1][3] * 2.0;
			I = m.matr[2][3] * 2.0;
			J = m.matr[3][3];	
		}
		
		QuadricShape(const Material * m = &GLASS, const Vector& v = Vector()) : Object(m,v) {}
		virtual ~QuadricShape() {};
		
};

struct Paraboloid : public QuadricShape {
	Vector f;
	Vector r;
	Vector n;
	
	Paraboloid(const Material* m, const Vector & _p, const Vector& _r, const Vector& _n, const Vector& _v = Vector()) : QuadricShape(m,_v), f(_p), r(_r), n(_n) {
		float K = r*n;
		
		A = 1 - n.x * n.x; 
		B = 1 - n.y * n.y;
		C = 1 - n.z * n.z;
		D = -2 * n.x * n.y;
		E = -2 * n.x * n.z;
		F = -2 * n.y * n.z;
		G = -2 * f.x + 2 * n.x * K;
		H = -2 * f.y + 2 * n.y * K;
		I = -2 * f.z + 2 * n.z * K;
		J = f * f - K * K;
	
	}
	
};

struct Ellipsoid : public QuadricShape {
	
	Ellipsoid(const Material *m, const Vector& pos, const Vector& sc,const Vector& rot, const Vector& _v = Vector()): QuadricShape(m,_v) {
		Matrix_4_4 matr = Matrix_4_4::eye();
		matr.matr[3][3] = 0;
	
		Matrix_4_4 trans = Matrix_4_4::scale(sc);
			
		matr = trans.T() * matr * trans;
		
		trans = Matrix_4_4::rotateX(rot.x) * Matrix_4_4::rotateY(rot.y) * Matrix_4_4::rotateZ(rot.z);
			
		matr = trans.T() * matr * trans;
		
		
		trans = Matrix_4_4::translate(pos);
	
		matr = (trans.T() * matr) * trans;
		

		matr.matr[3][3] -= 1;
		
		setParams(matr);
		
	}
	
};

struct Room {
	long objectNumber;
	Object* objects[20];
	long lightNumber;
	Light* lights[6]; 
	
	Room() : objectNumber(0), lightNumber(0) {
	}
	
	void addObject(Object *o) {
		if (objectNumber == 20) return;
		objects[objectNumber] = o;
		objectNumber++;
	}
	
	void addLight(Light *o) {
		if (lightNumber == 6) return;
		lights[lightNumber] = o;
		lightNumber++;
	}
	
	Intersection getFirstInter(const Ray& r) {
		Intersection closest, tmp;
		closest.t = T_MAX;
		for (int i = 0; i< objectNumber; i++) {
			Intersection inter = objects[i]->intersect(r);
			if (inter.real && inter.t < closest.t)
				closest = inter;
		}
		return closest;
	}
	
	Color traceRay(const Ray &ray, int depth = 0) {
		if (depth  >= MAX_DEPTH) return AMBIENT_LIGHT;
		Intersection hit = getFirstInter(ray);
		
		if (! hit.real || hit.material == NULL || hit.t <= 0) return Color();
		Color outRadiance = AMBIENT_LIGHT * hit.material -> get_kd(hit.pos);
		float time_elapsed = ray.shootTime - hit.t;
		
		for (int i = 0; i< lightNumber; i++) {
			Vector norm = hit.n;
			Vector vIn = (ray.dir);
			
			if ((vIn * norm) > 0) norm = norm * -1;
		
			LightInfo li = lights[i]->getInfo(hit.pos, time_elapsed);
			if (li.valid) {
				Intersection shadowIntersection = getFirstInter(Ray(hit.pos + norm*STEP_EPSILON, li.dir, time_elapsed));
				if (shadowIntersection.t <= 0 || shadowIntersection.t > li.time) {
					outRadiance = outRadiance + hit.material->shade(norm,vIn * -1, hit.pos,li);
				}
			}
		}
		

		if (hit.material->reflective) {
			
			Vector norm = hit.n;
			Vector vIn = (ray.dir);
			
			if ((vIn * norm) > 0) norm = norm * -1;
			
			Color fres = hit.material -> fresnel(norm,vIn);
			Vector vOut = hit.material -> reflect(norm,vIn);
			
			Ray reflectedRay = Ray (hit.pos + norm*STEP_EPSILON, vOut,time_elapsed);
			
			Color l_in = traceRay(reflectedRay, depth +1);
			outRadiance = outRadiance + (l_in * fres) ; 
		}
		if (hit.material->refractive) {
			
			Vector norm = hit.n;
			Vector vIn = (ray.dir);
			Vector nNorm = (vIn * norm > 0) ? norm * -1 : norm;
			
			Color fres = hit.material -> fresnel(nNorm ,vIn);
			
			Ray refractedRay = Ray (hit.pos - nNorm*STEP_EPSILON, hit.material -> refract(norm,vIn), time_elapsed);
			outRadiance = outRadiance + traceRay(refractedRay, depth +1) * (Color(1,1,1) - fres); 
		}
		return outRadiance;
	}
	
	~Room() {
		for (int i = 0; i< objectNumber; i++) delete objects[i];
		for (int i = 0; i< lightNumber; i++) delete lights[i];
	}
	
};

struct Screen {
	const static int WIDTH = 600;
	const static int HEIGHT = 600;
	
	Color image[WIDTH*HEIGHT];
	
	void draw() {
		glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_FLOAT, image);
	}
	
	Vector static getPixelPos(float x, float y) {
		float posX = (x + 0.5) / ((float) Screen::WIDTH/2.0) - 1.0;
		float posY = (y + 0.5) / ((float) Screen::HEIGHT/2.0) - 1.0; 
		return Vector (posX,posY);		
	}
	
};

struct Camera {
	Vector pos,dir,up, right;
	
	Camera() {
		*this = Camera(Vector(0.01,0,0), Vector(1,0,0), Vector(0,1,0));
	}
	
	Camera(Vector pos, Vector dir, Vector up) {
		this -> pos = pos;
		this -> dir = dir.norm();
		this -> right = (dir % up).norm();
		this -> up = (this-> right % this->dir).norm();
	}
	
	Ray getRay(int x, int y) {
		Vector hitScreen = Screen::getPixelPos(x,y);
		hitScreen = dir + right * hitScreen.x + up * hitScreen.y;
		return Ray(pos,hitScreen);
	}
	
};

struct World {
	Camera cam;
	Screen screen;
	Room room;
	
	World() {
	
		room.addObject( new Plane(&SIMPLE2,Vector(10,0,0),Vector(-1,0,0)));
		room.addObject( new Plane(new Material(Color(.2,.4,.6)),Vector(10,0,-5),Vector(0,0,1)));
		room.addObject( new Plane(new Material(Color(.6,.4,.2)),Vector(10,5,0),Vector(0,-1,0)));
		room.addObject( new Plane(new Material(Color(.3,.3,.9)),Vector(10,-5,0),Vector(0,1,0)));
		room.addObject( new Plane(&SIMPLE,Vector(0,0,0),Vector(1,0,0)));
		
		room.addObject(new Ellipsoid(&GLASS, Vector(10,-5,-5), Vector(.25,.25,1), Vector(1,0,.2), Vector(-0.4,.4,.4)));
		room.addObject(new Paraboloid(&GOLD, Vector(5,0,7.5), Vector(5,0,-2.5) ,Vector(0,0,1)));
		
		room.addLight( new PointLight(Vector(2,3,-2), Vector(.1,-0.1,0), Color(1,1,1), 20));	
		
	}
	
	void draw() {screen.draw();}
	
	void render() {
		glViewport(0, 0, screen.WIDTH, screen.HEIGHT);
		for(int Y = 0; Y < screen.HEIGHT; Y++)
			for(int X = 0; X < screen.WIDTH; X++) {
				screen.image[Y*screen.WIDTH + X] = room.traceRay(cam.getRay(X,Y));
		}
	}
};

World world;

Vector camPos = Vector(.5,-4.5,-4.5);
Vector camFwd = Vector(1,1,1);
Vector camUp = Vector(0,1,0);

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	
	

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	world.cam = Camera(camPos,camFwd,camUp);
	world.render();
	world.draw();

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == ' ') {
		GLOBAL_TIME = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
		glutPostRedisplay( );
	}
	float UNIT = .5;
	if (key == 'a') {
		camPos = camPos - world.cam.right * UNIT;
		glutPostRedisplay( );
	}
	if (key == 'd') {
		camPos = camPos + world.cam.right * UNIT;
		glutPostRedisplay( );
	}
	if (key == 'w') {
		camPos = camPos + world.cam.dir * UNIT;
		glutPostRedisplay( );
	}
	if (key == 's') {
		camPos = camPos - world.cam.dir * UNIT;
		glutPostRedisplay( );
	}
	if (key == 'r') {
		camPos = camPos + world.cam.up * UNIT;
		camPos.x += UNIT;
		glutPostRedisplay( );
	}
	if (key == 'f') {
		camPos = camPos - world.cam.up * UNIT;
		glutPostRedisplay( );
	}
	if (key == '6') {
		camFwd = (camFwd % camUp + camFwd * 3) / 4; 
		glutPostRedisplay( );
	}
	if (key == '4') {
		camFwd = (camUp % camFwd + camFwd * 3) / 4; 
		glutPostRedisplay( );
	}
	if (key == '8') {
		camFwd = (world.cam.right % world.cam.dir + world.cam.dir*3) / 4; 
		camUp = world.cam.right % world.cam.dir;
		glutPostRedisplay( );
	}
	if (key == '2') {
		camFwd = (world.cam.dir % world.cam.right + world.cam.dir*3) / 4; 
		camUp = world.cam.right % world.cam.dir;
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