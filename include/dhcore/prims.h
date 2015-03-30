/***********************************************************************************
 * Copyright (c) 2012, Sepehr Taghdisian
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 ***********************************************************************************/

#ifndef __PRIMS_H__
#define __PRIMS_H__

#include "types.h"
#include "vec-math.h"
#include "std-math.h"
#include "err.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

struct rect2di
{
    union   {
        struct {
            int x;
            int y;
            int w;
            int h;
        };

        int n[4];
    };
};

struct rect2df
{
    union   {
        struct {
            float x;
            float y;
            float w;
            float h;
        };

        float f[4];
    };
};

/* sphere */
struct ALIGN16 sphere
{
    union   {
        struct {
            float x;
            float y;
            float z;
            float r;
        };

        float f[4];
    };
};

struct ALIGN16 aabb
{
    struct vec4f minpt;
    struct vec4f maxpt;
};

struct ALIGN16 aabb2d
{
    struct vec2f minpt;
    struct vec2f maxpt;
};

/* eq: N(dot)P + d = 0 */
struct ALIGN16 plane
{
    union   {
        struct {
            float nx;
            float ny;
            float nz;
            float d;
        };

        float f[4];
    };
};

struct ALIGN16 frustum
{
    union   {
        struct plane planes[6];
        struct vec3f points[8];
    };
};

/* eq: p = ray.pt + ray.dir*t */
struct ALIGN16 ray
{
    struct vec4f pt;
    struct vec4f dir;
};

/* inlines */
INLINE struct plane* plane_setv(struct plane* p, const struct vec3f* n, float d)
{
    p->nx = n->x;
    p->ny = n->y;
    p->nz = n->z;
    p->d = d;
    return p;
}

INLINE struct plane* plane_setf(struct plane* p, float nx, float ny, float nz, float d)
{
    p->nx = nx;
    p->ny = ny;
    p->nz = nz;
    p->d = d;
    return p;
}

INLINE struct ray* ray_setv(struct ray* r, const struct vec3f* pt, const struct vec3f* dir)
{
    vec3_setv(&r->pt, pt);
    vec3_setv(&r->dir, dir);
    return r;
}

INLINE struct rect2di* rect2di_setr(struct rect2di* r, const struct rect2di* rc)
{
    r->x = rc->x;
    r->y = rc->y;
    r->w = rc->w;
    r->h = rc->h;
    return r;
}

INLINE struct rect2di* rect2di_seti(struct rect2di* rc, int x, int y, int w, int h)
{
    rc->x = x;
    rc->y = y;
    rc->w = w;
    rc->h = h;
    return rc;
}

INLINE int rect2di_isequal(const struct rect2di rc1, const struct rect2di rc2)
{
	return (rc1.x == rc2.x && rc1.y == rc2.y && rc1.w == rc2.w && rc1.h == rc2.h);
}

INLINE struct rect2di* rect2di_shrink(struct rect2di* rr, const struct rect2di* r, int shrink)
{
    return rect2di_seti(rr, r->x + shrink, r->y + shrink, r->w - 2*shrink, r->h - 2*shrink);
}

INLINE struct rect2di* rect2di_grow(struct rect2di* rr, const struct rect2di* r, int grow)
{
    return rect2di_seti(rr, r->x - grow, r->y - grow, r->w + 2*grow, r->h + 2*grow);
}

INLINE int rect2di_testpt(const struct rect2di* rc, const struct vec2i* pt)
{
    return (pt->x > rc->x)&(pt->x < (rc->x + rc->w))&(pt->y > rc->y)&(pt->y < (rc->y + rc->h));
}

INLINE int rect2di_testpti(const struct rect2di* rc, int x, int y)
{
    return (x > rc->x)&(x < (rc->x + rc->w))&(y > rc->y)&(y < (rc->y + rc->h));
}


INLINE struct rect2df* rect2df_setf(struct rect2df* rc, float x, float y, float w, float h)
{
    rc->x = x;
    rc->y = y;
    rc->w = w;
    rc->h = h;
    return rc;
}

INLINE int rect2df_isequal(const struct rect2df rc1, const struct rect2df rc2)
{
	return (rc1.x == rc2.x && rc1.y == rc2.y && rc1.w == rc2.w && rc1.h == rc2.h);
}

