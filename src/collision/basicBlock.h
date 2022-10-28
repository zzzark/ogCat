/*
@author: zrk
@date:   2020/11/6
*/
#pragma once
#include "glm/glm.hpp"

namespace collision
{
    class basicBlock
    {
    protected:
        glm::vec3 _o;
        glm::vec3 _x;
        glm::vec3 _y;
        glm::vec3 _z;
        glm::vec3 _vts[8];
        bool _intersect(float mina, float maxa, float minb, float maxb) const;
        void _shadow(const glm::vec3& n, const glm::vec3 vts[8], float& min, float& max) const;
        bool _inside(const glm::vec3& n, const glm::vec3 vts[8]) const;
        
        //  return true if there is a point of block "bk" that inside block "this"
        bool _test(const basicBlock& bk) const;
    public:
        basicBlock();
        void set(const glm::vec3 o, float x, float y, float z);
        void update(const glm::mat4& mat);

        static bool test(const basicBlock& a, const basicBlock& b) {
            return a._test(b) && b._test(a);
        }
    };
}
