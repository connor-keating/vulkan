#pragma once

#include <math.h>
#pragma once

#include "core.c"


#define PI 	3.14159265358979323846f
#define DegreesToRadians(degrees) (degrees*(PI/180.0f))
#define RadiansToDegrees(radians) (radians(180.0f/PI))


typedef union ivec2 ivec2;
union ivec2
{
    struct 
    {
        i32 x, y;
    };
    i32 array[2];
};

typedef union fvec2 fvec2;
union fvec2
{
    struct 
    {
        f32 x, y;
    };
    f32 array[2];
};

typedef union fvec3 fvec3;
union fvec3
{
    struct 
    {
        f32 x, y, z;
    };
    f32 array[3];
};

typedef union fvec4 fvec4;
union fvec4
{
    struct 
    {
        f32 x, y, z, w;
    };
    f32 array[4];
};

typedef f32 fmatrix2[2][2];

typedef f32 fmatrix3[3][3];

typedef f32 fmatrix4[4][4];

static void fmatrix2_identity(fmatrix2 matLoadInto)
{
    int8_t i, j, dim;
    dim = 2;
	for(i=0; i<dim; ++i)
    {
		for(j=0; j<dim; ++j)
        {
			matLoadInto[i][j] = i==j ? 1.f : 0.f;
        }
    }
}

static void fmatrix4_identity(fmatrix4 matLoadInto)
{
    int8_t i, j, dim;
    dim = 4;
	for(i=0; i<dim; ++i)
    {
		for(j=0; j<dim; ++j)
        {
			matLoadInto[i][j] = i==j ? 1.f : 0.f;
        }
    }
}


static fvec2 fvec2_add(fvec2 a, fvec2 b)
{
    fvec2 result = {0};
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}


inline void fvec2_scale(fvec2* input, f32 scalar)
{
    input->x = input->x * scalar;
    input->y = input->y * scalar;
}


inline void fvec2_array_scale(fvec2* array, i32 amount, f32 scaler)
{
    for (fvec2* index = array; index < array+amount; index++)
    {
        fvec2_scale(index, scaler); 
    }
}


static fvec3 fvec3_add(fvec3 a, fvec3 b)
{
    fvec3 result = {0};
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}


inline f32 fvec3_dot(fvec3* a, fvec3* b)
{
    f32 product = \
        a->x * b->x + \
        a->y * b->y + \
        a->z * b->z;
    return product;
}


inline void fvec3_scale(fvec3* input, f32 scalar)
{
    input->x = input->x * scalar;
    input->y = input->y * scalar;
    input->z = input->z * scalar;
}


inline void fvec3_array_scale(fvec3* array, i32 amount, f32 scaler)
{
    for (fvec3* index = array; index < array+amount; index++)
    {
        fvec3_scale(index, scaler); 
    }
}


inline void fvec3_array_translate(fvec3 *array, i32 amount, fvec3 *translation)
{
    for (fvec3* index = array; index < array+amount; index++)
    {
        *index = fvec3_add(*index, *translation); 
    }
}


/// @brief Calculate cross product between 3D vectors a and b
/// @param a 
/// @param b 
/// @return fvec3 cross product
inline fvec3 fvec3_cross(fvec3 a, fvec3 b)
{
    fvec3 result = {0};

    // ⟨u2v3−u3v2,−(u1v3−u3v1),u1v2−u2v1⟩
    result.x = (a.y * b.z) - (a.z * b.y);
    result.y = (a.z * b.x) - (a.x * b.z);
    result.z = (a.x * b.y) - (a.y * b.x);
    return result;
}

/// @brief Calculate length of input vector.
/// @param vector
/// @return f32 vector length
inline f32 fvec3_length(fvec3* vector)
{
    f32 length = sqrtf(fvec3_dot(vector, vector));
    return length;
}

/// @brief Return the input vector of length 1.
/// @param vector
/// @return Normalized vector
inline void fvec3_normalize(fvec3* vector)
{
    f32 length = fvec3_length(vector);
    fvec3_scale(vector, (1.0f / length));
}

static fvec4 quaternion_load(fvec3 axesRotation, f32 angle)
{
    fvec4 quaternion = {0};
    f32 radians = DegreesToRadians(angle);
    f32 sinAngle = sinf(radians * 0.5f);
    fvec3_normalize(&axesRotation);
    quaternion.x = axesRotation.x * sinAngle;
    quaternion.y = axesRotation.y * sinAngle;
    quaternion.z = axesRotation.z * sinAngle;
    quaternion.w = cosf(radians * 0.5f);

    return quaternion;
}

/// @brief Grassmann product that multiplies two quaternions.
/// @param p quaternion 
/// @param q quaternion
/// @return  product of p * q.
static fvec4 grassman_product(fvec4 p, fvec4 q)
{
    fvec3 pVector = {p.x, p.y, p.z};
    f32 pScale = p.w;
    fvec3 qVector = {q.x, q.y, q.z};
    f32 qScale = q.w;

    f32 resultScale = pScale * qScale - fvec3_dot(&pVector, &qVector);

    fvec3_scale(&qVector, pScale);
    fvec3_scale(&pVector, qScale);
    fvec3 cross = fvec3_cross(pVector, qVector);
    fvec3 resultVector = fvec3_add(fvec3_add(qVector, pVector), cross);

    fvec4 result = {resultVector.x, resultVector.y, resultVector.z, resultScale};
    return result;
}

