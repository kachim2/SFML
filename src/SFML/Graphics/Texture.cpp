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
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/Graphics/TextureSaver.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <cassert>
#include <cstring>
#include <cmath>


namespace
{
    // Thread-safe unique identifier generator,
    // is used for states cache (see RenderTarget)
    sf::Uint64 getUniqueId()
    {
        static sf::Uint64 id = 1; // start at 1, zero is "no texture"
        static sf::Mutex mutex;

        sf::Lock lock(mutex);
        return id++;
    }
}


namespace sf
{
////////////////////////////////////////////////////////////
Texture::Texture() :
m_size         (0, 0, 0),
m_actualSize   (0, 0, 0),
m_texture      (0),
m_isSmooth     (false),
m_isRepeated   (false),
m_pixelsFlipped(false),
m_cacheId      (getUniqueId())
{

}


////////////////////////////////////////////////////////////
Texture::Texture(const Texture& copy) :
m_size         (0, 0, 0),
m_actualSize   (0, 0, 0),
m_texture      (0),
m_isSmooth     (copy.m_isSmooth),
m_isRepeated   (copy.m_isRepeated),
m_pixelsFlipped(false),
m_cacheId      (getUniqueId())
{
    if (copy.m_texture)
        loadFromImage(copy.copyToImage());
}


////////////////////////////////////////////////////////////
Texture::~Texture()
{
    // Destroy the OpenGL texture
    if (m_texture)
    {
        ensureGlContext();

        GLuint texture = static_cast<GLuint>(m_texture);
        glCheck(glDeleteTextures(1, &texture));
    }
}


////////////////////////////////////////////////////////////
bool Texture::create(unsigned int width, unsigned int height, unsigned int depth)
{
    // Check if texture parameters are valid before creating it
    if (!width)
    {
        err() << "Failed to create texture, invalid size (" << width << ")" << std::endl;
        return false;
    }

    if (!height && depth)
    {
        err() << "Failed to create texture, invalid size (" << width << "x" << height << "x" << depth << ")" << std::endl;
        return false;
    }

    // Compute the internal texture dimensions depending on NPOT textures support
    Vector3u actualSize(getValidSize(width), height ? getValidSize(height) : 0, depth ? getValidSize(depth) : 0);

    // Check the maximum texture size
    unsigned int maxSize = getMaximumSize();
    if ((actualSize.x > maxSize) || (actualSize.y > maxSize) || (actualSize.z > maxSize))
    {
        err() << "Failed to create texture, its internal size is too high ";
        err() << "(" << actualSize.x;
        if (actualSize.y)
            err() << "x" << actualSize.y;
        if (actualSize.z)
            err() << "x" << actualSize.z;
        err() << ", maximum is " << maxSize;
        if (actualSize.y)
            err() << "x" << maxSize;
        if (actualSize.z)
            err() << "x" << maxSize;
        err() << ")" << std::endl;
        return false;
    }

    // All the validity checks passed, we can store the new texture settings
    m_size.x        = width;
    m_size.y        = height;
    m_size.z        = depth;
    m_actualSize    = actualSize;
    m_pixelsFlipped = false;

    ensureGlContext();

    // Create the OpenGL texture if it doesn't exist yet
    if (!m_texture)
    {
        GLuint texture;
        glCheck(glGenTextures(1, &texture));
        m_texture = static_cast<unsigned int>(texture);
    }

    // Make sure that all the current texture bindings will be preserved
    priv::TextureSaver save2D;
    priv::TextureSaver save1D(0);
    priv::TextureSaver save3D(0, 0);

    // Initialize the texture
    GLenum target = GL_TEXTURE_3D;

    if (!height)
        target = GL_TEXTURE_1D;
    else if (!depth)
        target = GL_TEXTURE_2D;

    glCheck(glBindTexture(target, m_texture));
    glCheck(glTexParameteri(target, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
    glCheck(glTexParameteri(target, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
    glCheck(glTexParameteri(target, GL_TEXTURE_WRAP_R, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
    glCheck(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
    glCheck(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));

    if (!height)
        glCheck(glTexImage1D(target, 0, GL_RGBA8, m_actualSize.x, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    else if (!depth)
        glCheck(glTexImage2D(target, 0, GL_RGBA8, m_actualSize.x, m_actualSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    else
        glCheck(glTexImage3D(target, 0, GL_RGBA8, m_actualSize.x, m_actualSize.y, m_actualSize.z, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

    m_cacheId = getUniqueId();

    return true;
}


////////////////////////////////////////////////////////////
bool Texture::loadFromFile(const std::string& filename, const IntRect& area)
{
    Image image;
    return image.loadFromFile(filename) && loadFromImage(image, area);
}


////////////////////////////////////////////////////////////
bool Texture::loadFromMemory(const void* data, std::size_t size, const IntRect& area)
{
    Image image;
    return image.loadFromMemory(data, size) && loadFromImage(image, area);
}


////////////////////////////////////////////////////////////
bool Texture::loadFromStream(InputStream& stream, const IntRect& area)
{
    Image image;
    return image.loadFromStream(stream) && loadFromImage(image, area);
}


////////////////////////////////////////////////////////////
bool Texture::loadFromImage(const Image& image, const IntRect& area)
{
    // Retrieve the image size
    int width = static_cast<int>(image.getSize().x);
    int height = static_cast<int>(image.getSize().y);

    // Load the entire image if the source area is either empty or contains the whole image
    if (area.width == 0 || (area.height == 0) ||
       ((area.left <= 0) && (area.top <= 0) && (area.width >= width) && (area.height >= height)))
    {
        // Load the entire image
        if (create(image.getSize().x, image.getSize().y))
        {
            update(image);

            // Force an OpenGL flush, so that the texture will appear updated
            // in all contexts immediately (solves problems in multi-threaded apps)
            glCheck(glFlush());

            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // Load a sub-area of the image

        // Adjust the rectangle to the size of the image
        IntRect rectangle = area;
        if (rectangle.left   < 0) rectangle.left = 0;
        if (rectangle.top    < 0) rectangle.top  = 0;
        if (rectangle.left + rectangle.width > width)  rectangle.width  = width - rectangle.left;
        if (rectangle.top + rectangle.height > height) rectangle.height = height - rectangle.top;

        // Create the texture and upload the pixels
        if (create(rectangle.width, rectangle.height))
        {
            // Make sure that the current texture binding will be preserved
            priv::TextureSaver save;

            // Copy the pixels to the texture, row by row
            const Uint8* pixels = image.getPixelsPtr() + 4 * (rectangle.left + (width * rectangle.top));
            glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
            for (int i = 0; i < rectangle.height; ++i)
            {
                glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, rectangle.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
                pixels += 4 * width;
            }

            // Force an OpenGL flush, so that the texture will appear updated
            // in all contexts immediately (solves problems in multi-threaded apps)
            glCheck(glFlush());

            return true;
        }
        else
        {
            return false;
        }
    }
}


////////////////////////////////////////////////////////////
Vector2u Texture::getSize() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
Image Texture::copyToImage() const
{
    // Easy case: empty texture
    if (!m_texture)
        return Image();

    // Another easy case: non-2D texture
    if (!m_size.y || m_size.z)
        return Image();

    ensureGlContext();

    // Make sure that the current texture binding will be preserved
    priv::TextureSaver save;

    // Create an array of pixels
    std::vector<Uint8> pixels(m_size.x * m_size.y * 4);

    if ((m_size == m_actualSize) && !m_pixelsFlipped)
    {
        // Texture is not padded nor flipped, we can use a direct copy
        glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
        glCheck(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]));
    }
    else
    {
        // Texture is either padded or flipped, we have to use a slower algorithm

        // All the pixels will first be copied to a temporary array
        std::vector<Uint8> allPixels(m_actualSize.x * m_actualSize.y * 4);
        glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
        glCheck(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &allPixels[0]));

        // Then we copy the useful pixels from the temporary array to the final one
        const Uint8* src = &allPixels[0];
        Uint8* dst = &pixels[0];
        int srcPitch = m_actualSize.x * 4;
        int dstPitch = m_size.x * 4;

        // Handle the case where source pixels are flipped vertically
        if (m_pixelsFlipped)
        {
            src += srcPitch * (m_size.y - 1);
            srcPitch = -srcPitch;
        }

        for (unsigned int i = 0; i < m_size.y; ++i)
        {
            std::memcpy(dst, src, dstPitch);
            src += srcPitch;
            dst += dstPitch;
        }
    }

    // Create the image
    Image image;
    image.create(m_size.x, m_size.y, &pixels[0]);

    return image;
}


////////////////////////////////////////////////////////////
void Texture::update(const Uint8* texels)
{
    // Update the whole texture
    if (m_size.z)
        update(texels, m_size.x, m_size.y, m_size.z, 0, 0, 0);
    else if (m_size.y)
        update(texels, m_size.x, m_size.y, 0, 0);
    else if (m_size.x)
        update(texels, m_size.x, 0);
}


////////////////////////////////////////////////////////////
void Texture::update(const Uint8* texels, unsigned int width, unsigned int x)
{
    assert(!m_size.y);
    assert(x + width <= m_size.x);

    if (texels && m_texture)
    {
        ensureGlContext();

        // Make sure that the current 1D texture binding will be preserved
        priv::TextureSaver save(0);

        // Copy texels from the given array to the texture
        glCheck(glBindTexture(GL_TEXTURE_1D, m_texture));
        glCheck(glTexSubImage1D(GL_TEXTURE_1D, 0, x, width, GL_RGBA, GL_UNSIGNED_BYTE, texels));
        m_pixelsFlipped = false;
        m_cacheId = getUniqueId();
    }
}


////////////////////////////////////////////////////////////
void Texture::update(const Uint8* texels, unsigned int width, unsigned int height, unsigned int x, unsigned int y)
{
    assert(m_size.y);
    assert(!m_size.z);
    assert(x + width <= m_size.x);
    assert(y + height <= m_size.y);

    if (texels && m_texture)
    {
        ensureGlContext();

        // Make sure that the current 2D texture binding will be preserved
        priv::TextureSaver save;

        // Copy texels from the given array to the texture
        glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
        glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texels));
        m_pixelsFlipped = false;
        m_cacheId = getUniqueId();
    }
}


////////////////////////////////////////////////////////////
void Texture::update(const Uint8* texels, unsigned int width, unsigned int height, unsigned int depth, unsigned int x, unsigned int y, unsigned int z)
{
    assert(m_size.y);
    assert(m_size.z);
    assert(x + width <= m_size.x);
    assert(y + height <= m_size.y);
    assert(z + depth <= m_size.z);

    if (texels && m_texture)
    {
        ensureGlContext();

        // Make sure that the current 3D texture binding will be preserved
        priv::TextureSaver save(0, 0);

        // Copy texels from the given array to the texture
        glCheck(glBindTexture(GL_TEXTURE_3D, m_texture));
        glCheck(glTexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, texels));
        m_pixelsFlipped = false;
        m_cacheId = getUniqueId();
    }
}


////////////////////////////////////////////////////////////
void Texture::update(const Image& image)
{
    // Update the whole texture
    update(image.getPixelsPtr(), image.getSize().x, image.getSize().y, 0, 0);
}


////////////////////////////////////////////////////////////
void Texture::update(const Image& image, unsigned int x, unsigned int y)
{
    update(image.getPixelsPtr(), image.getSize().x, image.getSize().y, x, y);
}


////////////////////////////////////////////////////////////
void Texture::update(const Window& window)
{
    update(window, 0, 0);
}


////////////////////////////////////////////////////////////
void Texture::update(const Window& window, unsigned int x, unsigned int y)
{
    assert(m_size.y);
    assert(!m_size.z);
    assert(x + window.getSize().x <= m_size.x);
    assert(y + window.getSize().y <= m_size.y);

    if (m_texture && window.setActive(true))
    {
        // Make sure that the current texture binding will be preserved
        priv::TextureSaver save;

        // Copy pixels from the back-buffer to the texture
        glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
        glCheck(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 0, 0, window.getSize().x, window.getSize().y));
        m_pixelsFlipped = true;
        m_cacheId = getUniqueId();
    }
}


////////////////////////////////////////////////////////////
void Texture::setSmooth(bool smooth)
{
    if (smooth != m_isSmooth)
    {
        m_isSmooth = smooth;

        if (m_texture)
        {
            ensureGlContext();

            if (m_size.z)
            {
                // Make sure that the current 3D texture binding will be preserved
                priv::TextureSaver save(0, 0);

                glCheck(glBindTexture(GL_TEXTURE_3D, m_texture));
                glCheck(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
                glCheck(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
            }
            else if (m_size.y)
            {
                // Make sure that the current 2D texture binding will be preserved
                priv::TextureSaver save;

                glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
                glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
                glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
            }
            else
            {
                // Make sure that the current 1D texture binding will be preserved
                priv::TextureSaver save(0);

                glCheck(glBindTexture(GL_TEXTURE_1D, m_texture));
                glCheck(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
                glCheck(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
            }
        }
    }
}


////////////////////////////////////////////////////////////
bool Texture::isSmooth() const
{
    return m_isSmooth;
}


////////////////////////////////////////////////////////////
void Texture::setRepeated(bool repeated)
{
    if (repeated != m_isRepeated)
    {
        m_isRepeated = repeated;

        if (m_texture)
        {
            ensureGlContext();

            if (m_size.z)
            {
                // Make sure that the current 3D texture binding will be preserved
                priv::TextureSaver save(0, 0);

                glCheck(glBindTexture(GL_TEXTURE_3D, m_texture));
                glCheck(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
                glCheck(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
            }
            else if (m_size.y)
            {
                // Make sure that the current 2D texture binding will be preserved
                priv::TextureSaver save;

                glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
                glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
                glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
            }
            else
            {
                // Make sure that the current 1D texture binding will be preserved
                priv::TextureSaver save(0);

                glCheck(glBindTexture(GL_TEXTURE_1D, m_texture));
                glCheck(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
                glCheck(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
            }
        }
    }
}


////////////////////////////////////////////////////////////
bool Texture::isRepeated() const
{
    return m_isRepeated;
}


////////////////////////////////////////////////////////////
void Texture::bind(const Texture* texture, CoordinateType coordinateType)
{
    ensureGlContext();

    if (texture && texture->m_texture)
    {
        // Bind the texture
        if (texture->m_size.z)
            glCheck(glBindTexture(GL_TEXTURE_3D, texture->m_texture));
        else if (texture->m_size.y)
            glCheck(glBindTexture(GL_TEXTURE_2D, texture->m_texture));
        else
            glCheck(glBindTexture(GL_TEXTURE_1D, texture->m_texture));

        // Check if we need to define a special texture matrix
        if ((coordinateType == Pixels) || texture->m_pixelsFlipped)
        {
            GLfloat matrix[16] = {1.f, 0.f, 0.f, 0.f,
                                  0.f, 1.f, 0.f, 0.f,
                                  0.f, 0.f, 1.f, 0.f,
                                  0.f, 0.f, 0.f, 1.f};

            // If non-normalized coordinates (= pixels) are requested, we need to
            // setup scale factors that convert the range [0 .. size] to [0 .. 1]
            if (coordinateType == Pixels)
            {
                matrix[0] = 1.f / texture->m_actualSize.x;
                matrix[5] = 1.f / texture->m_actualSize.y;
            }

            // If pixels are flipped we must invert the Y axis
            if (texture->m_pixelsFlipped)
            {
                matrix[5] = -matrix[5];
                matrix[13] = static_cast<float>(texture->m_size.y) / texture->m_actualSize.y;
            }

            // Load the matrix
            glCheck(glMatrixMode(GL_TEXTURE));
            glCheck(glLoadMatrixf(matrix));

            // Go back to model-view mode (sf::RenderTarget relies on it)
            glCheck(glMatrixMode(GL_MODELVIEW));
        }
    }
    else
    {
        // Bind no texture
        glCheck(glBindTexture(GL_TEXTURE_3D, 0));
        glCheck(glBindTexture(GL_TEXTURE_2D, 0));
        glCheck(glBindTexture(GL_TEXTURE_1D, 0));

        // Reset the texture matrix
        glCheck(glMatrixMode(GL_TEXTURE));
        glCheck(glLoadIdentity());

        // Go back to model-view mode (sf::RenderTarget relies on it)
        glCheck(glMatrixMode(GL_MODELVIEW));
    }
}


////////////////////////////////////////////////////////////
void Texture::unbind(BindingType bindingType)
{
    ensureGlContext();

    // Bind no texture
    if (bindingType == Texture2D)
        glCheck(glBindTexture(GL_TEXTURE_2D, 0));
    else if (bindingType == Texture3D)
        glCheck(glBindTexture(GL_TEXTURE_3D, 0));
    else if (bindingType == Texture1D)
        glCheck(glBindTexture(GL_TEXTURE_1D, 0));
}


////////////////////////////////////////////////////////////
unsigned int Texture::getMaximumSize()
{
    ensureGlContext();

    GLint size;
    glCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size));

    return static_cast<unsigned int>(size);
}


////////////////////////////////////////////////////////////
Texture& Texture::operator =(const Texture& right)
{
    Texture temp(right);

    std::swap(m_size,          temp.m_size);
    std::swap(m_actualSize,    temp.m_actualSize);
    std::swap(m_texture,       temp.m_texture);
    std::swap(m_isSmooth,      temp.m_isSmooth);
    std::swap(m_isRepeated,    temp.m_isRepeated);
    std::swap(m_pixelsFlipped, temp.m_pixelsFlipped);
    m_cacheId = getUniqueId();

    return *this;
}


////////////////////////////////////////////////////////////
unsigned int Texture::getValidSize(unsigned int size)
{
    ensureGlContext();

    // Make sure that GLEW is initialized
    priv::ensureGlewInit();

    if (GLEW_ARB_texture_non_power_of_two)
    {
        // If hardware supports NPOT textures, then just return the unmodified size
        return size;
    }
    else
    {
        // If hardware doesn't support NPOT textures, we calculate the nearest power of two
        float exponent = std::ceil(std::log(static_cast<float>(size)) / std::log(2.f));
        unsigned int powerOfTwo = 1 << static_cast<unsigned int>(exponent);

        return powerOfTwo;
    }
}

} // namespace sf
