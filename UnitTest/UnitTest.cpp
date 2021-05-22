#include "pch.h"
#include "CppUnitTest.h"
#include "../Engine/Matrix4x4.h"
#include "../Engine/String.h"
#include "../ThirdParty/include/glm/detail/type_mat4x4.hpp"
#include "../ThirdParty/include/glm/ext/matrix_transform.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

bool compare(const Matrix4x4& m1, glm::mat4 m2)
{
	return	m1.data[0].x == m2[0][0] && m1.data[0].y == m2[0][1] && m1.data[0].z == m2[0][2] && m1.data[0].w == m2[0][3] &&
			m1.data[1].x == m2[1][0] && m1.data[1].y == m2[1][1] && m1.data[1].z == m2[1][2] && m1.data[1].w == m2[1][3] &&
			m1.data[2].x == m2[2][0] && m1.data[2].y == m2[2][1] && m1.data[2].z == m2[2][2] && m1.data[2].w == m2[2][3] &&
			m1.data[3].x == m2[3][0] && m1.data[3].y == m2[3][1] && m1.data[3].z == m2[3][2] && m1.data[3].w == m2[3][3];
}

namespace HexaTest
{
	TEST_CLASS(MatrixTest)
	{
	public:
		TEST_METHOD(Base)
		{
			Matrix4x4 m1;
			glm::mat4 m2(1.0f);

			Assert::IsTrue(compare(m1, cast_object<glm::mat4>(m1)));
		}
		
		TEST_METHOD(Translate)
		{
			Matrix4x4 m1 = Matrix4x4().translate(Vector3(1, 2, 3));

			glm::mat4 m2(1.0f);
			m2 = glm::translate(m2, glm::vec3(1, 2, 3));

			Assert::IsTrue(compare(m1, m2));
		}

		TEST_METHOD(Rotate)
		{
			Quaternion rot = Quaternion(Vector3(23, 35, 235));
			
			Matrix4x4 m1 = Matrix4x4().rotate(rot);

			glm::mat4 m2(1.0f);
			m2 = glm::rotate(m2, rot.axis_angle(), cast_object<glm::vec3>(rot.axis()));

			Assert::IsTrue(compare(m1, m2));
		}

		TEST_METHOD(Scale)
		{
			Matrix4x4 m1 = Matrix4x4().scale(Vector3(1, 2, 3));

			glm::mat4 m2(1.0f);
			m2 = glm::scale(m2, glm::vec3(1, 2, 3));

			Assert::IsTrue(compare(m1, m2));
		}

		TEST_METHOD(Multiply)
		{
			Quaternion rot1 = Quaternion(Vector3(23, 35, 235));
			Quaternion rot2 = Quaternion(Vector3(36, 234, 129));

			auto a = rot2.axis();
			auto b = rot2.axis_angle();
			
			Matrix4x4 m11 = Matrix4x4().translate(Vector3(1, 2, 3)).rotate(rot1).scale(Vector3(1, 2, 3));
			Matrix4x4 m12 = Matrix4x4()/*.translate(Vector3(4, 2, 6))*/.rotate(rot2);// .scale(Vector3(4, 7, 2));

			glm::mat4 m21(1.0f);
			m21 = glm::translate(m21, glm::vec3(1, 2, 3));
			m21 = glm::rotate(m21, rot1.axis_angle(), cast_object<glm::vec3>(rot1.axis()));
			m21 = glm::scale(m21, glm::vec3(1, 2, 3));

			glm::mat4 m22(1.0f);
			//m22 = glm::translate(m22, glm::vec3(4, 2, 6));
			m22 = glm::rotate(m22, rot2.axis_angle(), cast_object<glm::vec3>(rot2.axis()));
			//m22 = glm::scale(m22, glm::vec3(4, 7, 2));

			Assert::IsTrue(compare(m11, m21));
			Assert::IsTrue(compare(m12, m22));
			//Assert::IsTrue(compare(m11 * m12, m21 * m22));
		}
	};
}
