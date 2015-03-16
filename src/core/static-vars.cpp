/***********************************************************************************
 * Copyright (c) 2014, Sepehr Taghdisian (sep.tagh@gmail.com)
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

#include "dhcore/vec-math.h"
#include "dhcore/color.h"
#include "dhcore/prims.h"

using namespace dh;

// Vec3
const Vec3 Vec3::UnitX(1.0f, 0.0f, 0.0f);
const Vec3 Vec3::UnitY(0.0f, 1.0f, 0.0f);
const Vec3 Vec3::UnitZ(0.0f, 0.0f, 1.0f);
const Vec3 Vec3::UnitX_Neg(-1.0f, 0.0f, 0.0f);
const Vec3 Vec3::UnitY_Neg(0.0f, -1.0f, 0.0f);
const Vec3 Vec3::UnitZ_Neg(0.0f, 0.0f, -1.0f);
const Vec3 Vec3::Zero(0.0f, 0.0f, 0.0f);

// Quat
const Quat Quat::Ident(0.0f, 0.0f, 0.0f, 1.0f);

// Mat3
const Mat3 Mat3::Ident(1.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 1.0f,
                       0.0f, 0.0f, 0.0f);

// Mat4
const Mat4 Mat4::Ident(1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f);
const Mat4 Mat4::Zero(0.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 0.0f);

// Vec2
const Vec2 Vec2::Zero(0.0f, 0.0f);
const Vec2 Vec2::UnitX(1.0f, 0.0f);
const Vec2 Vec2::UnitY(0.0f, 1.0f);

// Vec2i
const Vec2i Vec2i::Zero(0, 0);
const Vec2i Vec2i::UnitX(1, 0);
const Vec2i Vec2i::UnitY(0, 1);

// Mat2
const Mat2 Mat2::Ident(1.0f, 0.0f,
                       0.0f, 1.0f,
                       0.0f, 0.0f);

// Color
const Color Color::Black(0.0f, 0.0f, 0.0f);
const Color Color::White(1.0f, 1.0f, 1.0f);
const Color Color::Red(1.0f, 0.0f, 0.0f);
const Color Color::Green(0.0f, 1.0f, 0.0f);
const Color Color::Blue(0.0f, 0.0f, 1.0f);
const Color Color::Yellow(1.0f, 1.0f, 0.0f);
const Color Color::Purple(0.0f, 1.0f, 1.0f);
const Color Color::Grey(0.3f, 0.3f, 0.3f);

// Rect
const Rect Rect::Zero(0, 0, 0, 0);

// Rectf
const Rectf Rectf::Zero(0, 0, 0, 0);
