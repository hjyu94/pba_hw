#ifndef __RIGID_PLANE_H__
#define __RIGID_PLANE_H__

#include "sphere.h"
#include "RigidBody.h"
#include <Mathematics/Hypersphere.h>
#include <Mathematics/Logger.h>
#include <Mathematics/Matrix4x4.h>
#include <cstdint>

using namespace gte;

CLASS_PTR(RigidPlane);
class RigidPlane : public gte::RigidBody<double>
{
public:
    RigidPlane(Plane3<double> const& plane);
    virtual ~RigidPlane() = default;

    inline Plane3<double> const& GetPlane() const
    {
        return mPlane;
    }

    inline double GetSignedDistance(Vector3<double> const& point) const
    {
        return Dot(mPlane.normal, point) - mPlane.constant;
    }

private:
    Plane3<double> mPlane;
};

#endif // __RIGID_PLANE_H__