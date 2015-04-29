////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2014 Laurent Gomila (laurent.gom@gmail.com)
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
#include <SFML/Graphics/RenderTargetImpl.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
RenderTargetImpl::RenderTargetImpl(RenderTarget& target) :
m_target     (target),
m_defaultView(),
m_view       ()
{
}


////////////////////////////////////////////////////////////
RenderTargetImpl::~RenderTargetImpl()
{
}


////////////////////////////////////////////////////////////
void RenderTargetImpl::initialize()
{
    // Setup the default and current views
    m_defaultView.reset(FloatRect(0, 0, static_cast<float>(m_target.getSize().x), static_cast<float>(m_target.getSize().y)));
    m_view = m_defaultView;
}


////////////////////////////////////////////////////////////
void RenderTargetImpl::deinitialize()
{
}


////////////////////////////////////////////////////////////
void RenderTargetImpl::setView(const View& view)
{
    m_view = view;
}


////////////////////////////////////////////////////////////
const View& RenderTargetImpl::getView() const
{
    return m_view;
}


////////////////////////////////////////////////////////////
const View& RenderTargetImpl::getDefaultView() const
{
    return m_defaultView;
}


////////////////////////////////////////////////////////////
IntRect RenderTargetImpl::getViewport(const View& view) const
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
Vector2u RenderTargetImpl::getSize() const
{
    return m_target.getSize();
}


////////////////////////////////////////////////////////////
bool RenderTargetImpl::activate(bool active)
{
    return m_target.activate(active);
}

} // namespace priv

} // namespace sf