/// @brief Rotate a 3D vector using a quaternion.
/// @param vector 
/// @param quaternion of unit lengh
/// @return fvec3 rotated vector 
static fvec3 fvec3_rotate(fvec3 vector, fvec4 quaternion)
{
    fvec4 p = {vector.x, vector.y, vector.z, 0.0f};
    fvec4 product = grassman_product(p, quaternion);
    // Our quaternions are always unit length!
    fvec4 conjugate = {-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w};
    fvec4 finalProduct = grassman_product(product, conjugate);
    fvec3 result = {finalProduct.x, finalProduct.y, finalProduct.z};
    return result;
}


/// @brief Convert quaternion to a 3x3 matrix.
/// @param q quaternion
/// @return fmatrix3
static void quaternion_to_matrix(fvec4 q, fmatrix3 matLoadInto)
{
    matLoadInto[0][0] = 1 - (2*q.y*q.y) - (2*q.z*q.z);
    matLoadInto[0][1] = (2*q.x*q.y) + (2*q.z*q.w);
    matLoadInto[0][2] = (2*q.x*q.z) - (2*q.y*q.w);
    matLoadInto[1][0] = (2*q.x*q.y) - (2*q.z*q.w);
    matLoadInto[1][1] = 1 - (2*q.x*q.x) - (2*q.z*q.z);
    matLoadInto[1][2] = (2*q.y*q.z) + (2*q.x*q.w);
    matLoadInto[2][0] = (2*q.x*q.z) + (2*q.y*q.w);
    matLoadInto[2][1] = (2*q.y*q.z) - (2*q.x*q.w);
    matLoadInto[2][2] = 1 - (2*q.x*q.x) - (2*q.y*q.y);

}

static void fmatrix4_zero(fmatrix4 matrix)
{

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matrix[i][j] = 0; 
        }
    } 
}

static void fmatrix4_rotation(fvec4 quaternion, fmatrix4 matLoadInto)
{
    // fmatrix4_identity(matLoadInto);
    // fvec4 quaternion = quaternion_load(axesOfRotation, rotationAmount);
    //TODO: Can you combine input mat with multiply?
    fmatrix3 myRotation = {0};
    quaternion_to_matrix(quaternion, myRotation);
    matLoadInto[0][0] = myRotation[0][0];
    matLoadInto[0][1] = myRotation[0][1];
    matLoadInto[0][2] = myRotation[0][2];
    matLoadInto[1][0] = myRotation[1][0];
    matLoadInto[1][1] = myRotation[1][1];
    matLoadInto[1][2] = myRotation[1][2];
    matLoadInto[2][0] = myRotation[2][0];
    matLoadInto[2][1] = myRotation[2][1];
    matLoadInto[2][2] = myRotation[2][2];
}

static void fmatrix4_projection_orthographic(fmatrix4 projection_matrix, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar)
{
    f32 width = right - left;
    f32 height = top - bottom;
    f32 depth = zfar - znear;

    projection_matrix[0][0] =  2.0f / width;
    projection_matrix[1][1] =  2.0f / height;
    projection_matrix[2][2] = -2.0f / depth;
    projection_matrix[3][3] = 1.0f;

    projection_matrix[3][0] = ( (right + left) / width ) * -1.0f;
    projection_matrix[3][1] = ( (top + bottom) / height) * -1.0f;
    projection_matrix[3][2] = ( (zfar + znear) / depth ) * -1.0f;
}

static void fmatrix4_mult(fmatrix4 c, fmatrix4 a, fmatrix4 b)
{
   for (int i = 0; i < 4; i++)
   {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                c[i][j] += a[i][k] * b[k][j]; 
            }
        }
   } 
}

static void fmatrix4_transform(fmatrix4 output, fvec3 translation, fvec3 scale, fvec3 rotate)
{
    UNREFERENCED_PARAMETER(translation);
    UNREFERENCED_PARAMETER(rotate);

    output[0][0] = scale.x;
    output[1][1] = scale.y;
    output[2][2] = scale.z;
    output[3][3] = 1.0f;
}

static void fvec3_matmul(fvec3 vector, fmatrix3 matrix)
{
    vector.x = (
        (vector.x * matrix[0][0]) +
        (vector.y * matrix[0][1]) +
        (vector.z * matrix[0][2]) 
    );
    vector.y = (
        (vector.x * matrix[1][0]) +
        (vector.y * matrix[1][1]) +
        (vector.z * matrix[1][2]) 
    );
    vector.z = (
        (vector.x * matrix[2][0]) +
        (vector.y * matrix[2][1]) +
        (vector.z * matrix[2][2]) 
    );

}
