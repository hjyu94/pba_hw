#include "RigidSphere.h"
using namespace gte;

RigidSphere::RigidSphere(const double massDensity)
	: RigidBody<double>{}
{
	m_sphere = Sphere::Create();
	double mass = massDensity;
	Matrix3x3<double> bodyInertia = massDensity * Matrix3x3<double>::Identity();
	SetMass(mass);
	SetBodyInertia(bodyInertia);
	SetPosition(Vector3<double>({ m_sphere->GetCenter().x, m_sphere->GetCenter().y, m_sphere->GetCenter().z }));
	UpdateWorldQuantities();
}

void RigidSphere::UpdateWorldQuantities()
{
	auto changedCenter = glm::vec3(
		this->GetPosition().mTuple[0],
		this->GetPosition().mTuple[1],
		this->GetPosition().mTuple[2]
	);
	m_sphere->setCenter(changedCenter);
}
