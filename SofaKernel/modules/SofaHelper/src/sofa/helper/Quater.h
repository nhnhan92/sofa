/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <sofa/helper/config.h>

#include <sofa/helper/fixed_array.h>
#include <sofa/helper/logging/Messaging.h>

#include <cmath>
#include <cassert>
#include <iostream>

namespace sofa::helper
{

template<class Real>
class SOFA_HELPER_API Quater
{
private:
    helper::fixed_array<Real, 4> _q;

public:
    typedef Real value_type;
    typedef std::size_t size_type;
    using Vector3 = helper::fixed_array<Real, 3>;

    Quater();
    ~Quater();
    Quater(Real x, Real y, Real z, Real w);
    template<class Real2>
    Quater(const Real2 q[]) { for (int i=0; i<4; i++) _q[i] = Real(q[i]); }
    template<class Real2>
    Quater(const Quater<Real2>& q) { for (int i=0; i<4; i++) _q[i] = Real(q[i]); }
    Quater( const Vector3& axis, Real angle );

    /** Sets this quaternion to the rotation required to rotate direction vector vFrom to direction vector vTo.        
        vFrom and vTo are assumed to be normalized.
    */
    Quater(const Vector3& vFrom, const Vector3& vTo);

    static Quater identity()
    {
        return Quater(0,0,0,1);
    }


    void set(Real x, Real y, Real z, Real w)
    {
        _q[0] = x;
        _q[1] = y;
        _q[2] = z;
        _q[3] = w;
    }


    /// Cast into a standard C array of elements.
    const Real* ptr() const
    {
        return this->_q.data();
    }

    /// Cast into a standard C array of elements.
    Real* ptr()
    {
        return &(this->_q[0]);
    }

    /// Returns true if norm of Quaternion is one, false otherwise.
    bool isNormalized();

    /// Normalize a quaternion
    void normalize();

    void clear()
    {
        _q[0]=0.0;
        _q[1]=0.0;
        _q[2]=0.0;
        _q[3]=1.0;
    }

    void fromFrame(Vector3& x, Vector3&y, Vector3&z);

    template <typename Mat33>
    void fromMatrix(const Mat33& m)
    {
        Real tr, s;

        tr = (Real)(m[0][0] + m[1][1] + m[2][2]);

        // check the diagonal
        if (tr > 0)
        {
            s = (float)sqrt(tr + 1);
            _q[3] = s * 0.5f; // w OK
            s = 0.5f / s;
            _q[0] = (Real)((m[2][1] - m[1][2]) * s); // x OK
            _q[1] = (Real)((m[0][2] - m[2][0]) * s); // y OK
            _q[2] = (Real)((m[1][0] - m[0][1]) * s); // z OK
        }
        else
        {
            if (m[1][1] > m[0][0] && m[2][2] <= m[1][1])
            {
                s = (Real)sqrt((m[1][1] - (m[2][2] + m[0][0])) + 1.0f);

                _q[1] = s * 0.5f; // y OK

                if (s != 0.0f)
                    s = 0.5f / s;

                _q[2] = (Real)((m[1][2] + m[2][1]) * s); // z OK
                _q[0] = (Real)((m[0][1] + m[1][0]) * s); // x OK
                _q[3] = (Real)((m[0][2] - m[2][0]) * s); // w OK
            }
            else if ((m[1][1] <= m[0][0] && m[2][2] > m[0][0]) || (m[2][2] > m[1][1]))
            {
                s = (Real)sqrt((m[2][2] - (m[0][0] + m[1][1])) + 1.0f);

                _q[2] = s * 0.5f; // z OK

                if (s != 0.0f)
                    s = 0.5f / s;

                _q[0] = (Real)((m[2][0] + m[0][2]) * s); // x OK
                _q[1] = (Real)((m[1][2] + m[2][1]) * s); // y OK
                _q[3] = (Real)((m[1][0] - m[0][1]) * s); // w OK
            }
            else
            {
                s = (Real)sqrt((m[0][0] - (m[1][1] + m[2][2])) + 1.0f);

                _q[0] = s * 0.5f; // x OK

                if (s != 0.0f)
                    s = 0.5f / s;

                _q[1] = (Real)((m[0][1] + m[1][0]) * s); // y OK
                _q[2] = (Real)((m[2][0] + m[0][2]) * s); // z OK
                _q[3] = (Real)((m[2][1] - m[1][2]) * s); // w OK
            }
        }
    }

