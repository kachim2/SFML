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
#include <SFML/Graphics/RenderTargetImplVBO.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/System/Err.hpp>
#include <iostream>


namespace
{
    // Convert an sf::BlendMode::Factor constant to the corresponding OpenGL constant.
    sf::Uint32 factorToGlConstant(sf::BlendMode::Factor blendFactor)
    {
        switch (blendFactor)
        {
            case sf::BlendMode::Zero:             return GL_ZERO;
            case sf::BlendMode::One:              return GL_ONE;
            case sf::BlendMode::SrcColor:         return GL_SRC_COLOR;
            case sf::BlendMode::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
            case sf::BlendMode::DstColor:         return GL_DST_COLOR;
            case sf::BlendMode::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
            case sf::BlendMode::SrcAlpha:         return GL_SRC_ALPHA;
            case sf::BlendMode::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case sf::BlendMode::DstAlpha:         return GL_DST_ALPHA;
            case sf::BlendMode::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
        }
    }


    // Convert an sf::BlendMode::BlendEquation constant to the corresponding OpenGL constant.
    sf::Uint32 equationToGlConstant(sf::BlendMode::Equation blendEquation)
    {
        switch (blendEquation)
        {
            case sf::BlendMode::Add:             return GLEXT_GL_FUNC_ADD;
            case sf::BlendMode::Subtract:        return GLEXT_GL_FUNC_SUBTRACT;
        }
    }
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
RenderTargetImplVBO::RenderTargetImplVBO(RenderTarget& target) :
RenderTargetImpl(target),
m_cache         (),
m_buffer        (0),
m_bufferSize    (0)
{
    m_cache.glStatesSet = false;
    m_savedStates.statesSet = false;

    m_cache.clearColor = Color(0, 0, 0, 0);

    static const std::string vertexShaderSource =
#if !defined(SFML_OPENGL_ES)
        "#version 130\n"
#endif
        "uniform mat4 sf_ModelViewMatrix;\n"
        "uniform mat4 sf_ProjectionMatrix;\n"
        "uniform mat4 sf_TextureMatrix;\n"
#if !defined(SFML_OPENGL_ES)
        "in vec4 sf_Vertex;\n"
        "in vec4 sf_Color;\n"
        "in vec4 sf_MultiTexCoord;\n"
        "out vec4 sf_FrontColor;\n"
        "out vec2 sf_TexCoord;\n"
#else
        "attribute vec4 sf_Vertex;\n"
        "attribute vec4 sf_Color;\n"
        "attribute vec4 sf_MultiTexCoord;\n"
        "varying vec4 sf_FrontColor;\n"
        "varying vec2 sf_TexCoord;\n"
#endif
        "void main() {\n"
        "    gl_Position = sf_ProjectionMatrix * sf_ModelViewMatrix * sf_Vertex;\n"
        "    sf_FrontColor = sf_Color;\n"
        "    sf_TexCoord = (sf_TextureMatrix * sf_MultiTexCoord).st;\n"
        "}";

    static const std::string fragmentShaderSource =
#if !defined(SFML_OPENGL_ES)
        "#version 130\n"
#endif
        "uniform sampler2D textureSampler;\n"
#if !defined(SFML_OPENGL_ES)
        "in vec4 sf_FrontColor;\n"
        "in vec2 sf_TexCoord;\n"
        "out vec4 sf_FragColor;\n"
#else
        "precision mediump float;\n"
        "varying vec4 sf_FrontColor;\n"
        "varying vec2 sf_TexCoord;\n"
#endif
        "void main() {\n"
#if !defined(SFML_OPENGL_ES)
        "    sf_FragColor = sf_FrontColor * texture2D(textureSampler, sf_TexCoord);\n"
#else
        "    gl_FragColor = sf_FrontColor * texture2D(textureSampler, sf_TexCoord);\n"
#endif
        "}";

    m_defaultShader.loadFromMemory(vertexShaderSource, fragmentShaderSource);

    Image image;
    image.create(1, 1, Color::White);
    m_defaultTexture.loadFromImage(image);
}


////////////////////////////////////////////////////////////
RenderTargetImplVBO::~RenderTargetImplVBO()
{
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::clear(const Color& color)
{
    if (activate(true))
    {
        if (color != m_cache.clearColor)
        {
            glCheck(glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f));
            m_cache.clearColor = color;
        }

        glCheck(glClear(GL_COLOR_BUFFER_BIT));
    }
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::setView(const View& view)
{
    RenderTargetImpl::setView(view);
    m_cache.viewChanged = true;
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::draw(const Vertex* vertices, std::size_t vertexCount,
                        PrimitiveType type, const RenderStates& states)
{
    // Nothing to draw?
    if (!vertices || (vertexCount == 0))
        return;

    // GL_QUADS is unavailable on OpenGL ES
    #ifdef SFML_OPENGL_ES
        if (type == Quads)
        {
            err() << "sf::Quads primitive type is not supported on OpenGL ES platforms, drawing skipped" << std::endl;
            return;
        }
        #define GL_QUADS 0
    #endif

    if (activate(true))
    {
        // First set the persistent OpenGL states if it's the very first call
        if (!m_cache.glStatesSet)
            resetGLStates();

        // Apply the blend mode
        if (states.blendMode != m_cache.lastBlendMode)
            applyBlendMode(states.blendMode);

        // Check if a user-supplied shader is compatible with the RenderTarget
        if (states.shader && !states.shader->m_compatible)
        {
            err() << "Shader incompatible with RenderTarget, check the guidelines for more information" << std::endl;
            return;
        }

        if (states.shader)
        {
            Shader& shader = *const_cast<Shader*>(states.shader);

            // Since the user might change uniform values between
            // subsequent draw calls, we just have to set them every time

            // Apply the transform (model view matrix uniform)
            applyTransform(shader, states.transform);

            // Apply the texture (sampler uniform)
            if (states.texture)
                applyTexture(shader, *states.texture);
            else
                applyTexture(shader, m_defaultTexture);

            // Apply the view (projection matrix uniform)
            applyCurrentView(shader);

            // Apply the shader
            Uint64 shaderId = shader.m_cacheId;
            if (shaderId != m_cache.lastShaderId)
                applyShader(shader);
        }
        else
        {
            Shader& shader = m_defaultShader;

            // Since the user can't change uniform values of the default shader
            // between subsequent draw calls, we can check if we have to
            // reset the uniforms using the cached values

            Uint64 shaderId = shader.m_cacheId;
            if (shaderId != m_cache.lastShaderId)
            {
                // Apply the transform (model view matrix uniform)
                applyTransform(shader, states.transform);

                // Apply the texture (sampler uniform)
                if (states.texture)
                    applyTexture(shader, *states.texture);
                else
                    applyTexture(shader, m_defaultTexture);

                // Apply the view (projection matrix uniform)
                applyCurrentView(shader);

                // Apply the shader
                applyShader(shader);
            }
            else
            {
                // Apply the transform (model view matrix uniform)
                applyTransform(shader, states.transform);

                // Apply the texture (sampler uniform)
                if (states.texture)
                {
                    if (states.texture->m_cacheId != m_cache.lastTextureId)
                        applyTexture(shader, *states.texture);
                }
                else
                {
                    if (m_defaultTexture.m_cacheId != m_cache.lastTextureId)
                        applyTexture(shader, m_defaultTexture);
                }

                // Apply the view (projection matrix uniform)
                if (m_cache.viewChanged)
                    applyCurrentView(shader);
            }
        }

        // Setup the pointers to the vertices' components
        const char* data = reinterpret_cast<const char*>(vertices);

        std::size_t size = sizeof(Vertex) * vertexCount;

        // We just make sure that the buffer is big enough for anything we draw
        std::size_t newSize = std::max<std::size_t>(m_bufferSize, size);

        // Orphan the buffer storage to maximize streaming performance
        glCheck(GLEXT_glBufferData(GLEXT_GL_ARRAY_BUFFER, newSize, NULL, GLEXT_GL_STREAM_DRAW));
        glCheck(GLEXT_glBufferData(GLEXT_GL_ARRAY_BUFFER, size, data, GLEXT_GL_STREAM_DRAW));

        // Only update attribute pointers if the buffer storage changes
        if (m_bufferSize != newSize)
        {
            glCheck(GLEXT_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0)));
            glCheck(GLEXT_glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(8)));
            glCheck(GLEXT_glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(12)));

            m_bufferSize = newSize;
        }

