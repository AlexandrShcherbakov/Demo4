#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <cmath>

#include "SOIL.h"

#ifndef UTILITY_H_INCLUDED
#include "GL.h"
#include "VM\vec2.h"
#include "VM\vec4.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class Image
{
	public:
		uint width, height;

		Image();
        Image(const std::string& filename);
        void load(const std::string& filename);
        VM::vec4 getAverageColor(const std::vector<VM::vec2>& texCoords) const;
        std::vector<char> getData(uint channels = 4u) const;
        void UndoGamma();

        const std::vector<VM::vec4>& operator[] (uint i) const;
        std::vector<VM::vec4>& operator[] (uint i);

	protected:
	private:
        std::vector<std::vector<VM::vec4> > data;
};

}
#endif // IMAGE_H
