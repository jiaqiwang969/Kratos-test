/*
//  KRATOS  _____________
//         /  _/ ____/   |
//         / // / __/ /| |
//       _/ // /_/ / ___ |
//      /___/\____/_/  |_| Application
//
//  Main authors:   Thomas Oberbichler
*/

#include "iga_application_variables.h"

namespace Kratos
{

KRATOS_CREATE_VARIABLE(double, CROSS_AREA)
KRATOS_CREATE_VARIABLE(double, PRESTRESS_CAUCHY)
KRATOS_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(PRESTRESS)
KRATOS_CREATE_VARIABLE(Vector, TANGENTS)

KRATOS_CREATE_VARIABLE(double, FORCE_PK2_1D)
KRATOS_CREATE_VARIABLE(double, FORCE_CAUCHY_1D)
KRATOS_CREATE_VARIABLE(double, PRINCIPAL_STRESS_1)
KRATOS_CREATE_VARIABLE(double, PRINCIPAL_STRESS_2)

KRATOS_CREATE_VARIABLE(Matrix, LOCAL_ELEMENT_ORIENTATION)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(LOCAL_PRESTRESS_AXIS_1)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(LOCAL_PRESTRESS_AXIS_2)

KRATOS_CREATE_VARIABLE(double, RAYLEIGH_ALPHA)
KRATOS_CREATE_VARIABLE(double, RAYLEIGH_BETA)

// 5p Director Shell Variables
KRATOS_CREATE_VARIABLE(bool, DIRECTOR_COMPUTED)
KRATOS_CREATE_VARIABLE(Vector, DIRECTOR)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(DIRECTORINC)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(MOMENTDIRECTORINC)
KRATOS_CREATE_VARIABLE(Matrix, DIRECTORTANGENTSPACE)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(MOMENT_LINE_LOAD)

//Load Condition Variables
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(POINT_LOAD)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(LINE_LOAD)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(SURFACE_LOAD)

KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(DEAD_LOAD)
KRATOS_CREATE_VARIABLE(double, PRESSURE_FOLLOWER_LOAD)

//Stress Recovery Variables
KRATOS_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(PK2_STRESS)
KRATOS_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(CAUCHY_STRESS)
KRATOS_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(CAUCHY_STRESS_TOP)
KRATOS_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(CAUCHY_STRESS_BOTTOM)
KRATOS_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(MEMBRANE_FORCE)
KRATOS_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(INTERNAL_MOMENT)

KRATOS_CREATE_VARIABLE(double, SHEAR_FORCE_1)
KRATOS_CREATE_VARIABLE(double, SHEAR_FORCE_2)

// Control parameters of conservative or non-conservative integration
KRATOS_CREATE_VARIABLE(bool, INTEGRATE_CONSERVATIVE)

//Penalty Variables
KRATOS_CREATE_VARIABLE(double, PENALTY_FACTOR)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(VECTOR_LAGRANGE_MULTIPLIER_REACTION)

//Nitsche Variables
KRATOS_CREATE_VARIABLE(double, NITSCHE_STABILIZATION_FACTOR)
KRATOS_CREATE_VARIABLE(int, EIGENVALUE_NITSCHE_STABILIZATION_SIZE)
KRATOS_CREATE_VARIABLE(Vector, EIGENVALUE_NITSCHE_STABILIZATION_VECTOR)
KRATOS_CREATE_VARIABLE(int, BUILD_LEVEL)

} // namespace Kratos