INLINE struct rect2df* rect2df_shrink(struct rect2df* rr, const struct rect2df* r, float shrink)
{
    return rect2df_setf(rr, r->x + shrink, r->y + shrink, r->w - 2.0f*shrink, r->h - 2.0f*shrink);
}

INLINE struct rect2df* rect2df_setr(struct rect2df *rr, const struct rect2df *r)
{
    rr->x = r->x;
    rr->y = r->y;
    rr->w = r->w;
    rr->h = r->h;
    return rr;
}

INLINE struct rect2df* rect2df_grow(struct rect2df* rr, const struct rect2df* r, float grow)
{
    return rect2df_setf(rr, r->x - grow, r->y - grow, r->w + 2.0f*grow, r->h + 2.0f*grow);
}

INLINE int rect2df_testpt(const struct rect2df* rc, const struct vec2f* pt)
{
    return (pt->x > rc->x)&(pt->x < (rc->x + rc->w))&(pt->y > rc->y)&(pt->y < (rc->y + rc->h));
}

INLINE int rect2df_testptf(const struct rect2df* rc, float x, float y)
{
    return (x > rc->x)&(x < (rc->x + rc->w))&(y > rc->y)&(y < (rc->y + rc->h));
}

INLINE struct sphere* sphere_setzero(struct sphere* s)
{
    s->x = 0.0f;
    s->y = 0.0f;
    s->z = 0.0f;
    s->r = 0.0f;
    return s;
}

INLINE struct sphere* sphere_setf(struct sphere* s, float x, float y, float z, float r)
{
    s->x = x;
    s->y = y;
    s->z = z;
    s->r = r;
    return s;
}

INLINE struct sphere* sphere_sets(struct sphere* r, const struct sphere* s)
{
    r->x = s->x;
    r->y = s->y;
    r->z = s->z;
    r->r = s->r;
    return r;
}

INLINE struct sphere* sphere_from_aabb(struct sphere* rs, const struct aabb* b)
{
    struct vec4f v;
    vec3_muls(&v, vec3_add(&v, &b->minpt, &b->maxpt), 0.5f);
    rs->x = v.x;
    rs->y = v.y;
    rs->z = v.z;
    rs->r = vec3_len(vec3_sub(&v, &b->maxpt, &b->minpt)) * 0.5f;
    return rs;
}

INLINE struct aabb* aabb_setf(struct aabb* r, float min_x, float min_y, float min_z,
    float max_x, float max_y, float max_z)
{
    vec3_setf(&r->minpt, min_x, min_y, min_z);
    vec3_setf(&r->maxpt, max_x, max_y, max_z);
    return r;
}

INLINE struct aabb* aabb_setb(struct aabb* r, const struct aabb* b)
{
    vec4_setv(&r->minpt, &b->minpt);
    vec4_setv(&r->maxpt, &b->maxpt);
    return r;
}

INLINE struct aabb* aabb_setv(struct aabb* r, const struct vec4f* minpt, const struct vec4f* maxpt)
{
    vec4_setv(&r->minpt, minpt);
    vec4_setv(&r->maxpt, maxpt);
    return r;
}

INLINE struct aabb* aabb_setzero(struct aabb* r)
{
    vec3_setf(&r->minpt, FL32_MAX, FL32_MAX, FL32_MAX);
    vec3_setf(&r->maxpt, -FL32_MAX, -FL32_MAX, -FL32_MAX);
    return r;
}

INLINE int aabb_iszero(const struct aabb* b)
{
    return (b->minpt.x == FL32_MAX && b->minpt.y == FL32_MAX && b->minpt.z == FL32_MAX) &&
           (b->maxpt.x == -FL32_MAX && b->maxpt.y == -FL32_MAX && b->maxpt.z == -FL32_MAX);
}

