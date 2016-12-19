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

    protected:
    private:
};

}

#endif // I16VEC4_H
