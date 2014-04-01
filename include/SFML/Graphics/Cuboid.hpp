
#ifndef SFML_CUBOID_HPP
#define SFML_CUBOID_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Polyhedron.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
/// \brief Specialized polyhedron representing a cuboid
///
////////////////////////////////////////////////////////////
class SFML_GRAPHICS_API Cuboid : public Polyhedron
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// \param size Size of the cuboid
    ///
    ////////////////////////////////////////////////////////////
    explicit Cuboid(const Vector3f& size = Vector3f(0, 0, 0));

    ////////////////////////////////////////////////////////////
    /// \brief Set the size of the cuboid
    ///
    /// \param size New size of the cuboid
    ///
    /// \see getSize
    ///
    ////////////////////////////////////////////////////////////
    void setSize(const Vector3f& size);

    ////////////////////////////////////////////////////////////
    /// \brief Get the size of the cuboid
    ///
    /// \return Size of the cuboid
    ///
    /// \see setSize
    ///
    ////////////////////////////////////////////////////////////
    const Vector3f& getSize() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the number of faces defining the polyhedron
    ///
    /// \return Number of faces defining the polyhedron
    ///
    ////////////////////////////////////////////////////////////
    virtual unsigned int getFaceCount() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get a face of the polyhedron
    ///
    /// The result is undefined if \a index is out of the valid range.
    ///
    /// \param index Index of the face to get, in range [0 .. getFaceCount() - 1]
    ///
    /// \return Index-th face of the polyhedron
    ///
    ////////////////////////////////////////////////////////////
    virtual Face getFace(unsigned int index) const;

private :

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    Vector3f m_size; ///< Size of the cuboid
};

} // namespace sf


#endif // SFML_CUBOID_HPP


////////////////////////////////////////////////////////////
/// \class sf::Cuboid
/// \ingroup graphics
///
/// This class inherits all the functions of sf::Transformable
/// (position, rotation, scale, bounds, ...) as well as the
/// functions of sf::Polyhedron (color, texture, ...).
///
/// Usage example:
/// \code
/// sf::Cuboid cuboid;
/// cuboid.setSize(sf::Vector3f(100, 50, 70));
/// cuboid.setColor(sf::Color::Red);
/// cuboid.setPosition(10, 20, 30);
/// ...
/// window.draw(cuboid);
/// \endcode
///
/// \see sf::Polyhedron, sf::SphericalPolyhedron, sf::ConvexPolyhedron
///
////////////////////////////////////////////////////////////
