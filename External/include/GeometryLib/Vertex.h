#ifndef VERTEX_H
#define VERTEX_H

#include "Utility.h"

class Vertex
{
    public:
        ///Getters
        VM::vec4 GetPosition() const {
            return Position;
        }
        VM::vec4 GetNormal() const {
            return Normal;
        }
        VM::vec4 GetColor() const {
            return Color;
        }
        VM::vec2 GetTexCoord() const {
            return TexCoord;
        }
        uint GetMaterialNumber() const {
            return MaterialNumber;
        }
        VM::i16vec4 GetRelationIndices() const {
            return RelationIndices;
        }
        VM::vec4 GetRelationWeights() const {
            return RelationWeights;
        }

        ///Setters
        void SetPosition(const VM::vec4& position) {
            Position = position;
        }
        void SetNormal(const VM::vec4& normal) {
            Normal = normal;
        }
        void SetTexCoord(const VM::vec2& texCoord) {
            TexCoord = texCoord;
        }
        void SetMaterialNumber(const uint materialNumber) {
            MaterialNumber = materialNumber;
        }
        void SetRelationIndices(const VM::i16vec4 relationIndices) {
            RelationIndices = relationIndices;
        }
        void SetRelationWeights(const VM::vec4 relationWeights) {
            RelationWeights = relationWeights;
        }

        ///Operators
        bool operator==(const Vertex& v) const {
            return MaterialNumber == v.MaterialNumber &&
                   TexCoord == v.TexCoord &&
                   Normal == v.Normal &&
                   Position == v.Position;
        }
        bool operator!=(const Vertex& v) const {
            return !(*this == v);
        }
    protected:
    private:
        VM::vec4 Position;
        VM::vec4 Normal;
        VM::vec4 Color;
        VM::vec2 TexCoord;
        uint MaterialNumber;
        VM::i16vec4 RelationIndices;
        VM::vec4 RelationWeights;
};

#endif // VERTEX_H
