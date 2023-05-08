#ifndef __RIGID_SPHERE_H__
#define __RIGID_SPHERE_H__

#include "sphere.h"
#include "RigidBody.h"
#include <Mathematics/Hypersphere.h>
#include <Mathematics/Logger.h>
#include <Mathematics/Matrix4x4.h>
#include <cstdint>

CLASS_PTR(RigidSphere);
class RigidSphere: public gte::RigidBody<double> 
{
public:
	RigidSphere(const double massDensity);
	void UpdateWorldQuantities();

//private:
	SpherePtr m_sphere;
};

#endif // __RIGID_SPHERE_H__