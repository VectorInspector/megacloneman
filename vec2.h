#ifndef VEC2_H
#define VEC2_H
#include <iostream>
#include <cmath>

enum Direction : int {
	dir_none	= 0
,	dir_right
,	dir_up
,	dir_left
,	dir_down
};

constexpr double pi () { return acos(-1); }

// kd: oh no oh no oh no oh no I forgot computer scientists are allergic to maths. If you're a
// mathematician, have a laugh with me at their inferiority. If you're neither, have a laugh at your
// own superiority instead.
struct Vec2 {
	Vec2 ();
	Vec2 (double x_, double y_);
	Vec2 (const Vec2& v);
	
	static constexpr double deg_to_rad = pi() / 180;
	static constexpr double rad_to_deg = 180 / pi();
	
	Vec2 operator = (const Vec2& v);
	// Vec2 operator * (const Vec2& v, double f) const { return Vec2(f * v.x, f * v.y); }
	Vec2 operator * (double f) const;
	Vec2 operator / (double f) const;
	Vec2 operator + (const Vec2& v) const;
	Vec2 operator - (const Vec2& v) const;
	Vec2 operator += (const Vec2& v);
	Vec2 operator -= (const Vec2& v);
	Vec2 operator *= (double f) const;
	Vec2 operator /= (double f) const;
	bool operator == (const Vec2& v);
	bool operator != (const Vec2& v);
	double Len2 () const;
	double Len () const;
	Vec2 Unit () const;
	
	double Dot (const Vec2& v) const;
	double InsDot (const Vec2& v) const;
	double Rad () const;
	double Deg () const;
	Vec2 Rot90is () const;
	Vec2 Rot90as () const;
	
	// Vec2& operator - (Vec2& 
	
	double x;
	double y;
};

Vec2 operator - (const Vec2& v);
Vec2 operator * (double f, const Vec2& v);
std::ostream& operator << (std::ostream& s, const Vec2& v);
Vec2 RadToVec2 (double rad = 0);
Vec2 DegToVec2 (double deg = 0);
Vec2 DirToVec2 (Direction dir = dir_none);
Vec2 CompVec2 (const Vec2& u, const Vec2& v);
Vec2 ClampVec2 (const Vec2& x, const Vec2& u, const Vec2& v);

// Sign of the input number.
int sgn (double u);

// Find the velocity that will, under the specified gravity and time, make a projectile hit
// "target_pos" when fired from "init_pos".
Vec2 GravityVel (double gravity, Vec2 init_pos, Vec2 target_pos, double hit_time);
Vec2 GravityHorVel (double gravity, Vec2 init_pos, Vec2 target_pos, double hor_vel);

// This will swap u and v in place if u is greater than v.
void Order2 (int& u, int& v);

// Linearly interpolate between two vector space types.
template <typename U>
U Linear (double x, double x0, U y0, double x1, U y1);

#endif
