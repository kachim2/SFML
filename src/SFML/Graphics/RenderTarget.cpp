////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTargetImplVBO.hpp>
#include <SFML/Graphics/RenderTargetImplDefault.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/System/Err.hpp>
#include <iostream>


namespace sf
{
////////////////////////////////////////////////////////////
RenderTarget::RenderTarget(ImplementationHint hint) :
m_impl       (NULL),
m_defaultView(),
m_view       ()
{
#if !defined(SFML_SYSTEM_EMSCRIPTEN)

    // Create the implementation
    if ((hint != Default) && priv::RenderTargetImplVBO::isAvailable())
    {
        // Use vertex-buffer object (VBO)
        m_impl = new priv::RenderTargetImplVBO(*this);
    }
    else
    {
        if (hint == Vbo)
            err() << "VBO RenderTarget implementation unavailable" << std::endl;

        // Use default implementation
        m_impl = new priv::RenderTargetImplDefault(*this);
    }

#else

    m_impl = new priv::RenderTargetImplVBO(*this);

#endif
}


////////////////////////////////////////////////////////////
RenderTarget::~RenderTarget()
{
    delete m_impl;
}


////////////////////////////////////////////////////////////
void RenderTarget::clear(const Color& color)
{
    m_impl->clear(color);
}


////////////////////////////////////////////////////////////
void RenderTarget::setView(const View& view)
{
    m_impl->setView(view);
}


////////////////////////////////////////////////////////////
const View& RenderTarget::getView() const
{
    return m_impl->getView();
}


////////////////////////////////////////////////////////////
const View& RenderTarget::getDefaultView() const
{
    return m_impl->getDefaultView();
}


////////////////////////////////////////////////////////////
IntRect RenderTarget::getViewport(const View& view) const
{
    float width  = static_cast<float>(getSize().x);
    float height = static_cast<float>(getSize().y);
    const FloatRect& viewport = view.getViewport();

    return IntRect(static_cast<int>(0.5f + width  * viewport.left),
                   static_cast<int>(0.5f + height * viewport.top),
                   static_cast<int>(0.5f + width  * viewport.width),
                   static_cast<int>(0.5f + height * viewport.height));
}


////////////////////////////////////////////////////////////
Vector2f RenderTarget::mapPixelToCoords(const Vector2i& point) const
{
    return mapPixelToCoords(point, getView());
}


////////////////////////////////////////////////////////////
Vector2f RenderTarget::mapPixelToCoords(const Vector2i& point, const View& view) const
{
    // First, convert from viewport coordinates to homogeneous coordinates
    Vector2f normalized;
    IntRect viewport = getViewport(view);
    normalized.x = -1.f + 2.f * (point.x - viewport.left) / viewport.width;
    normalized.y =  1.f - 2.f * (point.y - viewport.top)  / viewport.height;

    // Then transform by the inverse of the view matrix
    return view.getInverseTransform().transformPoint(normalized);
}


////////////////////////////////////////////////////////////
Vector2i RenderTarget::mapCoordsToPixel(const Vector2f& point) const
{
    return mapCoordsToPixel(point, getView());
}


////////////////////////////////////////////////////////////
Vector2i RenderTarget::mapCoordsToPixel(const Vector2f& point, const View& view) const
{
    // First, transform the point by the view matrix
    Vector2f normalized = view.getTransform().transformPoint(point);

    // Then convert to viewport coordinates
    Vector2i pixel;
    IntRect viewport = getViewport(view);
    pixel.x = static_cast<int>(( normalized.x + 1.f) / 2.f * viewport.width  + viewport.left);
    pixel.y = static_cast<int>((-normalized.y + 1.f) / 2.f * viewport.height + viewport.top);

    return pixel;
}


////////////////////////////////////////////////////////////
void RenderTarget::draw(const Drawable& drawable, const RenderStates& states)
{
    drawable.draw(*this, states);
}


////////////////////////////////////////////////////////////
void RenderTarget::draw(const Vertex* vertices, std::size_t vertexCount,
                        PrimitiveType type, const RenderStates& states)
{
    m_impl->draw(vertices, vertexCount, type, states);
}


////////////////////////////////////////////////////////////
void RenderTarget::pushGLStates()
{
    m_impl->pushGLStates();
}


////////////////////////////////////////////////////////////
void RenderTarget::popGLStates()
{
    m_impl->popGLStates();
}


////////////////////////////////////////////////////////////
void RenderTarget::resetGLStates()
{
    m_impl->resetGLStates();
}


////////////////////////////////////////////////////////////
void RenderTarget::initialize()
{
    m_impl->initialize();
}


////////////////////////////////////////////////////////////
void RenderTarget::deinitialize()
{
    m_impl->deinitialize();
}


} // namespace sf
