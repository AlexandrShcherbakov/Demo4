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
#endif // VOLUME_H
