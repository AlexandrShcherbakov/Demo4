#ifndef VOLUME_H
#define VOLUME_H

#include "Utility.h"

class Volume {
	public:
		virtual bool IncludesPoint(const VM::vec4& point) const = 0;
		virtual bool IntersectsWithCube(
			const VM::vec4& minPoint,
			const VM::vec4& maxPoint) const = 0;
};

class Tube: public Volume {
	protected:
		VM::vec4 Begin;
		VM::vec4 End;
		VM::vec4 Direction;
		void UpdateDirection();
	public:
		Tube();
		Tube(const VM::vec4& begin, const VM::vec4& end, const float radius);

		void SetBegin(const VM::vec4& point);
		void SetEnd(const VM::vec4& point);

		bool IncludesPoint(const VM::vec4& point) const;
		bool IntersectsWithCube(
			const VM::vec4& minPoint,
			const VM::vec4& maxPoint) const;

		float Radius;
};

class Capsule: public Tube {
	public:
		Capsule();
		Capsule(const VM::vec4& begin, const VM::vec4& end, const float radius):
		Tube(begin, end, radius) {};

		bool IncludesPoint(const VM::vec4& point) const;
};

class Sphere: public Volume {
	public:
        Sphere(): Center(VM::vec4(0, 0, 0, 0)), Radius(1) {};
        Sphere(const VM::vec4& center, const float radius) : Center(center), Radius(radius) {}

        inline bool IncludesPoint(const VM::vec4& point) const {
            return VM::length(point - Center) < Radius + VEC_EPS;
        }
        bool IntersectsWithCube(
			const VM::vec4& minPoint,
			const VM::vec4& maxPoint) const;

        VM::vec4 Center;
        float Radius;
};

class Hemisphere: public Sphere {
	public:
        Hemisphere(): Sphere(), Normal(VM::vec4(0, 0, 1, 0)) {};
        Hemisphere(const VM::vec4& center, const float radius, const VM::vec4& normal):
            Sphere(center, radius),
            Normal(VM::normalize(normal)) {};

		inline bool IncludesPoint(const VM::vec4& point) const {
		    //std::cout << Center << ' ' << Radius << ' ' << Normal << ' ' << point << std::endl;
            return Sphere::IncludesPoint(point) && dot(Normal, VM::normalize(point - Center)) > 0;
        }

        VM::vec4 Normal;
};

#endif // VOLUME_H
