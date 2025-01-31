#include "vec2.h"
#include <cmath>

// The operations are all just normal vector space operations, and some bonuses you may encounter
// from time to time, such as component-wise products.
Vec2::Vec2 () : x(0), y(0) {}
Vec2::Vec2 (double x_, double y_) : x(x_), y(y_) {}
Vec2::Vec2 (const Vec2& v) : x(v.x), y(v.y) {}
	
Vec2 Vec2::operator = (const Vec2& v) { x = v.x; y = v.y; return *this; }
Vec2 Vec2::operator * (double f) const { return Vec2(f * x, f * y); }
Vec2 Vec2::operator / (double f) const { return Vec2(f / x, f / y); }
Vec2 Vec2::operator + (const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
Vec2 Vec2::operator - (const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
Vec2 Vec2::operator += (const Vec2& v) { x += v.x; y += v.y; return *this; }
Vec2 Vec2::operator -= (const Vec2& v) { x -= v.x; y -= v.y; return *this; }
Vec2 Vec2::operator *= (double f) const { f *= x; f *= y; return *this; }
Vec2 Vec2::operator /= (double f) const { f /= x; f /= y; return *this; }
bool Vec2::operator == (const Vec2& v) { return v.x == x && v.y == y; }
bool Vec2::operator != (const Vec2& v) { return v.x != x || v.y != y; }
double Vec2::Len2 () const { return x * x + y * y; }
double Vec2::Len () const { return sqrt(x * x + y * y); }
Vec2 Vec2::Unit () const {
	if(0 != x && 0 != y) {
		auto len = Len();
		return Vec2(x / len, y / len);
	}
	
	return *this;
}

double Vec2::Dot (const Vec2& v) const { return x * v.x + y * v.y; }
double Vec2::InsDot (const Vec2& v) const { return x * v.y - y * v.x; }
double Vec2::Rad () const { return atan2(y, x); }
double Vec2::Deg () const { return rad_to_deg * atan2(y, x); }
Vec2 Vec2::Rot90is () const { return Vec2(-y, x); }
Vec2 Vec2::Rot90as () const { return Vec2(y, -x); }

Vec2 operator * (double f, const Vec2& v) { return v * f; }

std::ostream& operator << (std::ostream& s, const Vec2& v) {
	return s << "(" << v.x << " | " << v.y << ")";
}

// A bunch of other cool ways to create vectors.
Vec2 RadToVec2 (double rad) { return Vec2(cos(rad), sin(rad)); }
Vec2 DegToVec2 (double deg) {
	deg *= Vec2::deg_to_rad;
	return Vec2(cos(deg), sin(deg));
}

Vec2 DirToVec2 (Direction dir) {
	switch(dir) {
	default:		return Vec2( 0,  0);
	case dir_up:	return Vec2( 0, -1);
	case dir_down:	return Vec2( 0,  1);
	case dir_left:	return Vec2(-1,  0);
	case dir_right:	return Vec2( 1,  0);
	}
	
	return Vec2(0, 0);
}

Vec2 CompVec2 (const Vec2& u, const Vec2& v) {
	return Vec2(u.x * v.x, u.y * v.y);
}

#include <algorithm>

Vec2 ClampVec2 (const Vec2& x, const Vec2& u, const Vec2& v)  {
	return Vec2(
		std::clamp(x.x, u.x, v.x),
		std::clamp(x.y, u.y, v.y));
}

Vec2 operator - (const Vec2& v) {
	return Vec2(-v.x, -v.y);
}

int sgn (double u) {
	if(0 == u) {
		return 0;
	}
	
	return u < 0 ? -1 : 1;
}

Vec2 GravityHorVel (double gravity, Vec2 init_pos, Vec2 target_pos, double hor_vel) {
	auto diff = target_pos - init_pos;
	double hit_time = std::abs(diff.x / hor_vel);
	return GravityVel(gravity, init_pos, target_pos, hit_time);
}

Vec2 GravityVel (double gravity, Vec2 init_pos, Vec2 target_pos, double hit_time) {
	auto diff = target_pos - init_pos;
	auto hor_vel = diff.x / hit_time;
	auto ver_vel = diff.y / hit_time - gravity / 2 * (1 + hit_time);
	return Vec2(hor_vel, ver_vel);
}

void Order2 (int& u, int& v) {
	if(v < u) {
		int temp = u;
		u = v;
		v = temp;
	}
}

template <typename U>
U Linear (double x, double x0, U y0, double x1, U y1) {
	auto f = (x - x0) / (x1 - x0);
	return f * y1 + (1 - f) * y0;
}

// Generates symbols for linker.
template Vec2 Linear (double, double, Vec2, double, Vec2);
template double Linear (double, double, double, double, double);
template int Linear (double, double, int, double, int);
