/*
==============================================================================
KratosStructuralApplication 
A library based on:
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi, Janosch Stascheit, Felix Nagel 
pooyan@cimne.upc.edu 
rrossi@cimne.upc.edu
janosch.stascheit@rub.de
nagel@sd.rub.de
- CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain
- Ruhr-University Bochum, Institute for Structural Mechanics, Germany


Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

The  above  copyright  notice  and  this permission  notice  shall  be
included in all copies or substantial portions of the Software.

THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==============================================================================
*/
/* *********************************************************   
*          
*   Last Modified by:    $Author: kazem $
*   Date:                $Date: 2009-01-16 10:50:04 $
*   Revision:            $Revision: 1.1 $
*
* ***********************************************************/


// System includes 
#include <iostream>

// External includes 
#include<cmath>

// Project includes 

#include "includes/define.h"
#include "constitutive_laws/plane_strain.h"

#include "includes/constitutive_law.h"

#include "utilities/math_utils.h"
#include "includes/variables.h"
#include "includes/process_info.h"
#include "structural_application.h"
#include "includes/properties.h"

namespace Kratos
{
    namespace PlaneStrainAuxiliaries
    {
        boost::numeric::ublas::bounded_matrix<double,2,2> mstemp;
        #pragma omp threadprivate(mstemp)
        boost::numeric::ublas::bounded_matrix<double,2,2> msaux;
        #pragma omp threadprivate(msaux)
    }

    using namespace PlaneStrainAuxiliaries;

	/**
	 *	TO BE TESTED!!!
	 */
	PlaneStrain::PlaneStrain() 
	: ConstitutiveLaw< Node<3> >()
	{
	}
	/**
	 *	TO BE TESTED!!!
	 */
	PlaneStrain::~PlaneStrain()
	{
	}
	
	
	bool PlaneStrain::Has( const Variable<double>& rThisVariable )
	{
		return false;
	}
	
	bool PlaneStrain::Has( const Variable<Vector>& rThisVariable )
	{
		return false;
	}
	
	bool PlaneStrain::Has( const Variable<Matrix>& rThisVariable )
	{
		return false;
	}
	
	double PlaneStrain::GetValue( const Variable<double>& rThisVariable )
	{
	    KRATOS_ERROR(std::logic_error, "double Variable case not considered", "");
	}
	
	Vector PlaneStrain::GetValue( const Variable<Vector>& rThisVariable )
	{
	    KRATOS_ERROR(std::logic_error, "Vector Variable case not considered", "");
	}
	
	Matrix PlaneStrain::GetValue( const Variable<Matrix>& rThisVariable )
	{
	    KRATOS_ERROR(std::logic_error, "Matrix Variable case not considered", "");
	}

    void PlaneStrain::SetValue( const Variable<double>& rThisVariable, const double& rValue, 
                                const ProcessInfo& rCurrentProcessInfo )
	{
	}
	
    void PlaneStrain::SetValue( const Variable<Vector>& rThisVariable, const Vector& rValue, 
                                const ProcessInfo& rCurrentProcessInfo )
	{
	}
	
    void PlaneStrain::SetValue( const Variable<Matrix>& rThisVariable, const Matrix& rValue, 
                                const ProcessInfo& rCurrentProcessInfo )
	{
	}
    
    void PlaneStrain::Calculate(const Variable<Matrix >& rVariable, Matrix& rResult, 
                   const ProcessInfo& rCurrentProcessInfo)
    {
    }
	
	/**
	 *	TO BE TESTED!!!
	 */
	void PlaneStrain::InitializeMaterial( const Properties& props,
			const GeometryType& geom,
			const Vector& ShapeFunctionsValues )
	{
		mE = props[YOUNG_MODULUS];
		mNU = props[POISSON_RATIO];
//	KRATOS_WATCH("INSIDE THE CONS LAW")
//		KRATOS_WATCH(mE)
//			KRATOS_WATCH(mNU)
	}
		
	/**
	 *	TO BE TESTED!!!
	 */
	void PlaneStrain::CalculateElasticMatrix(Matrix& C, const double E, const double NU)
	{ 
 		double c1 = E*(1.00-NU) / ((1.00 +NU)*(1.00 - 2*NU));
 		double c2 = E*NU / ((1.00 +NU)*(1.00 - 2*NU));
 		double c3 = 0.5*E / (1 + NU);
 
 		C(0,0) = c1;	C(0,1) = c2;	C(0,2) = 0.0;
 		C(1,0) = c2;	C(1,1) = c1;	C(1,2) = 0.0;
 		C(2,0) = 0.0;	C(2,1) = 0.0;	C(2,2) = c3;
	//KRATOS_WATCH("inside D");
	}
	
	/**
	 *	TO BE TESTED!!!
	 */
	void PlaneStrain::CalculateStress(const Vector& StrainVector, Vector& StressVector)
	{
 		double c1 = mE*(1.00-mNU) / ((1.00 +mNU)*(1.00 - 2*mNU));
 		double c2 = mE*mNU / ((1.00 +mNU)*(1.00 - 2*mNU));
 		double c3 = 0.5*mE / (1 + mNU);

		StressVector[0] = c1*StrainVector[0] + c2 * (StrainVector[1])	;
		StressVector[1] = c1*StrainVector[1] + c2 * (StrainVector[0])	;
		StressVector[2] = c3*StrainVector[2];

	}


	/**
	 *	TO BE REVIEWED!!!
	 */
	void PlaneStrain::CalculateConstitutiveMatrix(const Vector& StrainVector, Matrix& rResult)
	{
		CalculateElasticMatrix( rResult, mE, mNU );
	}


	
	
	
    //**********************************************************************
    void PlaneStrain::CalculateCauchyStresses(
		Vector& rCauchy_StressVector,
		const Matrix& rF,
		const Vector& rPK2_StressVector,
		const Vector& rGreenLagrangeStrainVector)
    {
		Matrix S = MathUtils<double>::StressVectorToTensor( rPK2_StressVector );

        double J = MathUtils<double>::Det2( rF );

		noalias(mstemp) = prod(rF,S);
		noalias(msaux) = prod(mstemp,trans(rF));
		msaux *= J;

		if(rCauchy_StressVector.size() != 3)
			rCauchy_StressVector.resize(3);
		
		rCauchy_StressVector[0] = msaux(0,0);
		rCauchy_StressVector[1] = msaux(1,1);
		rCauchy_StressVector[2] = msaux(1,2);
    }
} // Namespace Kratos
