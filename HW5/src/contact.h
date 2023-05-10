#ifndef __CONTACT_H__
#define __CONTACT_H__

#include "common.h"
#include "rigidbody.h"

CLASS_PTR(Contact)
class Contact {
public:
	static ContactPtr Create(RigidBodyPtr a, RigidBodyPtr b, glm::vec3 p, glm::vec3 n)
	{
		ContactPtr contact = ContactPtr(new Contact(a, b, p, n));
		return contact;
	}

	RigidBodyPtr a;
	RigidBodyPtr b;
	glm::vec3 p; // contact point
	glm::vec3 n; // normal vector

	~Contact() {}

private:
	Contact(RigidBodyPtr a, RigidBodyPtr b, glm::vec3 p, glm::vec3 n)
		: a(a), b(b), p(p), n(n)
	{}
};

#endif