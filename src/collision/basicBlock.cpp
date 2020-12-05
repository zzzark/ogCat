#include <cfloat>
#include "basicBlock.h"

void collision::basicBlock::set(const glm::vec3 o, float x, float y, float z)
{
    _o = o;
    _x = glm::vec3(x, 0, 0);
    _y = glm::vec3(0, y, 0);
    _z = glm::vec3(0, 0, z);
}

void collision::basicBlock::update(const glm::mat4& mat)
{
    glm::vec3 off = mat * glm::vec4(_o, 1.0f);
    _x = mat * glm::vec4(_x, 0.0f);
    _y = mat * glm::vec4(_y, 0.0f);
    _z = mat * glm::vec4(_z, 0.0f);
    _vts[0] = off;
    _vts[1] = off + _x;
    _vts[2] = off + _y;
    _vts[3] = off + _z;
    _vts[4] = off + _x + _y;
    _vts[5] = off + _x + _z;
    _vts[6] = off + _y + _z;
    _vts[7] = off + _x + _y + _z;
}

bool collision::basicBlock::_intersect(float mina, float maxa, float minb, float maxb) const
{
    return (mina > minb ? mina : minb) <= (maxa > maxb ? maxb : maxa);
}

void collision::basicBlock::_shadow(const glm::vec3& n, const glm::vec3 vts[8], float& min, float& max) const
{
    min = FLT_MAX;
    max = -FLT_MAX;
    for (size_t i = 0; i < 8; i++) {
        float dt = glm::dot(n, vts[i]);
        min = min > dt ? dt : min;
        max = max < dt ? dt : max;
    }
}

bool collision::basicBlock::_inside(const glm::vec3& n, const glm::vec3 vts[8]) const
{
    float mina, maxa, minb, maxb;
    _shadow(n, vts, mina, maxa);
    _shadow(n, _vts, minb, maxb);
    return _intersect(mina, maxa, minb, maxb);
}

bool collision::basicBlock::_test(const basicBlock& bk) const
{
    return
        _inside(glm::normalize(_x), bk._vts) &&
        _inside(glm::normalize(_y), bk._vts) &&
        _inside(glm::normalize(_z), bk._vts);
}
