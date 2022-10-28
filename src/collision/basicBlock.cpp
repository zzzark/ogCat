#include <cfloat>
#include "basicBlock.h"

collision::basicBlock::basicBlock()
{
    _o = glm::vec3(0);
    _x = glm::vec3(0);
    _y = glm::vec3(0);
    _z = glm::vec3(0);
    for (int i = 0; i < 8; i++)
        _vts[i] = glm::vec3(0);
}

void collision::basicBlock::set(const glm::vec3 o, float x, float y, float z)
{
    _o = o;
    _x = glm::vec3(x, 0, 0);
    _y = glm::vec3(0, y, 0);
    _z = glm::vec3(0, 0, z);
}

void collision::basicBlock::update(const glm::mat4& mat)
{
    glm::vec3 off  = mat * glm::vec4(_o, 1.0f);
    glm::vec3 xoff = mat * glm::vec4(_x, 0.0f);
    glm::vec3 yoff = mat * glm::vec4(_y, 0.0f);
    glm::vec3 zoff = mat * glm::vec4(_z, 0.0f);
    _vts[0] = off;
    _vts[1] = off + xoff;
    _vts[2] = off + yoff;
    _vts[3] = off + zoff;
    _vts[4] = off + xoff + yoff;
    _vts[5] = off + xoff + zoff;
    _vts[6] = off + yoff + zoff;
    _vts[7] = off + xoff + yoff + zoff;
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
        if (min > dt) min = dt;
        if (max < dt) max = dt;
    }
}

bool collision::basicBlock::_inside(const glm::vec3& n, const glm::vec3 vts[8]) const
{
    float mina = FLT_MAX;
    float maxa = -FLT_MAX;
    float minb = FLT_MAX;
    float maxb = -FLT_MAX;
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
