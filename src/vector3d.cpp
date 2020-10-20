#include <cmath>
#include <vector>
#include "vector3d.h"

Vector3D::Vector3D(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D::Vector3D(const Vector3D& copy)
{
	this->x = copy.x;
	this->y = copy.y;
	this->z = copy.z;
}

double Vector3D::Dot(const Vector3D& rhs) const
{
	return ((x * rhs.x) + (y * rhs.y) + (z * rhs.z));
}

double Vector3D::Magnitude() const
{
	return (std::sqrt((x * x) + (y * y) + (z * z)));
}

std::vector<Vector3D> Vector3D::Repmat(const Vector3D& vectorToRepeat, int timesToRepeat)
{
	std::vector<Vector3D> result(timesToRepeat);
	for (auto& vector : result)
	{
		vector = vectorToRepeat;
	}
	return result;
}

Vector3D Vector3D::UnitVector() const
{
	if (this->Magnitude() != 0)
	{
		return Vector3D((*this).Scale(1 / this->Magnitude()));
	}
	else
	{
		return Vector3D(0.0, 0.0, 0.0);
	}
}

Vector3D Vector3D::RotateZ(double degrees) const
{
	double rad = degrees * 3.14159265 / 180;

	double x = Vector3D(std::cos(rad), -std::sin(rad), 0).Dot(*this); // dot of row one
	double y = Vector3D(std::sin(rad), std::cos(rad), 0).Dot(*this); // dot of row two
	double z = Vector3D(0.0, 0.0, 1.0).Dot(*this); // dot of row three

	return Vector3D(x, y, z);
}

Vector3D Vector3D::Project(const Vector3D& onto)
{
	double scalar = this->Dot(onto) / std::pow(onto.Magnitude(), 2);
	return Vector3D(onto.getX() * scalar, onto.getY() * scalar, onto.getZ() * scalar);
}

Vector3D Vector3D::Scale(double rhs) const
{
	return Vector3D(getX() * rhs, getY() * rhs, getZ() * rhs);
}

Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs)
{
	return Vector3D(lhs.getX() - rhs.getX(), lhs.getY() - rhs.getY(), lhs.getZ() - rhs.getZ());
}

// Too precise, need abs
bool operator==(const Vector3D& lhs, const Vector3D& rhs)
{
	return std::abs((lhs.getX() - rhs.getX()) < 1) &&
		(std::abs(lhs.getY() - rhs.getY()) < 1) &&
		(std::abs(lhs.getZ() - rhs.getZ()) < 1);
}

bool operator!=(const Vector3D& lhs, const Vector3D& rhs)
{
	return !(lhs == rhs);
}

Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs)
{
	return Vector3D(lhs.getX() + rhs.getX(), lhs.getY() + rhs.getY(), lhs.getZ() + rhs.getZ());
}

Vector3D operator*(const Vector3D& lhs, double rhs)
{
	return Vector3D(lhs.getX() * rhs, lhs.getY() * rhs, lhs.getZ() * rhs);
}

Vector3D operator/(const Vector3D& lhs, double rhs)
{
	return Vector3D(lhs.getX() / rhs, lhs.getY() / rhs, lhs.getZ() / rhs);
}

Vector3D& Vector3D::operator+=(const Vector3D& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& rhs)
{
	*this = *this - rhs;
	return *this;
}