        // Find the OpenGL primitive type
        static const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
                                       GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
        GLenum mode = modes[type];

        // Draw the primitives
        glCheck(glDrawArrays(mode, 0, vertexCount));

        // Unbind the shader, if any
        if (states.shader)
            applyShader(m_defaultShader);
    }
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::pushGLStates()
{
    if (activate(true))
    {
        #ifdef SFML_DEBUG
            // make sure that the user didn't leave an unchecked OpenGL error
            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
            {
                err() << "OpenGL error (" << error << ") detected in user code, "
                      << "you should check for errors with glGetError()"
                      << std::endl;
            }
        #endif

        if (m_savedStates.statesSet)
        {
            err() << "States already pushed, maximum stack depth is 1" << std::endl;
            return;
        }

#if !defined(SFML_SYSTEM_EMSCRIPTEN)

        m_savedStates.currentProgram = glCheck(GLEXT_glGetHandle(GLEXT_GL_PROGRAM_OBJECT));

#else

        glCheck(glGetIntegerv(GLEXT_GL_PROGRAM_OBJECT, reinterpret_cast<int*>(&m_savedStates.currentProgram)));

#endif

        glCheck(glGetIntegerv(GLEXT_GL_ARRAY_BUFFER_BINDING, &m_savedStates.boundArrayBuffer));

        glCheck(glGetIntegerv(GLEXT_GL_BLEND_SRC_RGB, &m_savedStates.blendSourceRGB));
        glCheck(glGetIntegerv(GLEXT_GL_BLEND_DST_RGB, &m_savedStates.blendDestinationRGB));

        glCheck(glGetIntegerv(GLEXT_GL_BLEND_SRC_ALPHA, &m_savedStates.blendSourceAlpha));
        glCheck(glGetIntegerv(GLEXT_GL_BLEND_DST_ALPHA, &m_savedStates.blendDestinationAlpha));

        glCheck(glGetIntegerv(GLEXT_GL_BLEND_EQUATION_RGB, &m_savedStates.blendEquationRGB));
        glCheck(glGetIntegerv(GLEXT_GL_BLEND_EQUATION_ALPHA, &m_savedStates.blendEquationAlpha));

        m_savedStates.cullFaceEnabled = glCheck(glIsEnabled(GL_CULL_FACE));
        m_savedStates.depthTestEnabled = glCheck(glIsEnabled(GL_DEPTH_TEST));
        m_savedStates.blendEnabled = glCheck(glIsEnabled(GL_BLEND));

        int array0Enabled = 0;
        int array1Enabled = 0;
        int array2Enabled = 0;

        glCheck(GLEXT_glGetVertexAttribiv(0, GLEXT_GL_VERTEX_ATTRIB_ARRAY_ENABLED, &array0Enabled));
        glCheck(GLEXT_glGetVertexAttribiv(1, GLEXT_GL_VERTEX_ATTRIB_ARRAY_ENABLED, &array1Enabled));
        glCheck(GLEXT_glGetVertexAttribiv(2, GLEXT_GL_VERTEX_ATTRIB_ARRAY_ENABLED, &array2Enabled));

        m_savedStates.attributeArray0Enabled = array0Enabled;
        m_savedStates.attributeArray1Enabled = array1Enabled;
        m_savedStates.attributeArray2Enabled = array2Enabled;

        m_savedStates.statesSet = true;
    }

    resetGLStates();
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::popGLStates()
{
    if (activate(true))
    {
        if (!m_savedStates.statesSet)
        {
            err() << "States not yet pushed, minimum stack depth is 0" << std::endl;
            return;
        }

        glCheck(GLEXT_glUseProgramObject(m_savedStates.currentProgram));

        glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, m_savedStates.boundArrayBuffer));

        glCheck(GLEXT_glBlendFuncSeparate(
            m_savedStates.blendSourceRGB, m_savedStates.blendDestinationRGB,
            m_savedStates.blendSourceAlpha, m_savedStates.blendDestinationAlpha));

        glCheck(GLEXT_glBlendEquationSeparate(m_savedStates.blendEquationRGB, m_savedStates.blendEquationAlpha));

        if (m_savedStates.cullFaceEnabled)
        {
            glCheck(glEnable(GL_CULL_FACE));
        }
        else
        {
            glCheck(glDisable(GL_CULL_FACE));
        }

        if (m_savedStates.depthTestEnabled)
        {
            glCheck(glEnable(GL_DEPTH_TEST));
        }
        else
        {
            glCheck(glDisable(GL_DEPTH_TEST));
        }

        if (m_savedStates.blendEnabled)
        {
            glCheck(glEnable(GL_BLEND));
        }
        else
        {
            glCheck(glDisable(GL_BLEND));
        }

        if (m_savedStates.attributeArray0Enabled)
        {
            glCheck(GLEXT_glEnableVertexAttribArray(0));
        }
        else
        {
            glCheck(GLEXT_glDisableVertexAttribArray(0));
        }

        if (m_savedStates.attributeArray1Enabled)
        {
            glCheck(GLEXT_glEnableVertexAttribArray(1));
        }
        else
        {
            glCheck(GLEXT_glDisableVertexAttribArray(1));
        }

        if (m_savedStates.attributeArray2Enabled)
        {
            glCheck(GLEXT_glEnableVertexAttribArray(2));
        }
        else
        {
            glCheck(GLEXT_glDisableVertexAttribArray(2));
        }

        m_savedStates.statesSet = false;
    }
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::resetGLStates()
{
    if (activate(true))
    {
        // Make sure that extensions are initialized
        priv::ensureExtensionsInit();

        // Define the default OpenGL states
        glCheck(glDisable(GL_CULL_FACE));
        glCheck(glDisable(GL_DEPTH_TEST));
        glCheck(glEnable(GL_BLEND));
        glCheck(GLEXT_glEnableVertexAttribArray(0)); // Position
        glCheck(GLEXT_glEnableVertexAttribArray(1)); // Color
        glCheck(GLEXT_glEnableVertexAttribArray(2)); // Texture coordinate
        glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, m_buffer));
        m_cache.glStatesSet = true;

        // Apply the default SFML states
        applyBlendMode(BlendAlpha);
        applyTransform(m_defaultShader, Transform::Identity);
        applyTexture(m_defaultShader, m_defaultTexture);
        applyShader(m_defaultShader);

        // Set the default view
        setView(getView());
    }
}


