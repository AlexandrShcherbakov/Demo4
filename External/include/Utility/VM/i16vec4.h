#ifndef I16VEC4_H
#define I16VEC4_H


namespace VM {

class i16vec4
{
    public:
        short x, y, z, w;

        i16vec4():
            x(0), y(0), z(0), w(0)
        {}

        i16vec4(const short value):
            x(value), y(value), z(value), w(value)
        {}

        i16vec4(const short x, const short y, const short z, const short w):
            x(x), y(y), z(z), w(w)
        {}

        explicit i16vec4(const short* arr):
            x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3])
        {}

        const short& operator[](const int index) const {
            if (index >= 4) throw "Too big index for i16vec4";
            if (index == 0) return x;
            if (index == 1) return y;
            if (index == 2) return z;
            return w;
        }

        short& operator[](const int index) {
            return const_cast<short&>(
                static_cast<const i16vec4&>(*this)[index]
            );
        }

    protected:
    private:
};

}

#endif // I16VEC4_H