/*
 *            6                                7
 *              ------------------------------
 *             /|                           /|
 *            / |                          / |
 *           /  |                         /  |
 *          /   |                        /   |
 *         /    |                       /    |
 *        /     |                      /     |
 *       /      |                     /      |
 *      /       |                    /       |
 *     /        |                   /        |
 *  2 /         |                3 /         |
 *   /----------------------------/          |
 *   |          |                 |          |
 *   |          |                 |          |      +Y
 *   |        4 |                 |          |
 *   |          |-----------------|----------|      |
 *   |         /                  |         /  5    |
 *   |        /                   |        /        |       +Z
 *   |       /                    |       /         |
 *   |      /                     |      /          |     /
 *   |     /                      |     /           |    /
 *   |    /                       |    /            |   /
 *   |   /                        |   /             |  /
 *   |  /                         |  /              | /
 *   | /                          | /               |/
 *   |/                           |/                ----------------- +X
 *   ------------------------------
 *  0                              1
 */
INLINE struct vec4f* aabb_getpt(struct vec4f* r, const struct aabb* b, int idx)
{
    ASSERT(idx < 8);
    return vec3_setf(r, (idx&1) ? b->maxpt.x : b->minpt.x,
                        (idx&2) ? b->maxpt.y : b->minpt.y,
                        (idx&4) ? b->maxpt.z : b->minpt.z);
}

INLINE struct vec4f* aabb_getptarr(struct vec4f* rs, const struct aabb* b)
{
    vec3_setv(&rs[0], &b->minpt);
    vec3_setf(&rs[1], b->maxpt.x, b->minpt.y, b->minpt.z);
    vec3_setf(&rs[2], b->minpt.x, b->maxpt.y, b->minpt.z);
    vec3_setf(&rs[3], b->maxpt.x, b->maxpt.y, b->minpt.z);
    vec3_setf(&rs[4], b->minpt.x, b->minpt.y, b->maxpt.z);
    vec3_setf(&rs[5], b->maxpt.x, b->minpt.y, b->maxpt.z);
    vec3_setf(&rs[6], b->minpt.x, b->maxpt.y, b->maxpt.z);
    vec3_setv(&rs[7], &b->maxpt);
    return rs;
}

INLINE void aabb_pushptv(struct aabb* rb, const struct vec4f* pt)
{
    if (pt->x < rb->minpt.x)        rb->minpt.x = pt->x;
    if (pt->x > rb->maxpt.x)        rb->maxpt.x = pt->x;
    if (pt->y < rb->minpt.y)        rb->minpt.y = pt->y;
    if (pt->y > rb->maxpt.y)        rb->maxpt.y = pt->y;
    if (pt->z < rb->minpt.z)        rb->minpt.z = pt->z;
    if (pt->z > rb->maxpt.z)        rb->maxpt.z = pt->z;
}

INLINE void aabb_pushptf(struct aabb* rb, float x, float y, float z)
{
    if (x < rb->minpt.x)        rb->minpt.x = x;
    if (x > rb->maxpt.x)        rb->maxpt.x = x;
    if (y < rb->minpt.y)        rb->minpt.y = y;
    if (y > rb->maxpt.y)        rb->maxpt.y = y;
    if (z < rb->minpt.z)        rb->minpt.z = z;
    if (z > rb->maxpt.z)        rb->maxpt.z = z;
}

INLINE float aabb_getwidth(const struct aabb* bb)
{
    return bb->maxpt.x - bb->minpt.x;
}

INLINE float aabb_getheight(const struct aabb* bb)
{
    return bb->maxpt.y - bb->minpt.y;
}

INLINE float aabb_getdepth(const struct aabb* bb)
{
    return bb->maxpt.z - bb->minpt.z;
}

INLINE struct aabb* aabb_from_sphere(struct aabb* rb, const struct sphere* s)
{
    vec3_setf(&rb->minpt, FL32_MAX, FL32_MAX, FL32_MAX);
    vec3_setf(&rb->maxpt, -FL32_MAX, -FL32_MAX, -FL32_MAX);
    aabb_pushptf(rb, s->x + s->r, s->y + s->r, s->z + s->r);
    aabb_pushptf(rb, s->x - s->r, s->y - s->r, s->z - s->r);
    return rb;
}

// AABB-2d
INLINE struct aabb2d* aabb2d_setf(struct aabb2d* r,
                                  float min_x, float min_y,
                                  float max_x, float max_y)
{
    vec2f_setf(&r->minpt, min_x, min_y);
    vec2f_setf(&r->maxpt, max_x, max_y);
    return r;
}

