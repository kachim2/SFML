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

#ifndef SFML_TEXTURESAVER_HPP
#define SFML_TEXTURESAVER_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/GLCheck.hpp>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Automatic wrapper for saving and restoring the current texture binding
///
////////////////////////////////////////////////////////////
class TextureSaver
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor (2D texture binding)
    ///
    /// The current 2D texture binding is saved.
    ///
    ////////////////////////////////////////////////////////////
    TextureSaver();

    ////////////////////////////////////////////////////////////
    /// \brief Constructor (1D texture binding)
    ///
    /// The current 1D texture binding is saved.
    ///
    ////////////////////////////////////////////////////////////
    TextureSaver(int);

    ////////////////////////////////////////////////////////////
    /// \brief Constructor (3D texture binding)
    ///
    /// The current 3D texture binding is saved.
    ///
    ////////////////////////////////////////////////////////////
    TextureSaver(int, int);

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    /// The previous texture binding is restored.
    ///
    ////////////////////////////////////////////////////////////
    ~TextureSaver();

private :

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    GLint m_textureBinding; ///< Texture binding to restore
    bool  m_restore1D;      ///< Whether to restore the 1D texture binding
    bool  m_restore2D;      ///< Whether to restore the 2D texture binding
};

} // namespace priv

} // namespace sf


#endif // SFML_TEXTURESAVER_HPP
