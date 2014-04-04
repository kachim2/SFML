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
#include <SFML/Graphics/TextureSaver.hpp>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
TextureSaver::TextureSaver() :
m_restore1D(false),
m_restore2D(true)
{
    glCheck(glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_textureBinding));
}


////////////////////////////////////////////////////////////
TextureSaver::TextureSaver(int) :
m_restore1D(true),
m_restore2D(false)
{
    glCheck(glGetIntegerv(GL_TEXTURE_BINDING_1D, &m_textureBinding));
}


////////////////////////////////////////////////////////////
TextureSaver::TextureSaver(int, int) :
m_restore1D(false),
m_restore2D(false)
{
    glCheck(glGetIntegerv(GL_TEXTURE_BINDING_3D, &m_textureBinding));
}


////////////////////////////////////////////////////////////
TextureSaver::~TextureSaver()
{
    if (m_restore2D)
        glCheck(glBindTexture(GL_TEXTURE_2D, m_textureBinding));
    else if (m_restore1D)
        glCheck(glBindTexture(GL_TEXTURE_1D, m_textureBinding));
    else
        glCheck(glBindTexture(GL_TEXTURE_3D, m_textureBinding));
}

} // namespace priv

} // namespace sf
