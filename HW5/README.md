HW5: Rigid Body Dynamics: Part 1 Colliding Contact

Assume there are N solid spheres with random radii and mass M and a plane with infinite mass in 3D space. Choose a coefficient of restitution as elastic as possible (say e=0.9). The inertia tensor of a solid sphere is available at: http://en.wikipedia.org/wiki/List_of_moment_of_inertia_tensors

Simulate colliding contact among spheres based on fixed time-stepping and impulse-based dynamics. Use the result of HW4 - additionally, you will need to detect the collision of spheres against the plane. No need to find the time of contact, i.e., apply an impulse to the penetration features. You shouldu refer to Baraff's course notes (notesg.pdf) for the very nice pseudo codes. Moreover, you can use rigid body data structures from the geometric tools website (https://www.geometrictools.com/Source/Physics.html); refer to RigidBody.h and PolyhderalMassProperties.h.

You may also refer to the attached reference implementation, but this does not mean that the reference implementation is perfect (still buggy sometimes).

Display the motion of spheres using texture mapping or flat shading to show the rotational motion clearly. Turn in source codes, executables, and readme files.