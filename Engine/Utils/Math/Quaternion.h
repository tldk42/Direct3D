#pragma once
#include <cmath>

struct Quaternion
{
	float W;
	float X;
	float Y;
	float Z; // components of a Quaternion	

public:
	// Default constructor
	Quaternion(float InW = 0, float InX = 0, float InY = 0, float InZ = 0)
		: W(InW), X(InX), Y(InY), Z(InZ) {}

	// NormaliZation function
	void Normalize()
	{
		const float magnitude = sqrt(W * W + X * X + Y * Y + Z * Z);
		W /= magnitude;
		X /= magnitude;
		Y /= magnitude;
		Z /= magnitude;
	}

	// Multiplication function
	Quaternion operator*(const Quaternion& q) const
	{
		return {
			W * q.W - X * q.X - Y * q.Y - Z * q.Z,
			W * q.X + X * q.W + Y * q.Z - Z * q.Y,
			W * q.Y - X * q.Z + Y * q.W + Z * q.X,
			W * q.Z + X * q.Y - Y * q.X + Z * q.W
		};
	}

	// Conjugation function
	Quaternion Conjugation() const
	{
		return {W, -X, -Y, -Z};
	}

	// Inversion function
	Quaternion Inverse() const
	{
		const float squareMagnitude = W * W + X * X + Y * Y + Z * Z;
		return {W / squareMagnitude, -X / squareMagnitude, -Y / squareMagnitude, -Z / squareMagnitude};
	}

	
/*
 * The rotate function rotates a point in 3D space.
 * axis is the vector around which to rotate, 
 * angle is the amount to rotate in radians,
 * point is the point in 3D space that gets rotated
 */
Quaternion Rotate(const Quaternion& axis, float angle, const Quaternion& point)
{
    // normalize the axis
    Quaternion u = axis;
    u.Normalize();

    // make a quaternion out of the axis and angle
    float halfAngle = angle / 2;
    Quaternion q(cos(halfAngle), u.X * sin(halfAngle), u.Y * sin(halfAngle), u.Z * sin(halfAngle));

    // rotate the point
    Quaternion rotatedPoint = q * point * q.Inverse();

    return rotatedPoint;
}
};