    template<class Mat33>
    void toMatrix(Mat33 &m) const
    {
        m[0][0] = typename Mat33::Real (1 - 2 * (_q[1] * _q[1] + _q[2] * _q[2]));
        m[0][1] = typename Mat33::Real (2 * (_q[0] * _q[1] - _q[2] * _q[3]));
        m[0][2] = typename Mat33::Real (2 * (_q[2] * _q[0] + _q[1] * _q[3]));

        m[1][0] = typename Mat33::Real (2 * (_q[0] * _q[1] + _q[2] * _q[3]));
        m[1][1] = typename Mat33::Real (1 - 2 * (_q[2] * _q[2] + _q[0] * _q[0]));
        m[1][2] = typename Mat33::Real (2 * (_q[1] * _q[2] - _q[0] * _q[3]));

        m[2][0] = typename Mat33::Real (2 * (_q[2] * _q[0] - _q[1] * _q[3]));
        m[2][1] = typename Mat33::Real (2 * (_q[1] * _q[2] + _q[0] * _q[3]));
        m[2][2] = typename Mat33::Real (1 - 2 * (_q[1] * _q[1] + _q[0] * _q[0]));
    }

    /// Apply the rotation to a given vector
    template<class Vec>
    Vec rotate( const Vec& v ) const
    {
        return Vec(
                typename Vec::value_type((1.0f - 2.0f * (_q[1] * _q[1] + _q[2] * _q[2]))*v[0] + (2.0f * (_q[0] * _q[1] - _q[2] * _q[3])) * v[1] + (2.0f * (_q[2] * _q[0] + _q[1] * _q[3])) * v[2]),
                typename Vec::value_type((2.0f * (_q[0] * _q[1] + _q[2] * _q[3]))*v[0] + (1.0f - 2.0f * (_q[2] * _q[2] + _q[0] * _q[0]))*v[1] + (2.0f * (_q[1] * _q[2] - _q[0] * _q[3]))*v[2]),
                typename Vec::value_type((2.0f * (_q[2] * _q[0] - _q[1] * _q[3]))*v[0] + (2.0f * (_q[1] * _q[2] + _q[0] * _q[3]))*v[1] + (1.0f - 2.0f * (_q[1] * _q[1] + _q[0] * _q[0]))*v[2])
                );

    }

    /// Apply the inverse rotation to a given vector
    template<class Vec>
    Vec inverseRotate( const Vec& v ) const
    {
        return Vec(
                typename Vec::value_type((1.0f - 2.0f * (_q[1] * _q[1] + _q[2] * _q[2]))*v[0] + (2.0f * (_q[0] * _q[1] + _q[2] * _q[3])) * v[1] + (2.0f * (_q[2] * _q[0] - _q[1] * _q[3])) * v[2]),
                typename Vec::value_type((2.0f * (_q[0] * _q[1] - _q[2] * _q[3]))*v[0] + (1.0f - 2.0f * (_q[2] * _q[2] + _q[0] * _q[0]))*v[1] + (2.0f * (_q[1] * _q[2] + _q[0] * _q[3]))*v[2]),
                typename Vec::value_type((2.0f * (_q[2] * _q[0] + _q[1] * _q[3]))*v[0] + (2.0f * (_q[1] * _q[2] - _q[0] * _q[3]))*v[1] + (1.0f - 2.0f * (_q[1] * _q[1] + _q[0] * _q[0]))*v[2])
                );

    }

    /// Given two quaternions, add them together to get a third quaternion.
    /// Adding quaternions to get a compound rotation is analagous to adding
    /// translations to get a compound translation.
    Quater<Real> operator+(const Quater<Real> &q1) const;

    Quater<Real> operator*(const Quater<Real> &q1) const;

    Quater<Real> operator*(const Real &r) const;
    Quater<Real> operator/(const Real &r) const;
    void operator*=(const Real &r);
    void operator/=(const Real &r);

    /// Given two Quaters, multiply them together to get a third quaternion.

    Quater quatVectMult(const Vector3& vect);

    Quater vectQuatMult(const Vector3& vect);

    Real& operator[](size_type index)
    {
        assert(index >= 0 && index < 4);
        return _q[index];
    }

    const Real& operator[](size_type index) const
    {
        assert(index >= 0 && index < 4);
        return _q[index];
    }