INLINE struct aabb2d* aabb2d_setb(struct aabb2d* r, const struct aabb2d* b)
{
    vec2f_setv(&r->minpt, &b->minpt);
    vec2f_setv(&r->maxpt, &b->maxpt);
    return r;
}

INLINE struct aabb2d* aabb2d_setv(struct aabb2d* r, const struct vec2f* minpt, const struct vec2f* maxpt)
{
    vec2f_setv(&r->minpt, minpt);
    vec2f_setv(&r->maxpt, maxpt);
    return r;
}

INLINE struct aabb2d* aabb2d_setzero(struct aabb2d* r)
{
    vec2f_setf(&r->minpt, FL32_MAX, FL32_MAX);
    vec2f_setf(&r->maxpt, -FL32_MAX, -FL32_MAX);
    return r;
}

INLINE int aabb2d_iszero(const struct aabb2d* b)
{
    return (b->minpt.x == FL32_MAX && b->minpt.y == FL32_MAX) &&
           (b->maxpt.x == -FL32_MAX && b->maxpt.y == -FL32_MAX);
}

INLINE int sphere_ptinv(const struct sphere* s, const struct vec4f* pt)
{
    struct vec4f d;
    vec3_setf(&d, pt->x - s->x, pt->y - s->y, pt->z - s->z);
    return ((vec3_dot(&d, &d) - s->r*s->r) < EPSILON);
}

INLINE int sphere_ptinf(const struct sphere* s, float x, float y, float z)
{
    struct vec4f d;
    vec3_setf(&d, x - s->x, y - s->y, z - s->z);
    return ((vec3_dot(&d, &d) - s->r*s->r) < EPSILON);
}

/*  */
CORE_API struct sphere* sphere_circum(struct sphere* rs, const struct vec4f* v0,
		const struct vec4f* v1, const struct vec4f* v2, const struct vec4f* v3);
CORE_API struct sphere* sphere_merge(struct sphere* rs, const struct sphere* s1,
		const struct sphere* s2);
CORE_API struct aabb* aabb_merge(struct aabb* rb, const struct aabb* b1, const struct aabb* b2);
CORE_API struct aabb* aabb_xform(struct aabb* rb, const struct aabb* b, const struct mat3f* mat);
CORE_API struct sphere* sphere_xform(struct sphere* rs, const struct sphere* s,
		const struct mat3f* m);
CORE_API int sphere_intersects(const struct sphere* s1, const struct sphere* s2);

/**
 * intersects plane with ray
 * @return a floating point number which we can put in ray equation and find a point where \
 * intersection happens, or FL32_MAX if no intersection
 */
CORE_API float ray_intersect_plane(const struct ray* r, const struct plane* p);

#ifdef __cplusplus

