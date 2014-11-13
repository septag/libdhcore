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

 /**
 * @defgroup vmath Vector Math
 */

 /**
 * common vector-math functions and definitions, vec4f, quat4f, mat3f, ...\n
 * there are different type of implementations for each cpu architecture:\n
 * #1: generic fpu implementation\n
 * #2: x86-64 SSE implementation, which can be activated by defining _SIMD_SSE_ preprocessor\n
 * many of vector-math functions returns pointer to return value provided as 'r' in the first -\n
 * paramter, so they can be combined and used in the single statement
 * @ingroup vmath
 */

#ifndef __VECMATH_H__
#define __VECMATH_H__

#include "types.h"
#include "core-api.h"
#include "std-math.h"
#include "mem-mgr.h"
#include "allocator.h"

#if defined(_MSVC_)
#pragma warning(disable: 662)
#endif

/* check for SSE validity */
#if !(defined(_X86_) || defined(_X64_)) && defined(_SIMD_SSE_)
#error "SSE SIMD instructions not available in any platform except x86/x86-64"
#endif

/* SIMD - SSE */
#if defined(_SIMD_SSE_)
#include <xmmintrin.h>
#include <emmintrin.h>
typedef __m128  simd_t;
typedef __m128i simd4i_t;

#define _mm_all_x(v)    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(0, 0, 0, 0))
#define _mm_all_y(v)    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(1, 1, 1, 1))
#define _mm_all_z(v)    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(2, 2, 2, 2))
#define _mm_all_w(v)    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(3, 3, 3, 3))
/* madd (mul+add): r = v1*v2 + v3 */
#define _mm_madd(v1, v2, v3) _mm_add_ps(_mm_mul_ps((v1), (v2)), (v3))
#endif

/**
 * 2-component integer vector, mainly used for screen-space operations
 * @ingroup vmath
 */
struct vec2i
{
	union	{
		struct	{
			int   x;
			int   y;
		};

		int n[2];
	};
};

/**
 * 2-component float vector
 * @ingroup vmath
 */
struct vec2f
{
	union	{
		struct	{
			float    x;
			float    y;
		};

		float f[2];
	};
};


/**
 * 4-component vector\n
 * can be used as 3-component (float3) or 4-component(float4) and also color (rgba) types
 * @ingroup vmath
 */

struct ALIGN16 vec4f
{
    union  {
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        float f[4];
    };
};

struct ALIGN16 vec4i
{
	union	{
		struct {
			int x;
			int y;
			int z;
			int w;
		};

		int n[4];
	};
};

#define vec3f vec4f

/* vec3 globals */
#if defined(_GNUC_) && !defined(__cplusplus)
static const struct vec3f g_vec3_zero = {.x=0.0f, .y=0.0f, .z=0.0f, .w=1.0f};
static const struct vec3f g_vec3_unitx = {.x=1.0f, .y=0.0f, .z=0.0f, .w=1.0f};
static const struct vec3f g_vec3_unity = {.x=0.0f, .y=1.0f, .z=0.0f, .w=1.0f};
static const struct vec3f g_vec3_unitz = {.x=0.0f, .y=0.0f, .z=1.0f, .w=1.0f};
static const struct vec3f g_vec3_unitx_neg = {.x=-1.0f, .y=0.0f, .z=0.0f, .w=1.0f};
static const struct vec3f g_vec3_unity_neg = {.x=0.0f, .y=-1.0f, .z=0.0f, .w=1.0f};
static const struct vec3f g_vec3_unitz_neg = {.x=0.0f, .y=0.0f, .z=-1.0f, .w=1.0f};
#else
static const struct vec3f g_vec3_zero = {0.0f, 0.0f, 0.0f, 1.0f};
static const struct vec3f g_vec3_unitx = {1.0f, 0.0f, 0.0f, 1.0f};
static const struct vec3f g_vec3_unity = {0.0f, 1.0f, 0.0f, 1.0f};
static const struct vec3f g_vec3_unitz = {0.0f, 0.0f, 1.0f, 1.0f};
static const struct vec3f g_vec3_unitx_neg = {-1.0f, 0.0f, 0.0f, 1.0f};
static const struct vec3f g_vec3_unity_neg = {0.0f, -1.0f, 0.0f, 1.0f};
static const struct vec3f g_vec3_unitz_neg = {0.0f, 0.0f, -1.0f, 1.0f};
#endif

struct ALIGN16 mat2f
{
    union   {
        struct {
            float m11, m12, m13;
            float m21, m22, m23;
            float m31, m32, m33;
        };

        struct {
            float row1[3];
            float row2[3];
            float row3[3];
        };

        float f[9];
    };

};

/**
 * row-major 4x3 matrix \n
 * row-major representation: m(row)(col)\n
 * @ingroup vmath
 */
struct ALIGN16 mat3f
{
    union   {
        struct {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };

        struct {
            float row1[4];
            float row2[4];
            float row3[4];
            float row4[4];
        };

        float    f[16];
    };
};

/**
 * row-major 4x4 matrix \n
 * but the representation is still row-major
 * @ingroup vmath
 */
struct ALIGN16 mat4f
{
    union   {
        struct {
            float m11, m12, m13, m14;    /* row #1 */
            float m21, m22, m23, m24;    /* row #2 */
            float m31, m32, m33, m34;    /* row #3 */
            float m41, m42, m43, m44;    /* row #4 */
        };

        struct {
            float row1[4];
            float row2[4];
            float row3[4];
            float row4[4];
        };
        float    f[16];
   };
};

/**
 * simd friendly array of vectors
 * stores array of vectors 16byte aligned and structure of arrays
 * @ingroup vmath
 */
struct ALIGN16 vec4f_simd
{
    struct allocator* alloc;
    uint cnt;
    float* xs;
    float* ys;
    float* zs;
    float* ws;
};

/**
 * simd friendly column-major 4x4 matrix
 * store four values for each matrix
 * @ingroup vmath
 */
struct ALIGN16 mat4f_simd
{
    union   {
        struct {
            float m11[4], m21[4], m31[4], m41[4];    /* col #1 */
            float m12[4], m22[4], m32[4], m42[4];    /* col #2 */
            float m13[4], m23[4], m33[4], m43[4];    /* col #3 */
            float m14[4], m24[4], m34[4], m44[4];    /* col #4 */
        };

        float f[4][4][4];
    };
};


/**
 * quaternion - 4 components
 * @ingroup vmath
 */
struct ALIGN16 quat4f
{
    union   {
        struct {
            float x, y, z, w;
        };
        float f[4];
    };
};

/**
 * transform - includes everything that represents object transform (rotation/position)
 * @ingroup vmath
 */
