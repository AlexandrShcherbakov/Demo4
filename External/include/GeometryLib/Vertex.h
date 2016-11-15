#ifndef VERTEX_H
#define VERTEX_H

#include "Utility.h"

class Vertex
{
    public:
        Vertex();
        VM::vec4 Position;
        VM::vec4 Normal;
        VM::vec2 TexCoord;
        uint MaterialNumber;
        VM::vec4 RelationIndices;
        VM::vec4 RelationWeights;

        inline bool operator==(const Vertex& v) const {
            return MaterialNumber == v.MaterialNumber &&
                   TexCoord == v.TexCoord &&
                   Normal == v.Normal &&
                   Position == v.Position;
        }

        inline bool operator!=(const Vertex& v) const {
            return !(*this == v);
        }
    protected:
    private:
};

#endif // VERTEX_H
