/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Format.h>
#include <LibGfx/Orientation.h>
#include <LibIPC/Forward.h>

namespace Gfx {

template<typename T>
class Size {
public:
    Size() { }

    Size(T w, T h)
        : m_width(w)
        , m_height(h)
    {
    }

    template<typename U>
    Size(U width, U height)
        : m_width(width)
        , m_height(height)
    {
    }

    template<typename U>
    explicit Size(const Size<U>& other)
        : m_width(other.width())
        , m_height(other.height())
    {
    }

    bool is_null() const { return !m_width && !m_height; }
    bool is_empty() const { return m_width <= 0 || m_height <= 0; }

    T width() const { return m_width; }
    T height() const { return m_height; }

    T area() const { return width() * height(); }

    void set_width(T w) { m_width = w; }
    void set_height(T h) { m_height = h; }

    template<typename U>
    bool contains(const Size<U>& other) const
    {
        return other.m_width <= m_width && other.m_height <= m_height;
    }

    template<class U>
    bool operator==(const Size<U>& other) const
    {
        return width() == other.width() && height() == other.height();
    }

    template<class U>
    bool operator!=(const Size<U>& other) const
    {
        return !(*this == other);
    }

    Size<T>& operator-=(const Size<T>& other)
    {
        m_width -= other.m_width;
        m_height -= other.m_height;
        return *this;
    }

    Size<T>& operator+=(const Size<T>& other)
    {
        m_width += other.m_width;
        m_height += other.m_height;
        return *this;
    }

    Size<T> operator*(T factor) const { return { m_width * factor, m_height * factor }; }

    Size<T>& operator*=(T factor)
    {
        m_width *= factor;
        m_height *= factor;
        return *this;
    }

    T primary_size_for_orientation(Orientation orientation) const
    {
        return orientation == Orientation::Vertical ? height() : width();
    }

    void set_primary_size_for_orientation(Orientation orientation, T value)
    {
        if (orientation == Orientation::Vertical) {
            set_height(value);
        } else {
            set_width(value);
        }
    }

    T secondary_size_for_orientation(Orientation orientation) const
    {
        return orientation == Orientation::Vertical ? width() : height();
    }

    void set_secondary_size_for_orientation(Orientation orientation, T value)
    {
        if (orientation == Orientation::Vertical) {
            set_width(value);
        } else {
            set_height(value);
        }
    }

    template<typename U>
    Size<U> to_type() const
    {
        return Size<U>(*this);
    }

    String to_string() const;

private:
    T m_width { 0 };
    T m_height { 0 };
};

using IntSize = Size<int>;
using FloatSize = Size<float>;

}

namespace AK {

template<typename T>
struct Formatter<Gfx::Size<T>> : Formatter<StringView> {
    void format(FormatBuilder& builder, const Gfx::Size<T>& value)
    {
        Formatter<StringView>::format(builder, value.to_string());
    }
};

}

namespace IPC {

bool encode(Encoder&, const Gfx::IntSize&);
bool decode(Decoder&, Gfx::IntSize&);

}