    Quater inverse() const;


    Vector3 quatToRotationVector() const;

    Vector3 toEulerVector() const;


    /*! Returns the slerp interpolation of Quaternions \p a and \p b, at time \p t.

     \p t should range in [0,1]. Result is \p a when \p t=0 and \p b when \p t=1.

     When \p allowFlip is \c true (default) the slerp interpolation will always use the "shortest path"
     between the Quaternions' orientations, by "flipping" the source Quaternion if needed (see
     negate()). */
    void slerp(const Quater& a, const Quater& b, Real t, bool allowFlip=true);

    // A useful function, builds a rotation matrix in Matrix based on
    // given quaternion.

    void buildRotationMatrix(Real m[4][4]) const;
    void writeOpenGlMatrix( double* m ) const;
    void writeOpenGlMatrix( float* m ) const;

    // This function computes a quaternion based on an axis (defined by
    // the given vector) and an angle about which to rotate.  The angle is
    // expressed in radians.
    Quater axisToQuat(Vector3 a, Real phi);
    void quatToAxis(Vector3 & a, Real &phi) const;

    static Quater createQuaterFromFrame(const Vector3& lox, const Vector3& loy, const Vector3& loz)
    {
        Quater<Real> q;
        Real m[3][3];

        for (unsigned int i = 0; i < 3; i++)
        {
            m[i][0] = lox[i];
            m[i][1] = loy[i];
            m[i][2] = loz[i];
        }
        q.fromMatrix(m);
        return q;
    }

    /// Create using rotation vector (axis*angle) given in parent coordinates
    template<class V>
    static Quater createFromRotationVector(const V& a)
    {
        Real phi = Real(sqrt(a*a));
        if( phi < 1.0e-5 )
            return Quater(0,0,0,1);
        else
        {
            Real nor = 1/phi;
            Real s = Real(sin(phi/2));
            return Quater( a[0]*s*nor, a[1]*s*nor,a[2]*s*nor, Real(cos(phi/2)));
        }
    }

    /// Create a quaternion from Euler angles
    /// Thanks to https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js#L199
    enum class EulerOrder
    {
        XYZ, YXZ, ZXY, ZYX, YZX, XZY, NONE
    };

    static Quater createQuaterFromEuler( Vector3 v, EulerOrder order = EulerOrder::ZYX)
    {
        Real quat[4];

        Real c1 = cos( v[0] / 2 );
        Real c2 = cos( v[1] / 2 );
        Real c3 = cos( v[2] / 2 );

        Real s1 = sin( v[0] / 2 );
        Real s2 = sin( v[1] / 2 );
        Real s3 = sin( v[2] / 2 );

        switch(order)
        {
        case EulerOrder::XYZ:
            quat[0] = s1 * c2 * c3 + c1 * s2 * s3;
            quat[1] = c1 * s2 * c3 - s1 * c2 * s3;
            quat[2] = c1 * c2 * s3 + s1 * s2 * c3;
            quat[3] = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        case EulerOrder::YXZ:
            quat[0] = s1 * c2 * c3 + c1 * s2 * s3;
            quat[1] = c1 * s2 * c3 - s1 * c2 * s3;
            quat[2] = c1 * c2 * s3 - s1 * s2 * c3;
            quat[3] = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        case EulerOrder::ZXY:
            quat[0] = s1 * c2 * c3 - c1 * s2 * s3;
            quat[1] = c1 * s2 * c3 + s1 * c2 * s3;
            quat[2] = c1 * c2 * s3 + s1 * s2 * c3;
            quat[3] = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        case EulerOrder::ZYX:
            quat[0] = s1 * c2 * c3 - c1 * s2 * s3;
            quat[1] = c1 * s2 * c3 + s1 * c2 * s3;
            quat[2] = c1 * c2 * s3 - s1 * s2 * c3;
            quat[3] = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        case EulerOrder::YZX:
            quat[0] = s1 * c2 * c3 + c1 * s2 * s3;
            quat[1] = c1 * s2 * c3 + s1 * c2 * s3;
            quat[2] = c1 * c2 * s3 - s1 * s2 * c3;
            quat[3] = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        case EulerOrder::XZY:
            quat[0] = s1 * c2 * c3 - c1 * s2 * s3;
            quat[1] = c1 * s2 * c3 - s1 * c2 * s3;
            quat[2] = c1 * c2 * s3 + s1 * s2 * c3;
            quat[3] = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        case EulerOrder::NONE:
        default:
            msg_error("Quaternion") << "FromEuler: given order is not a valid order to create a Quaternion";
            return Quater();
        }

        Quater quatResult{ quat[0], quat[1], quat[2], quat[3] };
        return quatResult;
    }


