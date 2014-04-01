////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/ConvexPolyhedron.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
ConvexPolyhedron::ConvexPolyhedron(unsigned int faceCount)
{
    setFaceCount(faceCount);
}


////////////////////////////////////////////////////////////
void ConvexPolyhedron::setFaceCount(unsigned int count)
{
    m_vertices.resize(count * 3);
    update();
}


////////////////////////////////////////////////////////////
unsigned int ConvexPolyhedron::getFaceCount() const
{
    return static_cast<unsigned int>(m_vertices.size() / 3);
}


////////////////////////////////////////////////////////////
void ConvexPolyhedron::setFace(unsigned int index, const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
    m_vertices[index * 3 + 0] = v0;
    m_vertices[index * 3 + 1] = v1;
    m_vertices[index * 3 + 2] = v2;
    update();
}


////////////////////////////////////////////////////////////
ConvexPolyhedron::Face ConvexPolyhedron::getFace(unsigned int index) const
{
    ConvexPolyhedron::Face face = { m_vertices[index * 3 + 0], m_vertices[index * 3 + 1], m_vertices[index * 3 + 2] };
    return face;
}

} // namespace sf