struct ALIGN16 xform3d
{
	struct vec4f p;
	struct quat4f q;
};


/* vec2i functions
 **
 * @ingroup vmath
 */
INLINE struct vec2i* vec2i_seti(struct vec2i* v, int x, int y)
{
    v->x = x;
    v->y = y;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2i* vec2i_setv(struct vec2i* r, const struct vec2i* v)
{
    r->x = v->x;
    r->y = v->y;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2i* vec2i_setvp(struct vec2i* r, const int* f)
{
    r->x = f[0];
    r->y = f[1];
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2i* vec2i_setzero(struct vec2i* r)
{
    r->x = 0;
    r->y = 0;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2i* vec2i_add(struct vec2i* v, const struct vec2i* v1, const struct vec2i* v2)
{
    v->x = v1->x + v2->x;
    v->y = v1->y + v2->y;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2i* vec2i_sub(struct vec2i* v, const struct vec2i* v1, const struct vec2i* v2)
{
    v->x = v1->x - v2->x;
    v->y = v1->y - v2->y;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE int vec2i_dot(const struct vec2i* v1, const struct vec2i* v2)
{
    return v1->x*v2->x + v1->y*v2->y;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2i* vec2i_muls(struct vec2i* v, const struct vec2i* v1, int k)
{
    v->x = v1->x * k;
    v->y = v1->y * k;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE int vec2i_isequal(const struct vec2i* v1, const struct vec2i* v2)
{
    return ((v1->x == v2->x) && (v1->y == v2->y));
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2f_setf(struct vec2f* v, float x, float y)
{
    v->x = x;
    v->y = y;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2f_setv(struct vec2f* r, const struct vec2f* v)
{
    r->x = v->x;
    r->y = v->y;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2f_setvp(struct vec2f* r, const float* f)
{
    r->x = f[0];
    r->y = f[1];
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2f_setzero(struct vec2f* r)
{
    r->x = 0.0f;
    r->y = 0.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2f_add(struct vec2f* v, const struct vec2f* v1, const struct vec2f* v2)
{
    v->x = v1->x + v2->x;
    v->y = v1->y + v2->y;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2f_sub(struct vec2f* v, const struct vec2f* v1, const struct vec2f* v2)
{
    v->x = v1->x - v2->x;
    v->y = v1->y - v2->y;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2f_muls(struct vec2f* v, const struct vec2f* v1, float k)
{
    v->x = v1->x * k;
    v->y = v1->y * k;
    return v;
}

/**
 * @ingroup vmath
 */
INLINE float vec2f_len(const struct vec2f* v)
{
    return sqrt((v->x*v->x) + (v->y*v->y));
}

/**
 * @ingroup vmath
 */
INLINE int vec2f_isequal(const struct vec2f* v1, const struct vec2f* v2)
{
    return (math_isequal(v1->x, v2->x) && math_isequal(v1->y, v2->y));
}

/* vec3 functions
 **
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_setf(struct vec4f* r, float x, float y, float z)
{
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_setv(struct vec4f* r, const struct vec4f* v)
{
    r->x = v->x;
    r->y = v->y;
    r->z = v->z;
    r->w = 1.0f;
    return r;
}

INLINE struct vec4f* vec3_setvp(struct vec3f* r, const float* fv)
{
    r->x = fv[0];
    r->y = fv[1];
    r->z = fv[2];
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_setzero(struct vec4f* r)
{
    r->x = 0.0f;
    r->y = 0.0f;
    r->z = 0.0f;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_setunitx(struct vec4f* r)
{
    r->x = 1.0f;
    r->y = 0.0f;
    r->z = 0.0f;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_setunity(struct vec4f* r)
{
    r->x = 0.0f;
    r->y = 1.0f;
    r->z = 0.0f;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_setunitz(struct vec4f* r)
{
    r->x = 0.0f;
    r->y = 0.0f;
    r->z = 1.0f;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_add(struct vec4f* r, const struct vec4f* v1, const struct vec4f* v2)
{
    r->x = v1->x + v2->x;
    r->y = v1->y + v2->y;
    r->z = v1->z + v2->z;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_sub(struct vec4f* r, const struct vec4f* v1, const struct vec4f* v2)
{
    r->x = v1->x - v2->x;
    r->y = v1->y - v2->y;
    r->z = v1->z - v2->z;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_muls(struct vec4f* r, const struct vec4f* v1, float k)
{
    r->x = v1->x * k;
    r->y = v1->y * k;
    r->z = v1->z * k;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE float vec3_dot(const struct vec4f* v1, const struct vec4f* v2)
{
    return (v1->x*v2->x + v1->y*v2->y + v1->z*v2->z);
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_cross(struct vec4f* r, const struct vec4f* v1, const struct vec4f* v2)
{
    return vec3_setf(r,
                     v1->y*v2->z - v1->z*v2->y,
                     v1->z*v2->x - v1->x*v2->z,
                     v1->x*v2->y - v1->y*v2->x);
}

/**
 * @ingroup vmath
 */
INLINE int vec3_isequal(const struct vec4f* v1, const struct vec4f* v2)
{
    return (math_isequal(v1->x, v2->x) && math_isequal(v1->y, v2->y) && math_isequal(v1->z, v2->z));
}

/**
 * @ingroup vmath
 */
INLINE float vec3_len(const struct vec4f* v)
{
    return sqrt(vec3_dot(v, v));
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec3_norm(struct vec4f* r, const struct vec4f* v)
{
    return vec3_muls(r, v, 1.0f/vec3_len(v));
}

/**
 * @ingroup vmath
 */
CORE_API float vec3_angle(const struct vec4f* v1, const struct vec4f* v2);

/**
 * lerps (linear-interpolation) between two vectors in space
 * @param t interpolator value between 0~1
 * @ingroup vmath
 */
CORE_API struct vec4f* vec3_lerp(struct vec4f* r, const struct vec4f* v1,
                                 const struct vec4f* v2, float t);

/**
 * Cubic interpolation between two vectors \n
 * Cubic interpolation needs 4 pointrs, two line segments that we should interpolate (v1, v2) ...\n
 * A point before the segment (v0) and a point after segment (v3)
 * @param t interpolator value which is between [0, 1]
 * @ingroup vmath
 */
CORE_API struct vec4f* vec3_cubic(struct vec4f* r, const struct vec4f* v0, const struct vec4f* v1,
    const struct vec4f* v2, const struct vec4f* v3, float t);

/**
 * transform vector3(x,y,z) by SRT(scale/rotation/translate) matrix, which is normally mat3f
 * @ingroup vmath
 */
CORE_API struct vec4f* vec3_transformsrt(struct vec4f* r, const struct vec4f* v,
                                         const struct mat3f* m);

/**
 * transform vector3(x,y,z) by SRT(scale/rotation/translate) matrix4x4
 * @ingroup vmath
 */
CORE_API struct vec4f* vec3_transformsrt_m4(struct vec4f* r, const struct vec4f* v,
    const struct mat4f* m);

/**
 * transform vector3(x,y,z) by SRT(scale/rotation) portion of the matrix, which is normally mat3f
 * @ingroup vmath
 */
CORE_API struct vec4f* vec3_transformsr(struct vec4f* r, const struct vec4f* v,
                                         const struct mat3f* m);

/* vec4 functions
 **
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_setf(struct vec4f* r, float x, float y, float z, float w)
{
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_setv(struct vec4f* r, const struct vec4f* v)
{
    r->x = v->x;
    r->y = v->y;
    r->z = v->z;
    r->w = v->w;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_setvp(struct vec3f* r, const float* fv)
{
    r->x = fv[0];
    r->y = fv[1];
    r->z = fv[2];
    r->w = fv[3];
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_setzero(struct vec4f* r)
{
    r->x = 0.0f;
    r->y = 0.0f;
    r->z = 0.0f;
    r->w = 0.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_add(struct vec4f* r, const struct vec4f* v1, const struct vec4f* v2)
{
    r->x = v1->x + v2->x;
    r->y = v1->y + v2->y;
    r->z = v1->z + v2->z;
    r->w = v1->w + v2->w;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_sub(struct vec4f* r, const struct vec4f* v1, const struct vec4f* v2)
{
    r->x = v1->x - v2->x;
    r->y = v1->y - v2->y;
    r->z = v1->z - v2->z;
    r->w = v1->w - v2->w;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* vec4_muls(struct vec4f* r, const struct vec4f* v1, float k)
{
    r->x = v1->x * k;
    r->y = v1->y * k;
    r->z = v1->z * k;
    r->w = v1->w * k;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE int vec4_isequal(const struct vec4f* v1, const struct vec4f* v2)
{
    return (math_isequal(v1->x, v2->x) &&
            math_isequal(v1->y, v2->y) &&
            math_isequal(v1->z, v2->z) &&
            math_isequal(v1->w, v2->w));
}

/**
 * transform vector3(x,y,z,w) by 4x4 matrix, which is normally mat4f
 * @ingroup vmath
 */
CORE_API struct vec4f* vec4_transform(struct vec4f* r, const struct vec4f* v,
                                      const struct mat4f* m);

/**
 * @ingroup vmath
 */
INLINE struct quat4f* quat_setf(struct quat4f* r, float x, float y, float z, float w)
{
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct quat4f* quat_setq(struct quat4f* r, const struct quat4f* q)
{
    r->x = q->x;
    r->y = q->y;
    r->z = q->z;
    r->w = q->w;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct quat4f* quat_setidentity(struct quat4f* r)
{
    r->x = 0.0f;
    r->y = 0.0f;
    r->z = 0.0f;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE int quat_isqual(const struct quat4f* q1, const struct quat4f* q2)
{
    return (math_isequal(q1->x, q2->x) &&
            math_isequal(q1->y, q2->y) &&
            math_isequal(q1->z, q2->z) &&
            math_isequal(q1->w, q2->w));
}

/**
 * multiply qutaernions, which is basically combining two quaternion rotations together
 * @ingroup vmath
 */
INLINE struct quat4f* quat_mul(struct quat4f* r, const struct quat4f* q1, const struct quat4f* q2)
{
    return quat_setf(r,
        q1->w*q2->x + q1->x*q2->w + q1->z*q2->y - q1->y*q2->z,
        q1->w*q2->y + q1->y*q2->w + q1->x*q2->z - q1->z*q2->x,
        q1->w*q2->z + q1->z*q2->w + q1->y*q2->x - q1->x*q2->y,
        q1->w*q2->w - q1->x*q2->x - q1->y*q2->y - q1->z*q2->z);
}

/**
 * inverse quaternion (conjucate), which inverses the rotation effect of the quaternion
 * @ingroup vmath
 */
INLINE struct quat4f* quat_inv(struct quat4f* r, const struct quat4f* q)
{
    r->w = q->w;
    r->x = -r->x;
    r->y = -r->y;
    r->z = -r->z;
    return r;
}

/**
 * slerp (spherical-linear interpolation) quaternion
 * @param t: interpolator between 0~1
 * @ingroup vmath
 */
CORE_API struct quat4f* quat_slerp(struct quat4f* r, const struct quat4f* q1,
                                   const struct quat4f* q2, float t);
/**
 * get the angle of the quaternion rotation
 * @ingroup vmath
 */
CORE_API float quat_getangle(const struct quat4f* q);
/**
 * get rotation axis of quaternion
 * @ingroup vmath
 */
CORE_API struct vec4f* quat_getrotaxis(struct vec4f* axis, const struct quat4f* q);
/**
 * get quaternion representation in euler values form
 * @ingroup vmath
 */
CORE_API void quat_geteuler(float* pitch, float* yaw, float* roll, const struct quat4f* q);
/**
 * set rotations for quaternion, by axis/angle, euler or from the transform matrix
 * @ingroup vmath
 */
CORE_API struct quat4f* quat_fromaxis(struct quat4f* r, const struct vec4f* axis, float angle);
/**
 * @ingroup vmath
 */
CORE_API struct quat4f* quat_fromeuler(struct quat4f* r, float pitch, float yaw, float roll);
/**
 * @ingroup vmath
 */
CORE_API struct quat4f* quat_frommat3(struct quat4f* r, const struct mat3f* mat);

/* mat2f functions */
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_setf(struct mat2f *r,
                                 float m11, float m12,
                                 float m21, float m22,
                                 float m31, float m32);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_setm(struct mat2f *r, const struct mat2f *m);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_muls(struct mat2f *r, const struct mat2f *m, float k);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_add(struct mat2f *r, const struct mat2f *m1, const struct mat2f *m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_mul(struct mat2f *r, const struct mat2f *m1, const struct mat2f *m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_set_trans(struct mat2f *r, const struct vec2f *t);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_set_transf(struct mat2f *r, float x, float y);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_set_scale(struct mat2f *r, const struct vec2f *s);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_set_scalef(struct mat2f *r, float sx, float sy);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_set_rot(struct mat2f *r, float angle);
/**
 * @ingroup vmath
 */
CORE_API struct mat2f* mat2_set_ident(struct mat2f *r);
/**
 * @ingroup vmath
 */
INLINE struct mat2f* mat2_transpose(struct mat2f *r, const struct mat2f *m)
{
    return mat2_setf(r,
                     m->m11, m->m21,
                     m->m12, m->m22,
                     m->m31, m->m32);
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2_transform(struct vec2f *r, const struct vec2f *v, const struct mat2f *m)
{
    return vec2f_setf(r,
                      v->x*m->m11 + v->y*m->m21 + m->m31,
                      v->x*m->m12 + v->y*m->m22 + m->m32);
}

/**
 * @ingroup vmath
 */
INLINE struct vec2f* vec2_transformsr(struct vec2f *r, const struct vec2f *v, const struct mat2f *m)
{
    return vec2f_setf(r,
                      v->x*m->m11 + v->y*m->m21,
                      v->x*m->m12 + v->y*m->m22);
}

/* mat3f functions
 **
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_setf(struct mat3f* r,
                                 float m11, float m12, float m13,
                                 float m21, float m22, float m23,
                                 float m31, float m32, float m33,
                                 float m41, float m42, float m43);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_setm(struct mat3f* r, const struct mat3f* m);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_ident(struct mat3f* r);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_muls(struct mat3f* r, const struct mat3f* m, float k);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_add(struct mat3f* r, const struct mat3f* m1, const struct mat3f* m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_sub(struct mat3f* r, const struct mat3f* m1, const struct mat3f* m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_mul(struct mat3f* r, const struct mat3f* m1, const struct mat3f* m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_trans(struct mat3f* r, const struct vec4f* v);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_transf(struct mat3f* r, float x, float y, float z);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_scalef(struct mat3f* r, float x, float y, float z);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_scale(struct mat3f* r, const struct vec4f* s);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_rotaxis(struct mat3f* r, const struct vec4f* axis, float angle);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_roteuler(struct mat3f* r, float pitch, float yaw, float roll);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_rotquat(struct mat3f* r, const struct quat4f* q);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_trans_rot(struct mat3f* r, const struct vec3f* t,
                                          const struct quat4f* q);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_proj(struct mat3f* r, const struct vec4f* proj_plane_norm);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_set_refl(struct mat3f* r, const struct vec4f* refl_plane_norm);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat3_mul4(struct mat4f* r, const struct mat3f* m1, const struct mat4f* m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_inv(struct mat3f* r, const struct mat3f* m);
/**
 * @ingroup vmath
 */
CORE_API float mat3_det(const struct mat3f* m);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_transpose(struct mat3f* r, const struct mat3f* m);
/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_transpose_self(struct mat3f* r);
/**
 * @ingroup vmath
 */
CORE_API void mat3_get_roteuler(float* pitch, float* yaw, float* roll, const struct mat3f* m);
/**
 * @ingroup vmath
 */
CORE_API struct quat4f* mat3_get_rotquat(struct quat4f* q, const struct mat3f* m);

/**
 * @ingroup vmath
 */
INLINE struct vec4f* mat3_get_trans(struct vec4f* r, const struct mat3f* m)
{
    return vec3_setf(r, m->m41, m->m42, m->m43);
}

/**
 * @ingroup vmath
 */
INLINE void mat3_get_transf(float* x, float* y, float* z, const struct mat3f* m)
{
    *x = m->m41;
    *y = m->m42;
    *z = m->m43;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* mat3_get_transv(struct vec4f* r, const struct mat3f* m)
{
    r->x = m->m41;
    r->y = m->m42;
    r->z = m->m43;
    r->w = 1.0f;
    return r;
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* mat3_get_xaxis(struct vec4f* r, const struct mat3f* m)
{
    return vec3_setf(r, m->m11, m->m12, m->m13);
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* mat3_get_yaxis(struct vec4f* r, const struct mat3f* m)
{
   return vec3_setf(r, m->m21, m->m22, m->m23);
}

/**
 * @ingroup vmath
 */
INLINE struct vec4f* mat3_get_zaxis(struct vec4f* r, const struct mat3f* m)
{
    return vec3_setf(r, m->m31, m->m32, m->m33);
}

/**
 * @ingroup vmath
 */
CORE_API struct mat3f* mat3_invrt(struct mat3f* r, const struct mat3f* m);

/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_setf(struct mat4f* r,
                                 float m11, float m12, float m13, float m14,
                                 float m21, float m22, float m23, float m24,
                                 float m31, float m32, float m33, float m34,
                                 float m41, float m42, float m43, float m44);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_setm(struct mat4f* r, const struct mat4f* m);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_set_ident(struct mat4f* r);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_muls(struct mat4f* r, const struct mat4f* m, float k);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_add(struct mat4f* r, const struct mat4f* m1, const struct mat4f* m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_sub(struct mat4f* r, const struct mat4f* m1, const struct mat4f* m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_mul(struct mat4f* r, const struct mat4f* m1, const struct mat4f* m2);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_inv(struct mat4f* r, const struct mat4f* m);
/**
 * @ingroup vmath
 */
CORE_API float mat4_det(const struct mat4f* m);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_transpose(struct mat4f* r, const struct mat4f* m);
/**
 * @ingroup vmath
 */
CORE_API struct mat4f* mat4_transpose_self(struct mat4f* r);

/**
 * create simd vec4 by a given array count
 * @see vec4simd_destroy
 * @ingroup vmath
 */
CORE_API result_t vec4simd_create(struct vec4f_simd* v, struct allocator* alloc, uint cnt);

/**
 * destroy vec4-simd
 * @see vec4simd_create
 * @ingroup vmath
 */
CORE_API void vec4simd_destroy(struct vec4f_simd* v);


/**
 * @ingroup vmath
 */
CORE_API struct mat4f_simd* mat4simd_setm(struct mat4f_simd* r, const struct mat4f* m);

/**
 * @ingroup vmath
 */
INLINE struct xform3d* xform3d_setpq(struct xform3d* xform,
		const struct vec4f* p, const struct quat4f* q)
{
	vec3_setv(&xform->p, p);
	quat_setq(&xform->q, q);
	return xform;
}

/**
 * @ingroup vmath
 */
INLINE struct xform3d* xform3d_setf(struct xform3d* xform, float x, float y, float z,
		float pitch, float yaw, float roll)
{
	vec3_setf(&xform->p, x, y, z);
	quat_fromeuler(&xform->q, pitch, yaw, roll);
	return xform;
}

/**
 * @ingroup vmath
 */
INLINE struct xform3d* xform3d_setf_raw(struct xform3d* xform, float x, float y, float z,
    float rx, float ry, float rz, float rw)
{
    vec3_setf(&xform->p, x, y, z);
    quat_setf(&xform->q, rx, ry, rz, rw);
    return xform;
}

/**
 * @ingroup vmath
 */
INLINE struct xform3d* xform3d_frommat3(struct xform3d* xform, const struct mat3f* m)
{
	struct quat4f q;
	struct vec4f p;
	return xform3d_setpq(xform, mat3_get_trans(&p, m), quat_frommat3(&q, m));
}

/**
 * @ingroup vmath
 */
INLINE struct xform3d* xform3d_set(struct xform3d* r, const struct xform3d* xform)
{
	memcpy(r, xform, sizeof(struct xform3d));
	return r;
}

/**
 * @ingroup vmath
 */
INLINE struct mat3f* xform3d_getmat(struct mat3f* mat, const struct xform3d* xform)
{
    return mat3_set_trans_rot(mat, &xform->p, &xform->q);
}

/**
 * @ingroup vmath
 */
INLINE struct xform3d* xform3d_setidentity(struct xform3d* xform)
{
	vec3_setzero(&xform->p);
	quat_setidentity(&xform->q);
	return xform;
}

INLINE struct vec4i* vec4i_seti(struct vec4i* r, int x, int y, int z, int w)
{
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}

INLINE struct vec4i* vec4i_seta(struct vec4i* r, int a)
{
    r->x = a;
    r->y = a;
    r->z = a;
    r->w = a;
    return r;
}

INLINE struct vec4i* vec4i_add(struct vec4i* r, const struct vec4i* a, const struct vec4i* b)
{
    r->x = a->x + b->x;
    r->y = a->y + b->y;
    r->z = a->z + b->z;
    r->w = a->w + b->w;
    return r;
}

INLINE struct vec4i* vec4i_mul(struct vec4i* r, const struct vec4i* a, const struct vec4i* b)
{
    r->x = a->x * b->x;
    r->y = a->y * b->y;
    r->z = a->z * b->z;
    r->w = a->w * b->w;
    return r;
}

INLINE struct vec4i* vec4i_or(struct vec4i* r, const struct vec4i* a, const struct vec4i* b)
{
    r->x = a->x | b->x;
    r->y = a->y | b->y;
    r->z = a->z | b->z;
    r->w = a->w | b->w;
    return r;
}

#ifdef __cplusplus

namespace dh {

/* Vec3 */
class CORE_CPP_API ALIGN16 Vec3
{
protected:
    vec4f m_vec;

public:
    static const Vec3 UnitX;
    static const Vec3 UnitY;
    static const Vec3 UnitZ;
    static const Vec3 UnitX_Neg;
    static const Vec3 UnitY_Neg;
    static const Vec3 UnitZ_Neg;
    static const Vec3 Zero;

public:
    Vec3()
    {
    }

    Vec3(float x, float y, float z)
    {
        vec3_setf(&m_vec, x, y, z);
    }

    Vec3(const vec3f &v)
    {
        m_vec = v;
    }

    float x() const     {   return m_vec.x; }
    float y() const     {   return m_vec.y; }
    float z() const     {   return m_vec.z; }

    Vec3& set(const vec3f &v)
    {
        m_vec = v;
        return *this;
    }

    Vec3& set(float x, float y, float z)
    {
        vec3_setf(&m_vec, x, y, z);
        return *this;
    }

    bool operator==(const Vec3 &v) const
    {
        return vec3_isequal(&m_vec, &v.m_vec);
    }

    bool operator!=(const Vec3 &v) const
    {
        return !vec3_isequal(&m_vec, &v.m_vec);
    }

    Vec3 operator*(float k) const
    {
        Vec3 r;
        vec3_muls(&r.m_vec, &m_vec, k);
        return r;
    }

    Vec3& operator*=(float k)
    {
        vec3_muls(&m_vec, &m_vec, k);
        return *this;
    }

    Vec3 operator/(float k) const
    {
        Vec3 r;
        vec3_muls(&r.m_vec, &m_vec, 1.0f/k);
        return r;
    }

    Vec3& operator/=(float k)
    {
        vec3_muls(&m_vec, &m_vec, 1.0f/k);
        return *this;
    }

    Vec3 operator+(const Vec3& v) const
    {
        Vec3 r;
        vec3_add(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec3& operator+=(const Vec3& v)
    {
        vec3_add(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    Vec3 operator-(const Vec3& v) const
    {
        Vec3 r;
        vec3_sub(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec3& operator-=(const Vec3& v)
    {
        vec3_sub(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    static float dot(const Vec3& v0, const Vec3& v1)
    {
        return vec3_dot(&v0.m_vec, &v1.m_vec);
    }

    static Vec3 cross(const Vec3& v0, const Vec3& v1)
    {
        Vec3 r;
        vec3_cross(&r.m_vec, &v0.m_vec, &v1.m_vec);
        return r;
    }

    static float angle(const Vec3& v0, const Vec3& v1)
    {
        return vec3_angle(&v0.m_vec, &v1.m_vec);
    }

    static Vec3 lerp(const Vec3& v0, const Vec3& v1, float t)
    {
        Vec3 r;
        vec3_lerp(&r.m_vec, &v0.m_vec, &v1.m_vec, t);
        return r;
    }

    static Vec3 cubic(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3, float t)
    {
        Vec3 r;
        vec3_cubic(&r.m_vec, &v0.m_vec, &v1.m_vec, &v2.m_vec, &v3.m_vec, t);
        return r;
    }

    Vec3& set_normalize()
    {
        vec3_norm(&m_vec, &m_vec);
        return *this;
    }

    static Vec3 normalize(const Vec3& v)
    {
        Vec3 r;
        vec3_norm(&r.m_vec, &v.m_vec);
        return r;
    }

    float length() const
    {
        return vec3_len(&m_vec);
    }

    float& operator [](int idx)   {   return m_vec.f[idx];    }
    float operator [](int idx) const {   return m_vec.f[idx];    }

    operator vec4f*()   {   return &m_vec;  }
    operator const vec4f*() const   {   return &m_vec;  }
    operator float*()   {   return m_vec.f; }
    operator const float*() const {   return m_vec.f; }
};

/* Vec4 */
class ALIGN16 Vec4
{
private:
    vec4f m_vec;

public:
    Vec4()
    {
    }

    Vec4(float x, float y, float z, float w)
    {
        vec4_setf(&m_vec, x, y, z, w);
    }

    Vec4& set(float x, float y, float z, float w)
    {
        vec4_setf(&m_vec, x, y, z, w);
        return *this;
    }

    bool operator==(const Vec4 &v) const
    {
        return vec4_isequal(&m_vec, &v.m_vec);
    }

    bool operator!=(const Vec4 &v) const
    {
        return !vec4_isequal(&m_vec, &v.m_vec);
    }

    Vec4 operator*(float k) const
    {
        Vec4 r;
        vec4_muls(&r.m_vec, &m_vec, k);
        return r;
    }

    Vec4& operator*=(float k)
    {
        vec4_muls(&m_vec, &m_vec, k);
        return *this;
    }

    Vec4 operator/(float k) const
    {
        Vec4 r;
        vec4_muls(&r.m_vec, &m_vec, 1.0f/k);
        return r;
    }

    Vec4& operator/=(float k)
    {
        vec4_muls(&m_vec, &m_vec, 1.0f/k);
        return *this;
    }

    Vec4 operator+(const Vec4& v) const
    {
        Vec4 r;
        vec4_add(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec4& operator+=(const Vec4& v)
    {
        vec4_add(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    Vec4 operator-(const Vec4& v) const
    {
        Vec4 r;
        vec4_sub(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec4& operator-=(const Vec4& v)
    {
        vec4_sub(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    float& operator [](int idx)   {   return m_vec.f[idx];    }
    float operator [](int idx) const {   return m_vec.f[idx];    }

    float x() const { return m_vec.x;   }
    float y() const { return m_vec.y;   }
    float z() const { return m_vec.z;   }
    float w() const { return m_vec.w;   }

    operator vec4f*()   {   return &m_vec;  }
    operator const vec4f*() const   {   return &m_vec;  }
    operator float*()   {   return m_vec.f; }
    operator const float*() const {   return m_vec.f; }
};

/* Quat */
class CORE_CPP_API ALIGN16 Quat
{
private:
    quat4f m_quat;

public:
    static const Quat Ident;

public:
    Quat()
    {
    }

    Quat(float x, float y, float z, float w)
    {
        quat_setf(&m_quat, x, y, z, w);
    }

    Quat(const quat4f &q)
    {
        m_quat = q;
    }

    Quat& set(float x, float y, float z, float w)
    {
        quat_setf(&m_quat, x, y, z, w);
        return *this;
    }

    Quat& set(const quat4f &q)
    {
        m_quat = q;
        return *this;
    }

    Quat operator*(const Quat& q) const
    {
        Quat r;
        quat_mul(&r.m_quat, &m_quat, &q.m_quat);
        return r;
    }

    Quat& operator*=(const Quat& q)
    {
        quat_mul(&m_quat, &m_quat, &q.m_quat);
        return *this;
    }

    static Quat slerp(const Quat& q0, const Quat& q1, float t)
    {
        Quat r;
        quat_slerp(&r.m_quat, &q0.m_quat, &q1.m_quat, t);
        return r;
    }

    float rotation_angle() const
    {
        return quat_getangle(&m_quat);
    }

    Vec3 rotation_axis() const
    {
        Vec3 r;
        quat_getrotaxis(r, &m_quat);
        return r;
    }

    Vec3 rotation_euler() const
    {
        float pitch, yaw, roll;
        quat_geteuler(&pitch, &yaw, &roll, &m_quat);
        return Vec3(pitch, yaw, roll);
    }

    Quat& from_euler(float pitch, float yaw, float roll)
    {
        quat_fromeuler(&m_quat, pitch, yaw, roll);
        return *this;
    }

    Quat& from_axis(const Vec3& axis, float angle)
    {
        quat_fromaxis(&m_quat, axis, angle);
        return *this;
    }

    Quat& set_identity()
    {
        quat_setidentity(&m_quat);
        return *this;
    }

    float& operator [](int idx)   {   return m_quat.f[idx];    }
    float operator [](int idx) const {   return m_quat.f[idx];    }
    operator quat4f*()  {   return &m_quat; }
    operator const quat4f*() const  { return &m_quat; }
    operator float*() { return m_quat.f; }
    operator const float*() const { return m_quat.f;  }
};

class Mat4;

/* Mat3 */
class CORE_CPP_API ALIGN16 Mat3
{
private:
    mat3f m_mat;

public:
    static const Mat3 Ident;

public:
    Mat3()
    {
    }

    Mat3(float m11, float m12, float m13,
         float m21, float m22, float m23,
         float m31, float m32, float m33,
         float m41, float m42, float m43)
    {
        mat3_setf(&m_mat,
                  m11, m12, m13,
                  m21, m22, m23,
                  m31, m32, m33,
                  m41, m42, m43);
    }
    Mat3(const mat3f &m)
    {
        m_mat = m;
    }

    Mat3& set(float m11, float m12, float m13,
              float m21, float m22, float m23,
              float m31, float m32, float m33,
              float m41, float m42, float m43)
    {
        mat3_setf(&m_mat,
                  m11, m12, m13,
                  m21, m22, m23,
                  m31, m32, m33,
                  m41, m42, m43);
        return *this;
    }

    Mat3& set(const mat3f &m)
    {
        m_mat = m;
        return *this;
    }

    Mat3 operator+(const Mat3& m) const
    {
        Mat3 r;
        mat3_add(&r.m_mat, &m_mat, &m.m_mat);
        return r;
    }

    Mat3& operator+=(const Mat3& m)
    {
        mat3_add(&m_mat, &m_mat, &m.m_mat);
        return *this;
    }

    Mat3 operator*(float k) const
    {
        Mat3 r;
        mat3_muls(&r.m_mat, &m_mat, k);
        return r;
    }

    Mat3& operator*=(float k)
    {
        mat3_muls(&m_mat, &m_mat, k);
        return *this;
    }

    Mat3 operator*(const Mat3 &m) const
    {
        Mat3 r;
        mat3_mul(&r.m_mat, &m_mat, &m.m_mat);
        return r;
    }

    Mat3 operator*=(const Mat3 &m)
    {
        mat3_mul(&m_mat, &m_mat, &m.m_mat);
        return *this;
    }

    Mat4 operator*(const Mat4 &m) const;

    Mat3 rotation() const
    {
        return Mat3(m_mat.m11, m_mat.m12, m_mat.m13,
                    m_mat.m21, m_mat.m22, m_mat.m23,
                    m_mat.m31, m_mat.m32, m_mat.m33,
                    0.0f, 0.0f, 0.0f);
    }

    Quat rotation_quat() const
    {
        Quat r;
        mat3_get_rotquat(r, &m_mat);
        return r;
    }

    Vec3 rotation_euler() const
    {
        Vec3 r;
        mat3_get_roteuler(&r[0], &r[1], &r[2], &m_mat);
        return r;
    }

    Vec3 translation() const
    {
        Vec3 r;
        mat3_get_trans(r, &m_mat);
        return r;
    }

    Mat3& set_identity()
    {
        mat3_set_ident(&m_mat);
        return *this;
    }

    Mat3& set_rotation_euler(float pitch, float yaw, float roll)
    {
        mat3_set_roteuler(&m_mat, pitch, yaw, roll);
        return *this;
    }

    Mat3& set_rotation_quat(const Quat& quat)
    {
        mat3_set_rotquat(&m_mat, quat);
        return *this;
    }

    Mat3& set_rotation_axis(const Vec3& axis, float angle)
    {
        mat3_set_rotaxis(&m_mat, axis, angle);
        return *this;
    }

    Mat3& set_translation(const Vec3 &t)
    {
        mat3_set_trans(&m_mat, t);
        return *this;
    }

    Mat3& set_translation(float x, float y, float z)
    {
        mat3_set_transf(&m_mat, x, y, z);
        return *this;
    }

    Mat3& set_projection(const Vec3& norm)
    {
        mat3_set_proj(&m_mat, norm);
        return *this;
    }

    Mat3& set_reflection(const Vec3& refl)
    {
        mat3_set_refl(&m_mat, refl);
        return *this;
    }

    Mat3& set_scale(float sx, float sy, float sz)
    {
        mat3_set_scalef(&m_mat, sx, sy, sz);
        return *this;
    }

    Mat3& set_scale(const Vec3& s)
    {
        mat3_set_scale(&m_mat, s);
        return *this;
    }

    static Mat3 invert(const Mat3& m)
    {
        Mat3 r;
        mat3_inv(&r.m_mat, &m.m_mat);
        return r;
    }

    static Mat3 invert_transform(const Mat3& m)
    {
        Mat3 r;
        mat3_invrt(&r.m_mat, &m.m_mat);
        return r;
    }

    Mat3& set_invert()
    {
        mat3_inv(&m_mat, &m_mat);
        return *this;
    }

    Mat3& set_invert_transform()
    {
        mat3_invrt(&m_mat, &m_mat);
        return *this;
    }

    Mat3& set_transpose()
    {
        mat3_transpose_self(&m_mat);
        return *this;
    }

    static Mat3 transpose(const Mat3& m)
    {
        Mat3 r;
        mat3_transpose(&r.m_mat, &m.m_mat);
        return r;
    }

    Vec3 transform_SRT(const Vec3& v) const
    {
        Vec3 r;
        vec3_transformsrt(r, v, &m_mat);
        return r;
    }

    Vec3 transform_SR(const Vec3& v) const
    {
        Vec3 r;
        vec3_transformsr(r, v, &m_mat);
        return r;
    }

    Vec3 row(int idx) const
    {
        return Vec3(m_mat.f[idx*4], m_mat.f[idx*4+1], m_mat.f[idx*4+2]);
    }

    float& operator [](int idx)   {   return m_mat.f[idx];    }
    float operator [](int idx) const {   return m_mat.f[idx];    }
    operator mat3f*()   {   return &m_mat;  }
    operator const mat3f*() const   {   return &m_mat;  }
    operator float*()   {   return m_mat.f; }
    operator const float*() const {   return m_mat.f; }
};

/* Mat4 */
class CORE_CPP_API ALIGN16 Mat4
{
private:
    mat4f m_mat;

public:
    static const Mat4 Ident;

public:
    Mat4()
    {
    }

    Mat4(float m11, float m12, float m13, float m14,
         float m21, float m22, float m23, float m24,
         float m31, float m32, float m33, float m34,
         float m41, float m42, float m43, float m44)
    {
        mat4_setf(&m_mat,
                  m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);
    }
    Mat4(const mat4f &m)
    {
        m_mat = m;
    }

    Mat4& set(float m11, float m12, float m13, float m14,
             float m21, float m22, float m23, float m24,
             float m31, float m32, float m33, float m34,
             float m41, float m42, float m43, float m44)
    {
        mat4_setf(&m_mat,
                  m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);
        return *this;
    }

    Mat4& set(const mat4f &m)
    {
        m_mat = m;
        return *this;
    }

    Mat4 operator+(const Mat4 &m) const
    {
        Mat4 r;
        mat4_add(&r.m_mat, &m_mat, &m.m_mat);
        return r;
    }

    Mat4& operator+=(const Mat4 &m)
    {
        mat4_add(&m_mat, &m_mat, &m.m_mat);
        return *this;
    }

    Mat4 operator*(float k) const
    {
        Mat4 r;
        mat4_muls(&r.m_mat, &m_mat, k);
        return r;
    }

    Mat4& operator*=(float k)
    {
        mat4_muls(&m_mat, &m_mat, k);
        return *this;
    }

    Mat4 operator*(const Mat4 &m) const
    {
        Mat4 r;
        mat4_mul(&r.m_mat, &m_mat, &m.m_mat);
        return r;
    }

    Mat4 operator*=(const Mat4 &m)
    {
        mat4_mul(&m_mat, &m_mat, &m.m_mat);
        return *this;
    }

    Mat4& set_invert()
    {
        mat4_inv(&m_mat, &m_mat);
        return *this;
    }

    Mat4& set_transpose()
    {
        mat4_transpose_self(&m_mat);
        return *this;
    }

    static Mat4 transpose(const Mat4& m)
    {
        Mat4 r;
        mat4_transpose(&r.m_mat, &m.m_mat);
        return r;
    }

    Vec4 transform(const Vec4& v) const
    {
        Vec4 r;
        vec4_transform(r, v, &m_mat);
        return r;
    }

    Vec4 transform_SRT(const Vec4& v) const
    {
        Vec4 r;
        vec3_transformsrt_m4(r, v, &m_mat);
        return r;
    }

    Mat4& set_identity()
    {
        mat4_set_ident(&m_mat);
        return *this;
    }

    Vec4 row(int idx) const
    {
        return Vec4(m_mat.f[idx*4], m_mat.f[idx*4+1], m_mat.f[idx*4+2], m_mat.f[idx*4+3]);
    }

    float& operator [](int idx)   {   return m_mat.f[idx];    }
    float operator [](int idx) const {   return m_mat.f[idx];    }
    operator mat4f*()   {   return &m_mat;  }
    operator const mat4f*() const   {   return &m_mat;  }
    operator float*()   {   return m_mat.f; }
    operator const float*() const {   return m_mat.f; }
};

inline Mat4 Mat3::operator*(const Mat4 &m) const
{
    Mat4 r;
    mat3_mul4(r, &m_mat, m);
    return r;
}

/* Vec2 */
class CORE_CPP_API Vec2
{
private:
    vec2f m_vec;

public:
    static const Vec2 Zero;
    static const Vec2 UnitX;
    static const Vec2 UnitY;

public:
    Vec2()
    {
    }

    Vec2(float x, float y)
    {
        vec2f_setf(&m_vec, x, y);
    }
    Vec2(const vec2f v)
    {
        m_vec = v;
    }

    Vec2& set(float x, float y)
    {
        vec2f_setf(&m_vec, x, y);
        return *this;
    }

    Vec2& set(const vec2f &v)
    {
        m_vec = v;
        return *this;
    }

    bool operator==(const Vec2 &v) const
    {
        return vec2f_isequal(&m_vec, &v.m_vec);
    }

    Vec2 operator+(const Vec2 &v) const
    {
        Vec2 r;
        vec2f_add(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec2& operator+=(const Vec2 &v)
    {
        vec2f_add(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    Vec2 operator-(const Vec2 &v) const
    {
        Vec2 r;
        vec2f_sub(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec2& operator-=(const Vec2 &v)
    {
        vec2f_sub(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    Vec2 operator*(float k) const
    {
        Vec2 r;
        vec2f_muls(&r.m_vec, &m_vec, k);
        return r;
    }

    Vec2& operator*=(float k)
    {
        vec2f_muls(&m_vec, &m_vec, k);
        return *this;
    }

    float length() const
    {
        return vec2f_len(&m_vec);
    }

    static float dot(const Vec2& v0, const Vec2& v1)
    {
        return v0[0]*v1[0] + v0[1]*v1[1];
    }

    float x() const {   return m_vec.x; }
    float y() const {   return m_vec.y; }

    operator vec2f*() { return &m_vec;  }
    operator const vec2f*() const   {   return &m_vec;  }
    operator float*()   { return m_vec.f;   }
    operator const float*() const   {   return m_vec.f; }
    float& operator[](int idx)  {   return m_vec.f[idx];    }
    float operator[](int idx) const {   return m_vec.f[idx];    }
};

/* Vec2i */
class CORE_CPP_API Vec2i
{
private:
    vec2i m_vec;

public:
    static const Vec2i Zero;
    static const Vec2i UnitX;
    static const Vec2i UnitY;

public:
    Vec2i()
    {
    }

    Vec2i(int x, int y)
    {
        vec2i_seti(&m_vec, x, y);
    }
    Vec2i(const vec2i v)
    {
        m_vec = v;
    }

    Vec2i& set(const vec2i v)
    {
        m_vec = v;
        return *this;
    }

    Vec2i& set(int x, int y)
    {
        vec2i_seti(&m_vec, x, y);
        return *this;
    }

    bool operator==(const Vec2i& v) const
    {
        return m_vec.x == v.m_vec.x && m_vec.y == v.m_vec.y;
    }

    Vec2i operator+(const Vec2i& v) const
    {
        Vec2i r;
        vec2i_add(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec2i& operator+=(const Vec2i& v)
    {
        vec2i_add(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    Vec2i operator-(const Vec2i& v) const
    {
        Vec2i r;
        vec2i_sub(&r.m_vec, &m_vec, &v.m_vec);
        return r;
    }

    Vec2i& operator-=(const Vec2i& v)
    {
        vec2i_sub(&m_vec, &m_vec, &v.m_vec);
        return *this;
    }

    Vec2i operator*(int k) const
    {
        Vec2i r;
        vec2i_muls(&r.m_vec, &m_vec, k);
        return r;
    }

    Vec2i& operator*=(int k)
    {
        vec2i_muls(&m_vec, &m_vec, k);
        return *this;
    }

    int x() const {   return m_vec.x; }
    int y() const {   return m_vec.y; }

    operator vec2i*() { return &m_vec;  }
    operator const vec2i*() const   {   return &m_vec;  }
    operator int*()   { return m_vec.n;   }
    operator const int*() const   {   return m_vec.n; }
    int& operator[](int idx)  {   return m_vec.n[idx];    }
    int operator[](int idx) const {   return m_vec.n[idx];    }
};

class CORE_CPP_API ALIGN16 Mat2
{
private:
    mat2f m_mat;

public:
    static const Mat2 Ident;

public:
    Mat2()
    {
    }

    Mat2(float m11, float m12,
         float m21, float m22,
         float m31, float m32)
    {
        mat2_setf(&m_mat,
                  m11, m12,
                  m21, m22,
                  m31, m32);
    }

    Mat2(const mat2f &mat)
    {
        m_mat = mat;
    }

    Mat2& set(float m11, float m12,
              float m21, float m22,
              float m31, float m32)
    {
        mat2_setf(&m_mat,
                  m11, m12,
                  m21, m22,
                  m31, m32);
        return *this;
    }

    Mat2& set(const mat2f &mat)
    {
        m_mat = mat;
        return *this;
    }

    Mat2 operator*(float k) const
    {
        Mat2 r;
        mat2_muls(&r.m_mat, &m_mat, k);
        return r;
    }

    Mat2& operator*=(float k)
    {
        mat2_muls(&m_mat, &m_mat, k);
        return *this;
    }

    Mat2 operator+(const Mat2 &m) const
    {
        Mat2 r;
        mat2_add(&r.m_mat, &m_mat, &m.m_mat);
        return r;
    }

    Mat2& operator+=(const Mat2 &m)
    {
        mat2_add(&m_mat, &m_mat, &m.m_mat);
        return *this;
    }

    Vec2 translation() const
    {
        return Vec2(m_mat.m31, m_mat.m32);
    }

    Mat2& set_identity()
    {
        mat2_set_ident(&m_mat);
        return *this;
    }

    Mat2& set_rotation(float angle)
    {
        mat2_set_rot(&m_mat, angle);
        return *this;
    }

    Mat2& set_translation(float x, float y)
    {
        mat2_set_transf(&m_mat, x, y);
        return *this;
    }

    Mat2& set_translation(const Vec2 &t)
    {
        mat2_set_trans(&m_mat, t);
        return *this;
    }

    Mat2& set_scale(float sx, float sy)
    {
        mat2_set_scalef(&m_mat, sx, sy);
        return *this;
    }

    Mat2& set_scale(const Vec2 &s)
    {
        mat2_set_scale(&m_mat, s);
        return *this;
    }

    Vec2 transform(const Vec2 &v) const
    {
        Vec2 r;
        vec2_transform(r, v, &m_mat);
        return r;
    }

    Vec2 transform_SR(const Vec2 &v) const
    {
        Vec2 r;
        vec2_transformsr(r, v, &m_mat);
        return r;
    }

    Mat2& set_transpose()
    {
        mat2_transpose(&m_mat, &m_mat);
        return *this;
    }

    operator const mat2f*() const   {   return &m_mat;  }
    operator mat2f*() { return &m_mat;  }
    operator float*() { return m_mat.f; }
    operator const float*() const   {   return m_mat.f; }
};

} /* dh */

#endif

#endif /* __VECMATH_H__ */
