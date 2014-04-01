
#ifndef SFML_BOX_HPP
#define SFML_BOX_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/System/Vector2.hpp>
#include <algorithm>


namespace sf
{
////////////////////////////////////////////////////////////
/// \brief Utility class for manipulating 3D axis aligned boxes
///
////////////////////////////////////////////////////////////
template <typename T>
class Box
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// Creates an empty box (it is equivalent to calling
    /// Box(0, 0, 0, 0, 0, 0)).
    ///
    ////////////////////////////////////////////////////////////
    Box();

    ////////////////////////////////////////////////////////////
    /// \brief Construct the box from its coordinates
    ///
    /// Be careful, the last three parameters are the width,
    /// height and depth, not the right, bottom and back coordinates!
    ///
    /// \param boxLeft   Left coordinate of the box
    /// \param boxTop    Top coordinate of the box
    /// \param boxFront  Front coordinate of the box
    /// \param boxWidth  Width of the box
    /// \param boxHeight Height of the box
    /// \param boxDepth  Depth of the box
    ///
    ////////////////////////////////////////////////////////////
    Box(T boxLeft, T boxTop, T boxFront, T boxWidth, T boxHeight, T boxDepth);

    ////////////////////////////////////////////////////////////
    /// \brief Construct the box from position and size
    ///
    /// Be careful, the last parameter is the size,
    /// not the bottom-right-back corner!
    ///
    /// \param position Position of the top-left-front corner of the box
    /// \param size     Size of the box
    ///
    ////////////////////////////////////////////////////////////
    Box(const Vector3<T>& position, const Vector3<T>& size);

    ////////////////////////////////////////////////////////////
    /// \brief Construct the box from another type of box
    ///
    /// This constructor doesn't replace the copy constructor,
    /// it's called only when U != T.
    /// A call to this constructor will fail to compile if U
    /// is not convertible to T.
    ///
    /// \param box Box to convert
    ///
    ////////////////////////////////////////////////////////////
    template <typename U>
    explicit Box(const Box<U>& box);

    ////////////////////////////////////////////////////////////
    /// \brief Check if a point is inside the box's area
    ///
    /// \param x X coordinate of the point to test
    /// \param y Y coordinate of the point to test
    /// \param z Z coordinate of the point to test
    ///
    /// \return True if the point is inside, false otherwise
    ///
    /// \see intersects
    ///
    ////////////////////////////////////////////////////////////
    bool contains(T x, T y, T z) const;

    ////////////////////////////////////////////////////////////
    /// \brief Check if a point is inside the box's area
    ///
    /// \param point Point to test
    ///
    /// \return True if the point is inside, false otherwise
    ///
    /// \see intersects
    ///
    ////////////////////////////////////////////////////////////
    bool contains(const Vector3<T>& point) const;

    ////////////////////////////////////////////////////////////
    /// \brief Check the intersection between two boxes
    ///
    /// \param box Box to test
    ///
    /// \return True if boxes overlap, false otherwise
    ///
    /// \see contains
    ///
    ////////////////////////////////////////////////////////////
    bool intersects(const Box<T>& box) const;

    ////////////////////////////////////////////////////////////
    /// \brief Check the intersection between two boxes
    ///
    /// This overload returns the overlapped box in the
    /// \a intersection parameter.
    ///
    /// \param box          Box to test
    /// \param intersection Box to be filled with the intersection
    ///
    /// \return True if boxes overlap, false otherwise
    ///
    /// \see contains
    ///
    ////////////////////////////////////////////////////////////
    bool intersects(const Box<T>& box, Box<T>& intersection) const;

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    T left;   ///< Left coordinate of the box
    T top;    ///< Top coordinate of the box
    T front;  ///< Front coordinate of the box
    T width;  ///< Width of the box
    T height; ///< Height of the box
    T depth;  ///< Depth of the box
};

////////////////////////////////////////////////////////////
/// \relates Box
/// \brief Overload of binary operator ==
///
/// This operator compares strict equality between two boxes.
///
/// \param left  Left operand (a box)
/// \param right Right operand (a box)
///
/// \return True if \a left is equal to \a right
///
////////////////////////////////////////////////////////////
template <typename T>
bool operator ==(const Box<T>& left, const Box<T>& right);

////////////////////////////////////////////////////////////
/// \relates Box
/// \brief Overload of binary operator !=
///
/// This operator compares strict difference between two boxes.
///
/// \param left  Left operand (a box)
/// \param right Right operand (a box)
///
/// \return True if \a left is not equal to \a right
///
////////////////////////////////////////////////////////////
template <typename T>
bool operator !=(const Box<T>& left, const Box<T>& right);

#include <SFML/Graphics/Box.inl>

// Create typedefs for the most common types
typedef Box<int>   IntBox;
typedef Box<float> FloatBox;

} // namespace sf


#endif // SFML_BOX_HPP


////////////////////////////////////////////////////////////
/// \class sf::Box
/// \ingroup graphics
///
/// A box is defined by its top-left-front corner and its size.
/// It is a very simple class defined for convenience, so
/// its member variables (left, top, front, width, height
/// and depth) are public and can be accessed directly, just like
/// the vector classes (Vector2 and Vector3) and the Rect class.
///
/// To keep things simple, sf::Box doesn't define
/// functions to emulate the properties that are not directly
/// members (such as right, bottom, center, etc.), it rather
/// only provides intersection functions.
///
/// sf::Box uses the usual rules for its boundaries:
/// \li The left, top and front edges are included in the box's area
/// \li The right (left + width), bottom (top + height) and back (front + depth) edges are excluded from the box's area
///
/// This means that sf::IntBox(0, 0, 0, 1, 1, 1) and
/// sf::IntBox(1, 1, 1, 1, 1, 1) don't intersect.
///
/// sf::Box is a template and may be used with any numeric type, but
/// for simplicity the instanciations used by SFML are typedefed:
/// \li sf::Box<int> is sf::IntBox
/// \li sf::Box<float> is sf::FloatBox
///
/// So that you don't have to care about the template syntax.
///
/// Usage example:
/// \code
/// // Define a box, located at (0, 0, 0) with a size of 20x5x10
/// sf::IntBox box1(0, 0, 0, 20, 5, 10);
///
/// // Define another box, located at (4, 2, 3) with a size of 18x10x14
/// sf::Vector3i position(4, 2, 3);
/// sf::Vector3i size(18, 10, 14);
/// sf::IntBox box2(position, size);
///
/// // Test intersections with the point (3, 1)
/// bool b1 = box1.contains(3, 1, 2); // true
/// bool b2 = box2.contains(3, 1, 2); // false
///
/// // Test the intersection between box1 and box2
/// sf::IntBox result;
/// bool b3 = box1.intersects(box2, result); // true
/// // result == (4, 2, 3, 16, 3, 7)
/// \endcode
///
////////////////////////////////////////////////////////////