    /// Create a quaternion from Euler angles
    static Quater fromEuler( Real alpha, Real beta, Real gamma, EulerOrder order = EulerOrder::ZYX ){
        return createQuaterFromEuler( {alpha, beta, gamma }, order );
    }

    /// Create using the entries of a rotation vector (axis*angle) given in parent coordinates
    template<class T>
    static Quater createFromRotationVector(T a0, T a1, T a2 )
    {
        Real phi = Real(sqrt(a0*a0+a1*a1+a2*a2));
        if( phi < 1.0e-5 )
            return Quater(0,0,0,1);
        else
        {
            Real nor = 1/phi;
            Real s = sin(phi/2.0);
            return Quater( a0*s*nor, a1*s*nor,a2*s*nor, cos(phi/2.0) );
        }
    }
    /// Create using rotation vector (axis*angle) given in parent coordinates
    template<class V>
    static Quater set(const V& a) { return createFromRotationVector(a); }

    /// Create using using the entries of a rotation vector (axis*angle) given in parent coordinates
    template<class T>
    static Quater set(T a0, T a1, T a2) { return createFromRotationVector(a0,a1,a2); }

    /// Return the quaternion resulting of the movement between 2 quaternions
    Quater quatDiff( Quater a, const Quater& b)
    {
        // If the axes are not oriented in the same direction, flip the axis and angle of a to get the same convention than b
        if (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3]<0)
        {
            a[0] = -a[0];
            a[1] = -a[1];
            a[2] = -a[2];
            a[3] = -a[3];
        }

        Quater q = b.inverse() * a;
        return q;
    }

    /// Return the eulerian vector resulting of the movement between 2 quaternions
    Vector3 angularDisplacement( Quater a, const Quater& b)
    {
        return quatDiff(a,b).quatToRotationVector();    // Use of quatToRotationVector instead of toEulerVector:
                                                        // this is done to keep the old behavior (before the
                                                        // correction of the toEulerVector function).
    }

    /// Sets this quaternion to the rotation required to rotate direction vector vFrom to direction vector vTo. vFrom and vTo are assumed to be normalized.
    void setFromUnitVectors(const Vector3& vFrom, const Vector3& vTo);


    // Print the quaternion (C style)
    void print();
    Quater<Real> slerp(Quater<Real> &q1, Real t);
    Quater<Real> slerp2(Quater<Real> &q1, Real t);

    void operator+=(const Quater& q2);
    void operator*=(const Quater& q2);

    bool operator==(const Quater& q) const
    {
        for (int i=0; i<4; i++)
            if ( std::abs( _q[i] - q._q[i] ) > std::numeric_limits<SReal>::epsilon() ) return false;
        return true;
    }

    bool operator!=(const Quater& q) const
    {
        for (int i=0; i<4; i++)
            if ( std::abs( _q[i] - q._q[i] ) > std::numeric_limits<SReal>::epsilon() ) return true;
        return false;
    }

    /// write to an output stream
    inline friend std::ostream& operator << ( std::ostream& out, const Quater& v )
    {
        out<<v._q[0]<<" "<<v._q[1]<<" "<<v._q[2]<<" "<<v._q[3];
        return out;
    }

    /// read from an input stream
    inline friend std::istream& operator >> ( std::istream& in, Quater& v )
    {
        in>>v._q[0]>>v._q[1]>>v._q[2]>>v._q[3];
        return in;
    }

    enum { static_size = 4 };
    static unsigned int size() {return 4;}

    /// Compile-time constant specifying the number of scalars within this vector (equivalent to the size() method)
    enum { total_size = 4 };
    /// Compile-time constant specifying the number of dimensions of space (NOT equivalent to total_size for quaternions)
    enum { spatial_dimensions = 3 };
};

#if  !defined(SOFA_HELPER_QUATER_CPP)
extern template class SOFA_HELPER_API Quater<double>;
extern template class SOFA_HELPER_API Quater<float>;
#endif

} // namespace sofa::helper