namespace dh {


class ALIGN16 Sphere
{
private:
    sphere m_sphere;

public:
    Sphere()
    {
        sphere_setf(&m_sphere, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    Sphere(float x, float y, float z, float r)
    {
        sphere_setf(&m_sphere, x, y, z, r);
    }

    Sphere(const Vec3& center, float r)
    {
        sphere_setf(&m_sphere, center.x(), center.y(), center.z(), r);
    }

    Sphere(const sphere &s)
    {
        m_sphere = s;
    }

    Sphere& operator=(const Sphere &s)
    {
        sphere_sets(&m_sphere, &s.m_sphere);
        return *this;
    }

    Sphere& set(float x, float y, float z, float r)
    {
        sphere_setf(&m_sphere, x, y, z, r);
        return *this;
    }

    Sphere& set(const Vec3 &center, float r)
    {
        sphere_setf(&m_sphere, center.x(), center.y(), center.z(), r);
        return *this;
    }

    Sphere& set(const sphere &s)
    {
        m_sphere = s;
        return *this;
    }

    Sphere& from_aabb(const aabb *box)
    {
        sphere_from_aabb(&m_sphere, box);
        return *this;
    }

    bool point_in(const Vec3 &pt)
    {
        return sphere_ptinv(&m_sphere, pt);
    }

    bool point_in(float x, float y, float z)
    {
        return sphere_ptinf(&m_sphere, x, y, z);
    }

    static Sphere merge(const Sphere &sphere0, const Sphere &sphere1)
    {
        Sphere r;
        sphere_merge(&r.m_sphere, &sphere0.m_sphere, &sphere1.m_sphere);
        return r;
    }

    static Sphere circum(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3)
    {
        Sphere r;
        sphere_circum(&r.m_sphere, p0, p1, p2, p3);
        return r;
    }

    static bool intersects(const Sphere &sphere0, const Sphere &sphere1)
    {
        return sphere_intersects(&sphere0.m_sphere, &sphere1.m_sphere);
    }

    Sphere operator+(const Sphere& s) const
    {
        Sphere r;
        sphere_merge(&r.m_sphere, &m_sphere, &s.m_sphere);
        return r;
    }

    Sphere& operator+=(const Sphere& s)
    {
        sphere_merge(&m_sphere, &m_sphere, &s.m_sphere);
        return *this;
    }

    float x() const {   return m_sphere.x; }
    float y() const {   return m_sphere.y; }
    float z() const {   return m_sphere.z; }
    float r() const {   return m_sphere.r; }

    operator sphere*() { return &m_sphere; }
    operator const sphere*() const {  return &m_sphere;   }
    operator float*() { return m_sphere.f; }
    operator const float*() { return m_sphere.f; }
};

class ALIGN16 AABB
{
private:
    aabb m_aabb;

public:
    AABB()
    {
        aabb_setzero(&m_aabb);
    }

    AABB(const Vec3& minpt, const Vec3& maxpt)
    {
        aabb_setv(&m_aabb, minpt, maxpt);
    }

    AABB(const aabb &box)
    {
        m_aabb = box;
    }

    AABB& set(const Vec3 &minpt, const Vec3 &maxpt)
    {
        aabb_setv(&m_aabb, minpt, maxpt);
        return *this;
    }

    AABB& operator=(const AABB &box)
    {
        aabb_setb(&m_aabb, &box.m_aabb);
        return *this;
    }

    AABB& set(const aabb &box)
    {
        m_aabb = box;
        return *this;
    }

    Vec3 min() const
    {
        return Vec3(m_aabb.minpt);
    }

    Vec3 max() const
    {
        return Vec3(m_aabb.maxpt);
    }

    Vec3 corner(int idx) const
    {
        Vec3 r;
        aabb_getpt(r, &m_aabb, idx);
        return r;
    }

    Vec3 center() const
    {
        return (this->min() + this->max())*0.5f;
    }

    static AABB transform(const AABB& box, const Mat3& mat)
    {
        AABB r;
        aabb_xform(&r.m_aabb, &box.m_aabb, mat);
        return r;
    }

    AABB& set_transform(const Mat3& mat)
    {
        aabb_xform(&m_aabb, &m_aabb, mat);
        return *this;
    }

    AABB& set_empty()
    {
        aabb_setzero(&m_aabb);
        return *this;
    }

    bool empty() const
    {
        return aabb_iszero(&m_aabb);
    }

    void add_point(const Vec3& pt)
    {
        aabb_pushptv(&m_aabb, pt);
    }

    void add_point(float x, float y, float z)
    {
        aabb_pushptf(&m_aabb, x, y, z);
    }

    float width() const
    {
        return aabb_getwidth(&m_aabb);
    }

    float height() const
    {
        return aabb_getheight(&m_aabb);
    }

    float depth() const
    {
        return aabb_getdepth(&m_aabb);
    }

    static AABB merge(const AABB& box0, const AABB& box1)
    {
        AABB r;
        aabb_merge(&r.m_aabb, &box0.m_aabb, &box1.m_aabb);
        return r;
    }

    AABB& from_sphere(const Sphere& s)
    {
        aabb_from_sphere(&m_aabb, s);
        return *this;
    }

    AABB operator+(const AABB& box) const
    {
        AABB r;
        aabb_merge(&r.m_aabb, &m_aabb, &box.m_aabb);
        return r;
    }

    AABB& operator+=(const AABB& box)
    {
        aabb_merge(&m_aabb, &m_aabb, &box.m_aabb);
        return *this;
    }

    operator aabb*() {  return &m_aabb; }
    operator const aabb*() const  { return &m_aabb; }
};

class ALIGN16 AABB2
{
private:
    aabb2d m_aabb;

public:
    AABB2()
    {
        aabb2d_setzero(&m_aabb);
    }

    AABB2(const Vec2& minpt, const Vec2& maxpt)
    {
        aabb2d_setv(&m_aabb, minpt, maxpt);
    }

    AABB2(const aabb2d &box)
    {
        m_aabb = box;
    }

    AABB2& set(const Vec2 &minpt, const Vec2 &maxpt)
    {
        aabb2d_setv(&m_aabb, minpt, maxpt);
        return *this;
    }

    AABB2& operator=(const AABB2 &box)
    {
        aabb2d_setb(&m_aabb, &box.m_aabb);
        return *this;
    }

    AABB2& set(const aabb2d &box)
    {
        m_aabb = box;
        return *this;
    }

    Vec2 min() const
    {
        return Vec2(m_aabb.minpt);
    }

    Vec2 max() const
    {
        return Vec2(m_aabb.maxpt);
    }
};


class ALIGN16 Plane
{
private:
    plane m_plane;

public:
    Plane()
    {
        m_plane.nx = 0.0f;
        m_plane.ny = 1.0f;
        m_plane.nz = 0.0f;
        m_plane.d = 0.0f;
    }

    Plane(const Vec3& norm, float d)
    {
        plane_setv(&m_plane, norm, d);
    }
    Plane(float nx, float ny, float nz, float d)
    {
        plane_setf(&m_plane, nx, ny, nz, d);
    }

    Plane(const plane &pl)
    {
        m_plane = pl;
    }

    Plane& operator=(const Plane &p)
    {
        plane_setf(&m_plane, p.m_plane.nx, p.m_plane.ny, p.m_plane.nz, p.m_plane.d);
        return *this;
    }

    Plane& set(const plane &pl)
    {
        m_plane = pl;
        return *this;
    }

    Plane& set(const Vec3& norm, float d)
    {
        plane_setv(&m_plane, norm, d);
        return *this;
    }

    Plane& set(float nx, float ny, float nz, float d)
    {
        plane_setf(&m_plane, nx, ny, nz, d);
        return *this;
    }

    Vec3 normal() const
    {
        return Vec3(m_plane.nx, m_plane.ny, m_plane.nz);
    }

    float d() const
    {
        return m_plane.d;
    }

    void set_normalize()
    {
        Vec3 nd(m_plane.nx, m_plane.ny, m_plane.nz);
        float nlen = nd.length();
        m_plane.nx = nd.x() / nlen;
        m_plane.ny = nd.y() / nlen;
        m_plane.nz = nd.z() / nlen;
        m_plane.d /= nlen;
    }

    operator plane*() { return &m_plane; }
    operator const plane*() const { return &m_plane; }
};

class ALIGN16 Ray
{
private:
    ray m_ray;

public:
    Ray()
    {
        vec3_setf(&m_ray.pt, 0.0f, 0.0f, 0.0f);
        vec3_setf(&m_ray.dir, 0.0f, 0.0f, 1.0f);
    }

    Ray(const Vec3& pt, const Vec3& dir)
    {
        ray_setv(&m_ray, pt, dir);
    }

    Ray(const ray &r)
    {
        m_ray = r;
    }

    Ray& set(const Vec3 &pt, const Vec3 &dir)
    {
        ray_setv(&m_ray, pt, dir);
        return *this;
    }

    Ray& set(const ray &r)
    {
        m_ray = r;
        return *this;
    }

    Vec3 point() const
    {
        return Vec3(m_ray.pt);
    }

    Vec3 direction() const
    {
        return Vec3(m_ray.dir);
    }

    float intersect_plane(const Plane& p)
    {
        return ray_intersect_plane(&m_ray, p);
    }

    operator ray*() { return &m_ray; }
    operator const ray*() const { return &m_ray; }
};

class ALIGN16 Rect
{
private:
    rect2di m_rc;

public:
    static const Rect Zero;

public:
    Rect()
    {
        rect2di_seti(&m_rc, 0, 0, 0, 0);
    }

    Rect(int x, int y, int width, int height)
    {
        rect2di_seti(&m_rc, x, y, width, height);
    }

    Rect(const rect2di rc)
    {
        m_rc = rc;
    }

    Rect& operator=(const Rect &r)
    {
        rect2di_setr(&m_rc, &r.m_rc);
        return *this;
    }

    bool operator==(const Rect &r) const
    {
        return  m_rc.x == r.m_rc.x &&
                m_rc.y == r.m_rc.y &&
                m_rc.w == r.m_rc.w &&
                m_rc.h == r.m_rc.h;
    }

    Rect& set(int x, int y, int width, int height)
    {
        rect2di_seti(&m_rc, x, y, width, height);
        return *this;
    }

    Rect& set(const rect2di rc)
    {
        m_rc = rc;
        return *this;
    }

    Rect& set_region(int left, int top, int right, int bottom)
    {
        rect2di_seti(&m_rc, left, top, right-left, bottom-top);
        return *this;
    }

    int x() const   { return m_rc.x;    }
    int y() const   { return m_rc.y;    }
    int top() const { return m_rc.y;    }
    int bottom() const { return m_rc.y + m_rc.h; }
    int left() const { return m_rc.x; }
    int right() const { return m_rc.x + m_rc.w; }
    int width() const { return m_rc.w; }
    int height() const { return m_rc.h; }

    bool point_in(const Vec2i &pt)
    {
        return rect2di_testpt(&m_rc, pt);
    }

    bool point_in(int x, int y)
    {
        return rect2di_testpti(&m_rc, x, y);
    }

    Rect& grow(int value)
    {
        rect2di_grow(&m_rc, &m_rc, value);
        return *this;
    }

    Rect& shrink(int value)
    {
        rect2di_shrink(&m_rc, &m_rc, value);
        return *this;
    }

    operator int*() { return m_rc.n; }
    operator const int*() const { return m_rc.n; }
    operator rect2di*() { return &m_rc; }
    operator const rect2di*() const { return &m_rc; }
};

class ALIGN16 Rectf
{
private:
    rect2df m_rc;

public:
    static const Rectf Zero;

public:
    Rectf()
    {
        rect2df_setf(&m_rc, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    Rectf(float x, float y, float width, float height)
    {
        rect2df_setf(&m_rc, x, y, width, height);
    }

    Rectf(const rect2df rc)
    {
        m_rc = rc;
    }

    Rectf& operator=(const Rectf &rc)
    {
        rect2df_setr(&m_rc, &rc.m_rc);
        return *this;
    }

    bool operator==(const Rectf &r) const
    {
        return  m_rc.x == r.m_rc.x &&
                m_rc.y == r.m_rc.y &&
                m_rc.w == r.m_rc.w &&
                m_rc.h == r.m_rc.h;
    }

    Rectf& set(float x, float y, float width, float height)
    {
        rect2df_setf(&m_rc, x, y, width, height);
        return *this;
    }

    Rectf& set(const rect2df rc)
    {
        m_rc = rc;
        return *this;
    }

    Rectf& set_region(float left, float top, float right, float bottom)
    {
        rect2df_setf(&m_rc, left, top, right-left, bottom-top);
        return *this;
    }

    float x() const   { return m_rc.x;    }
    float y() const   { return m_rc.y;    }
    float top() const { return m_rc.y;    }
    float bottom() const { return m_rc.y + m_rc.h; }
    float left() const { return m_rc.x; }
    float right() const { return m_rc.x + m_rc.w; }
    float width() const { return m_rc.w; }
    float height() const { return m_rc.h; }

    bool point_in(const Vec2& pt) const
    {
        return rect2df_testpt(&m_rc, pt);
    }

    bool point_in(float x, float y) const
    {
        return rect2df_testptf(&m_rc, x, y);
    }

    Rectf& grow(float value)
    {
        rect2df_grow(&m_rc, &m_rc, value);
        return *this;
    }

    Rectf& shrink(float value)
    {
        rect2df_shrink(&m_rc, &m_rc, value);
        return *this;
    }

    operator float*() { return m_rc.f; }
    operator const float*() const { return m_rc.f; }
    operator rect2df*() { return &m_rc; }
    operator const rect2df*() const { return &m_rc; }
};


} /* dh */
#endif

#endif /* PRIMS_H */