////////////////////////////////////////////////////////////
bool RenderTargetImplVBO::isAvailable()
{
    bool available = Shader::isAvailable() && GLEXT_vertex_buffer_object && GLEXT_vertex_program;

    return available;
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::initialize()
{
    // Setup the default and current views
    RenderTargetImpl::initialize();

    // Set GL states only on first draw, so that we don't pollute user's states
    m_cache.glStatesSet = false;

    if (activate(true))
    {
        // Destroy any previous buffer object
        // A 0 ID is silently ignored by GL
        glCheck(GLEXT_glDeleteBuffers(1, &m_buffer));

        // Create the buffer object
        glCheck(GLEXT_glGenBuffers(1, &m_buffer));
    }
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::deinitialize()
{
    // Destroy the buffer object
    if (activate(true))
        glCheck(GLEXT_glDeleteBuffers(1, &m_buffer));
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::applyCurrentView(Shader& shader)
{
    // Set the viewport
    IntRect viewport = getViewport(getView());
    int top = getSize().y - (viewport.top + viewport.height);
    glCheck(glViewport(viewport.left, top, viewport.width, viewport.height));

    // Set the projection matrix
    shader.setParameter("sf_ProjectionMatrix", getView().getTransform());

    m_cache.viewChanged = false;
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::applyBlendMode(const BlendMode& mode)
{
    // Apply the blend mode, falling back to the non-separate versions if necessary
    glCheck(GLEXT_glBlendFuncSeparate(
        factorToGlConstant(mode.colorSrcFactor), factorToGlConstant(mode.colorDstFactor),
        factorToGlConstant(mode.alphaSrcFactor), factorToGlConstant(mode.alphaDstFactor)));

    glCheck(GLEXT_glBlendEquationSeparate(
        equationToGlConstant(mode.colorEquation),
        equationToGlConstant(mode.alphaEquation)));

    m_cache.lastBlendMode = mode;
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::applyTransform(Shader& shader, const Transform& transform)
{
    // Set the model view matrix
    shader.setParameter("sf_ModelViewMatrix", transform);
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::applyTexture(Shader& shader, const Texture& texture)
{
    shader.setParameter("textureSampler", texture);

    // Make sure the sampler is actually bound to the program
    Shader::bind(&shader);

    if (texture.m_texture)
    {
        // If non-normalized coordinates (= pixels) are requested, we need to
        // setup scale factors that convert the range [0 .. size] to [0 .. 1]
        float scaleX = 1.f / texture.m_actualSize.x;
        float scaleY = 1.f / texture.m_actualSize.y;

        float flipFactor = 0.f;

        // If pixels are flipped we must invert the Y axis
        if (texture.m_pixelsFlipped)
        {
            scaleY = -scaleY;
            flipFactor = static_cast<float>(texture.m_size.y) / texture.m_actualSize.y;
        }

        // Load the texture matrix matrix
        Transform transform(scaleX, 0.f,    0.f,
                            0.f,    scaleY, flipFactor,
                            0.f,    0.f,    1.f);
        shader.setParameter("sf_TextureMatrix", transform);
    }
    else
    {
        // Reset the texture matrix
        shader.setParameter("sf_TextureMatrix", Transform::Identity);
    }

    m_cache.lastTextureId = texture.m_cacheId;
}


////////////////////////////////////////////////////////////
void RenderTargetImplVBO::applyShader(const Shader& shader)
{
    Shader::bind(&shader);

    m_cache.lastShaderId = shader.m_cacheId;
}

} // namespace priv

} // namespace sf


////////////////////////////////////////////////////////////
// Render states caching strategies
//
// * View
//   If SetView was called since last draw, the projection
//   matrix is updated. We don't need more, the view doesn't
//   change frequently.
//
// * Blending mode
//   Since it overloads the == operator, we can easily check
//   whether any of the 6 blending components changed and,
//   thus, whether we need to update the blend mode.
//
// * Texture
//   Storing the pointer or OpenGL ID of the last used texture
//   is not enough; if the sf::Texture instance is destroyed,
//   both the pointer and the OpenGL ID might be recycled in
//   a new texture instance. We need to use our own unique
//   identifier system to ensure consistent caching.
//
// * Shader
//   Shaders are very hard to optimize, because they have
//   parameters that can be hard (if not impossible) to track,
//   like matrices or textures. The only optimization that we
//   do is that we avoid setting the default shader if it was
//   already set for the previous draw.
//
////////////////////////////////////////////////////////////
