/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Optional.h>
#include <LibGfx/AffineTransform.h>
#include <LibGfx/Rect.h>

namespace Gfx {

bool AffineTransform::is_identity() const
{
    return m_values[0] == 1 && m_values[1] == 0 && m_values[2] == 0 && m_values[3] == 1 && m_values[4] == 0 && m_values[5] == 0;
}

static float hypotenuse(float x, float y)
{
    // FIXME: This won't handle overflow :(
    return sqrt(x * x + y * y);
}

float AffineTransform::x_scale() const
{
    return hypotenuse(m_values[0], m_values[1]);
}

float AffineTransform::y_scale() const
{
    return hypotenuse(m_values[2], m_values[3]);
}

AffineTransform& AffineTransform::scale(float sx, float sy)
{
    m_values[0] *= sx;
    m_values[1] *= sx;
    m_values[2] *= sy;
    m_values[3] *= sy;
    return *this;
}

AffineTransform& AffineTransform::translate(float tx, float ty)
{
    m_values[4] += tx * m_values[0] + ty * m_values[2];
    m_values[5] += tx * m_values[1] + ty * m_values[3];
    return *this;
}

AffineTransform& AffineTransform::multiply(const AffineTransform& other)
{
    AffineTransform result;
    result.m_values[0] = other.a() * a() + other.b() * c();
    result.m_values[1] = other.a() * b() + other.b() * d();
    result.m_values[2] = other.c() * a() + other.d() * c();
    result.m_values[3] = other.c() * b() + other.d() * d();
    result.m_values[4] = other.e() * a() + other.f() * c() + e();
    result.m_values[5] = other.e() * b() + other.f() * d() + f();
    *this = result;
    return *this;
}

AffineTransform& AffineTransform::rotate_radians(float radians)
{
    float sin_angle = sinf(radians);
    float cos_angle = cosf(radians);
    AffineTransform rotation(cos_angle, sin_angle, -sin_angle, cos_angle, 0, 0);
    multiply(rotation);
    return *this;
}

void AffineTransform::map(float unmapped_x, float unmapped_y, float& mapped_x, float& mapped_y) const
{
    mapped_x = (m_values[0] * unmapped_x + m_values[2] * unmapped_y + m_values[4]);
    mapped_y = (m_values[1] * unmapped_x + m_values[3] * unmapped_y + m_values[5]);
}

template<>
IntPoint AffineTransform::map(const IntPoint& point) const
{
    float mapped_x;
    float mapped_y;
    map(point.x(), point.y(), mapped_x, mapped_y);
    return { roundf(mapped_x), roundf(mapped_y) };
}

template<>
FloatPoint AffineTransform::map(const FloatPoint& point) const
{
    float mapped_x;
    float mapped_y;
    map(point.x(), point.y(), mapped_x, mapped_y);
    return { mapped_x, mapped_y };
}

template<>
IntSize AffineTransform::map(const IntSize& size) const
{
    return { roundf(size.width() * x_scale()), roundf(size.height() * y_scale()) };
}

template<>
FloatSize AffineTransform::map(const FloatSize& size) const
{
    return { size.width() * x_scale(), size.height() * y_scale() };
}

template<typename T>
static T smallest_of(T p1, T p2, T p3, T p4)
{
    return min(min(p1, p2), min(p3, p4));
}

template<typename T>
static T largest_of(T p1, T p2, T p3, T p4)
{
    return max(max(p1, p2), max(p3, p4));
}

template<>
FloatRect AffineTransform::map(const FloatRect& rect) const
{
    FloatPoint p1 = map(rect.top_left());
    FloatPoint p2 = map(rect.top_right().translated(1, 0));
    FloatPoint p3 = map(rect.bottom_right().translated(1, 1));
    FloatPoint p4 = map(rect.bottom_left().translated(0, 1));
    float left = smallest_of(p1.x(), p2.x(), p3.x(), p4.x());
    float top = smallest_of(p1.y(), p2.y(), p3.y(), p4.y());
    float right = largest_of(p1.x(), p2.x(), p3.x(), p4.x());
    float bottom = largest_of(p1.y(), p2.y(), p3.y(), p4.y());
    return { left, top, right - left, bottom - top };
}

template<>
IntRect AffineTransform::map(const IntRect& rect) const
{
    return enclosing_int_rect(map(FloatRect(rect)));
}

}
