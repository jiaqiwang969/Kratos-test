//   
//   Project Name:        Kratos       
//   Last modified by:    $Author: rrossi $
//   Date:                $Date: 2008-12-09 20:20:55 $
//   Revision:            $Revision: 1.5 $
//
//

// System includes 

#if defined(KRATOS_PYTHON)
// External includes 
#include <boost/python.hpp>

//Trilinos includes
#include "mpi.h"
#include "Epetra_MpiComm.h"
#include "Epetra_Comm.h"
#include "Epetra_Map.h"
#include "Epetra_Vector.h"
#include "Epetra_FECrsGraph.h"
#include "Epetra_FECrsMatrix.h"
#include "Epetra_FEVector.h"
#include "Epetra_IntSerialDenseVector.h"
#include "Epetra_SerialDenseMatrix.h"


// Project includes 
#include "includes/define.h"
#include "trilinos_application.h"
#include "trilinos_space.h"
#include "spaces/ublas_space.h"
// #include "add_trilinos_linear_solvers_to_python.h"
#include "includes/model_part.h"

//strategies
#include "solving_strategies/strategies/solving_strategy.h"
#include "solving_strategies/strategies/residualbased_linear_strategy.h"
#include "solving_strategies/strategies/residualbased_newton_raphson_strategy.h"

//schemes
#include "solving_strategies/schemes/scheme.h"
#include "custom_strategies/schemes/trilinos_residualbased_incrementalupdate_static_scheme.h"

//convergence criterias
#include "solving_strategies/convergencecriterias/convergence_criteria.h"
#include "solving_strategies/convergencecriterias/displacement_criteria.h"

//Builder And Solver
// #include "solving_strategies/builder_and_solvers/builder_and_solver.h"
#include "custom_strategies/builder_and_solvers/trilinos_residualbased_elimination_builder_and_solver.h"
#include "custom_strategies/convergencecriterias/trilinos_displacement_criteria.h"


//linear solvers
#include "linear_solvers/linear_solver.h"

//utilities
#include "python/pointer_vector_set_python_interface.h"

//teuchos parameter list
#include "Teuchos_ParameterList.hpp"

#include "external_includes/aztec_solver.h"
#include "external_includes/amesos_solver.h"
#include "external_includes/ml_solver.h"



namespace Kratos
{

namespace Python
{

using namespace boost::python;

void EraseAll(std::string& ThisString, std::string ToBeRemoved)
{
int position;
while( (position = ThisString.find_first_of(ToBeRemoved)) >= 0 ) 
{
	ThisString.erase(position ,ToBeRemoved.size() );
}

}

std::string ErrorCleaner(std::string const& Input)
{
std::string output(Input);

EraseAll(output, "boost::numeric::");

return output;
}



typedef TrilinosSpace<Epetra_FECrsMatrix, Epetra_FEVector> TrilinosSparseSpaceType;
typedef UblasSpace<double, Matrix, Vector> TrilinosLocalSpaceType;
typedef LinearSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > TrilinosLinearSolverType;

typedef Epetra_FECrsMatrix FECrsMatrix;

	void prova(TrilinosSparseSpaceType& dummy, FECrsMatrix& rX)
	{    rX.PutScalar(0.0);}  

	double Dot(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::VectorType& rX, TrilinosSparseSpaceType::VectorType& rY)
	{   return dummy.Dot(rX,rY);} 

	void ScaleAndAdd(TrilinosSparseSpaceType& dummy, const double A, const TrilinosSparseSpaceType::VectorType& rX, const double B, TrilinosSparseSpaceType::VectorType& rY) 
	// rY = (A * rX) + (B * rY) 
	{	dummy.ScaleAndAdd(A,rX,B, rY);	}

	
	void Mult(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::MatrixType& rA, TrilinosSparseSpaceType::VectorType& rX, TrilinosSparseSpaceType::VectorType& rY)
	//rY=A*rX (the product is stored inside the rY)
	{   dummy.Mult(rA, rX, rY);}
	
 	void TransposeMult(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::MatrixType& rA, TrilinosSparseSpaceType::VectorType& rX, TrilinosSparseSpaceType::VectorType& rY)
 	{   dummy.TransposeMult(rA, rX, rY);}
	
	TrilinosSparseSpaceType::IndexType Size(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::VectorType const& rV)
	{return dummy.Size(rV);} 

	void ResizeMatrix(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::MatrixType& A, unsigned int i1, unsigned int i2)
	{	dummy.Resize(A,i1,i2);	}			

	void ResizeVector(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::VectorType& x, unsigned int i1)
	{	dummy.Resize(x,i1);	}

	void SetToZeroMatrix(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::MatrixType& A)
	{	dummy.SetToZero(A);	}

	void SetToZeroVector(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::VectorType& x)
	{	dummy.SetToZero(x);	}

	void ClearMatrix(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::MatrixType& A)
	{	dummy.Clear(A);	}

	void ClearVector(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::VectorType& x)
	{	dummy.Clear(x);	}

	double TwoNorm(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::VectorType& x)
	{	return dummy.TwoNorm(x);	}

	void UnaliasedAdd(TrilinosSparseSpaceType& dummy, TrilinosSparseSpaceType::VectorType& x, const double A, const TrilinosSparseSpaceType::VectorType& rY) // x+= a*Y
	{	dummy.UnaliasedAdd(x,A,rY);	}

// bool (TrilinosLinearSolverType::*pointer_to_solve)(TrilinosLinearSolverType::SparseMatrixType& rA, TrilinosLinearSolverType::VectorType& rX, TrilinosLinearSolverType::VectorType& rB) = &TrilinosLinearSolverType::Solve;

	//************************************************************************************************
	Epetra_MpiComm CreateCommunicator()
	{
		Epetra_MpiComm comm(MPI_COMM_WORLD);
		return comm;
	}
	
	//************************************************************************************************


	//************************************************************************************************
	TrilinosSparseSpaceType::MatrixPointerType CreateEmptyMatrixPointer(TrilinosSparseSpaceType& dummy, Epetra_MpiComm& Comm){ return dummy.CreateEmptyMatrixPointer(Comm);}
	TrilinosSparseSpaceType::VectorPointerType CreateEmptyVectorPointer(TrilinosSparseSpaceType& dummy, Epetra_MpiComm& Comm){ return dummy.CreateEmptyVectorPointer(Comm);}

 	Epetra_FECrsMatrix& GetMatRef(TrilinosSparseSpaceType::MatrixPointerType& dummy)
 	{	return *dummy;	}
 	Epetra_FEVector& GetVecRef(TrilinosSparseSpaceType::VectorPointerType& dummy)
 	{	return *dummy;	}
	//************************************************************************************************

	void MoveMesh( Scheme< TrilinosSparseSpaceType, TrilinosLocalSpaceType >& dummy, ModelPart::NodesContainerType& rNodes)
	{	
		for(ModelPart::NodeIterator i = rNodes.begin() ; i != rNodes.end() ; ++i)
		{
			const array_1d<double,3>& disp = i->FastGetSolutionStepValue(DISPLACEMENT);
			(i)->X() = (i)->X0() + disp[0];
			(i)->Y() = (i)->Y0() + disp[1];
			(i)->Z() = (i)->Z0() + disp[2];
		}
	}	


	//************************************************************************************************
	//************************************************************************************************
	//teuchos paramter list
	void 	SetDoubleValue(Teuchos::ParameterList& dummy, const string &name, double value)
	{   dummy.set(name,value);	}
	void 	SetIntValue(Teuchos::ParameterList& dummy, const string &name, int value)
	{   dummy.set(name,value);	}
	void 	SetCharValue(Teuchos::ParameterList& dummy, const string &name, const char value[])
	{   dummy.set(name,value);	}
	
	
	
	BOOST_PYTHON_MODULE(KratosTrilinosApplication)
	{

			class_< Epetra_MpiComm  >("Epetra_MpiComm", init< Epetra_MpiComm& >() )
			;
	
			class_< Epetra_FECrsMatrix >("Epetra_FECrsMatrix", init< Epetra_FECrsMatrix& >() )
			;

			class_< Epetra_FEVector >("Epetra_FEVector", init< Epetra_FEVector& >() )
			;

			class_< TrilinosSparseSpaceType::MatrixPointerType  >("TrilinosMatrixPointer", init< TrilinosSparseSpaceType::MatrixPointerType >() )
  				.def("GetReference", GetMatRef, return_value_policy<reference_existing_object>() )
			;

			class_< TrilinosSparseSpaceType::VectorPointerType >("TrilinosVectorPointer", init< TrilinosSparseSpaceType::VectorPointerType >() )
  				.def("GetReference", GetVecRef, return_value_policy<reference_existing_object>() )
			;


	
		class_<KratosTrilinosApplication, 
				KratosTrilinosApplication::Pointer, 
				bases<KratosApplication>, boost::noncopyable >("KratosTrilinosApplication")
				;
	
		
		class_<TrilinosLinearSolverType, TrilinosLinearSolverType::Pointer>("TrilinosLinearSolver")
/*		.def("Initialize",&TrilinosLinearSolverType::Initialize)
		.def("Solve",pointer_to_solve)
			//.def("",&LinearSolverType::)
			.def(self_ns::str(self))*/
			;
	
	
	
		typedef SolvingStrategy< TrilinosSparseSpaceType, TrilinosLocalSpaceType, TrilinosLinearSolverType > TrilinosBaseSolvingStrategyType;
		typedef Scheme< TrilinosSparseSpaceType, TrilinosLocalSpaceType > TrilinosBaseSchemeType;
		typedef TrilinosResidualBasedEliminationBuilderAndSolver< TrilinosSparseSpaceType, TrilinosLocalSpaceType, TrilinosLinearSolverType > TrilinosBuilderAndSolverType;
	

	
		//********************************************************************
		//********************************************************************
		class_< TrilinosBaseSchemeType, boost::noncopyable >
				("TrilinosScheme", init< >() )
				.def("Initialize", &TrilinosBaseSchemeType::Initialize )
				.def("SchemeIsInitialized", &TrilinosBaseSchemeType::SchemeIsInitialized )
				.def("ElementsAreInitialized", &TrilinosBaseSchemeType::ElementsAreInitialized )
				.def("InitializeElements", &TrilinosBaseSchemeType::InitializeElements )
				.def("InitializeSolutionStep", &TrilinosBaseSchemeType::InitializeSolutionStep )
				.def("FinalizeSolutionStep", &TrilinosBaseSchemeType::FinalizeSolutionStep )
				.def("InitializeNonLinIteration", &TrilinosBaseSchemeType::InitializeNonLinIteration )
				.def("FinalizeNonLinIteration", &TrilinosBaseSchemeType::FinalizeNonLinIteration )
				.def("Predict", &TrilinosBaseSchemeType::Predict )
				.def("Update", &TrilinosBaseSchemeType::Update )
				.def("CalculateOutputData", &TrilinosBaseSchemeType::CalculateOutputData )
				.def("Clean", &TrilinosBaseSchemeType::Clean )
				.def("MoveMesh", MoveMesh )
				;
	
		class_< TrilinosResidualBasedIncrementalUpdateStaticScheme< TrilinosSparseSpaceType, TrilinosLocalSpaceType>,	
			bases< TrilinosBaseSchemeType >,  boost::noncopyable >
			(
				"TrilinosResidualBasedIncrementalUpdateStaticScheme", init< >() 
			);
	
	
		//********************************************************************
		//********************************************************************
		//convergence criteria base class
		typedef ConvergenceCriteria< TrilinosSparseSpaceType, TrilinosLocalSpaceType > TrilinosConvergenceCriteria;
		class_< TrilinosConvergenceCriteria, boost::noncopyable >("TrilinosConvergenceCriteria", init<>() )
			.def("SetActualizeRHSFlag", &ConvergenceCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >::SetActualizeRHSFlag )
			.def("GetActualizeRHSflag", &ConvergenceCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >::GetActualizeRHSflag )
			.def("PreCriteria", &ConvergenceCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >::PreCriteria )
			.def("PostCriteria", &ConvergenceCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >::PostCriteria )
			.def("Initialize", &ConvergenceCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >::Initialize )
			.def("InitializeSolutionStep", &ConvergenceCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >::InitializeSolutionStep )
			.def("FinalizeSolutionStep", &ConvergenceCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >::FinalizeSolutionStep )
			;                  
				
		class_< TrilinosDisplacementCriteria<TrilinosSparseSpaceType, TrilinosLocalSpaceType >,
			bases< TrilinosConvergenceCriteria >,  
			boost::noncopyable >
			("TrilinosDisplacementCriteria", init< double, double, Epetra_MpiComm& >() );
				
		//********************************************************************
		//********************************************************************
		//Builder and Solver

		class_< TrilinosBuilderAndSolverType::DofsArrayType, boost::noncopyable >("DofsArrayType",	init<>() );
			 
		class_< TrilinosBuilderAndSolverType, boost::noncopyable >
			("TrilinosResidualBasedBuilderAndSolver",init<Epetra_MpiComm&, int, TrilinosLinearSolverType::Pointer >() )
			.def("SetCalculateReactionsFlag", &TrilinosBuilderAndSolverType::SetCalculateReactionsFlag )
			.def("GetCalculateReactionsFlag", &TrilinosBuilderAndSolverType::GetCalculateReactionsFlag )
			.def("SetDofSetIsInitializedFlag", &TrilinosBuilderAndSolverType::SetDofSetIsInitializedFlag )
			.def("GetDofSetIsInitializedFlag", &TrilinosBuilderAndSolverType::GetDofSetIsInitializedFlag )
			.def("SetReshapeMatrixFlag", &TrilinosBuilderAndSolverType::SetReshapeMatrixFlag )
			.def("GetReshapeMatrixFlag", &TrilinosBuilderAndSolverType::GetReshapeMatrixFlag )
			.def("GetEquationSystemSize", &TrilinosBuilderAndSolverType::GetEquationSystemSize )
			.def("BuildLHS", &TrilinosBuilderAndSolverType::BuildLHS )
			.def("BuildRHS", &TrilinosBuilderAndSolverType::BuildRHS )
			.def("Build", &TrilinosBuilderAndSolverType::Build )
			.def("SystemSolve", &TrilinosBuilderAndSolverType::SystemSolve )
			.def("BuildAndSolve", &TrilinosBuilderAndSolverType::BuildAndSolve )
			.def("BuildRHSAndSolve", &TrilinosBuilderAndSolverType::BuildRHSAndSolve )
			.def("ApplyDirichletConditions", &TrilinosBuilderAndSolverType::ApplyDirichletConditions )
			.def("SetUpDofSet", &TrilinosBuilderAndSolverType::SetUpDofSet )
			.def("GetDofSet", &TrilinosBuilderAndSolverType::GetDofSet, return_internal_reference<>() )
			.def("SetUpSystem", &TrilinosBuilderAndSolverType::SetUpSystem )
			.def("ResizeAndInitializeVectors", &TrilinosBuilderAndSolverType::ResizeAndInitializeVectors )
			.def("InitializeSolutionStep", &TrilinosBuilderAndSolverType::InitializeSolutionStep )
			.def("FinalizeSolutionStep", &TrilinosBuilderAndSolverType::FinalizeSolutionStep )
			.def("CalculateReactions", &TrilinosBuilderAndSolverType::CalculateReactions )
			.def("Clear", &TrilinosBuilderAndSolverType::Clear )
			.def("SetEchoLevel", &TrilinosBuilderAndSolverType::SetEchoLevel )
			.def("GetEchoLevel", &TrilinosBuilderAndSolverType::GetEchoLevel )
			;
	
		
	
		//********************************************************************
		//********************************************************************
	
		class_< TrilinosSparseSpaceType, boost::noncopyable >("TrilinosSparseSpace",	init<>() )
		.def("ClearMatrix", ClearMatrix )
		.def("ClearVector", ClearVector )
		.def("ResizeMatrix", ResizeMatrix )
			.def("ResizeVector", ResizeVector )
			.def("SetToZeroMatrix", SetToZeroMatrix )
			.def("SetToZeroVector", SetToZeroVector )
			.def("TwoNorm", TwoNorm )
			//the dot product of two vectors
			.def("Dot", Dot)
			//the matrix-vector multiplication
			.def("Mult", Mult)
	// 		 .def("TransposeMult", TransposeMult)
			.def("Size", Size)
			.def("UnaliasedAdd",UnaliasedAdd)
			.def("ScaleAndAdd",ScaleAndAdd)
			.def("prova",prova)
			.def("CreateEmptyMatrixPointer",CreateEmptyMatrixPointer)
			.def("CreateEmptyVectorPointer",CreateEmptyVectorPointer)
			;

		//********************************************************************
		//********************************************************************
		//strategy base class
// 		class_< TrilinosBaseSolvingStrategyType, boost::noncopyable >("TrilinosSolvingStrategy", init< ModelPart&, bool >() )
// 			.def("Predict", &TrilinosBaseSolvingStrategyType::Predict )
// 			.def("Solve", &TrilinosBaseSolvingStrategyType::Solve )
// 			.def("IsConverged", &TrilinosBaseSolvingStrategyType::IsConverged )
// 			.def("CalculateOutputData", &TrilinosBaseSolvingStrategyType::CalculateOutputData )
// 			.def("SetEchoLevel", &TrilinosBaseSolvingStrategyType::SetEchoLevel )
// 			.def("GetEchoLevel", &TrilinosBaseSolvingStrategyType::GetEchoLevel )
// 			.def("SetRebuildLevel", &TrilinosBaseSolvingStrategyType::SetRebuildLevel )
// 			.def("GetRebuildLevel", &TrilinosBaseSolvingStrategyType::GetRebuildLevel )
// 			.def("SetMoveMeshFlag", &TrilinosBaseSolvingStrategyType::SetMoveMeshFlag )
// 			.def("MoveMeshFlag", &TrilinosBaseSolvingStrategyType::MoveMeshFlag )
// 			.def("MoveMesh", &TrilinosBaseSolvingStrategyType::MoveMesh )
// 			.def("Clear", &TrilinosBaseSolvingStrategyType::Clear )
// 			; 
// 			
// 		class_< ResidualBasedLinearStrategy<  TrilinosSparseSpaceType, TrilinosLocalSpaceType, TrilinosLinearSolverType >,
// 			bases< TrilinosBaseSolvingStrategyType >,  boost::noncopyable >
// 			("TrilinosResidualBasedLinearStrategy", 
// 			init<ModelPart&,TrilinosBaseSchemeType::Pointer, TrilinosLinearSolverType::Pointer, bool, bool, bool, bool	>() )
// 			;
// 
// 		class_< ResidualBasedNewtonRaphsonStrategy<  TrilinosSparseSpaceType, TrilinosLocalSpaceType, TrilinosLinearSolverType >,
// 			bases< TrilinosBaseSolvingStrategyType >,  boost::noncopyable >
// 			("TrilinosResidualBasedNewtonRaphsonStrategy") 
// 			.def(init<ModelPart&, TrilinosBaseSchemeType::Pointer, TrilinosLinearSolverType::Pointer, TrilinosConvergenceCriteria::Pointer, int, bool, bool, bool >() ) 
// 			.def( init<ModelPart&, TrilinosBaseSchemeType::Pointer, TrilinosLinearSolverType::Pointer,TrilinosConvergenceCriteria::Pointer,TrilinosBuilderAndSolverType::Pointer, int, bool, bool, bool >() )
// 			;

	def("CreateCommunicator", CreateCommunicator );
	def("ErrorCleaner", ErrorCleaner);

		//********************************************************************
		//********************************************************************
		class_< Teuchos::ParameterList, boost::noncopyable >("ParameterList",	init<>() )
			.def("set", SetDoubleValue )
			.def("set", SetIntValue )
			.def("set", SetCharValue )
			.def(self_ns::str(self))
		  ;

	typedef AztecSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > AztecSolverType;
	class_<AztecSolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
		( "AztecSolver",
			init<	Teuchos::ParameterList&, std::string, Teuchos::ParameterList& , double , int , int  >() );

	typedef AmesosSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > AmesosSolverType;
	class_<AmesosSolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
		( "AmesosSolver",
			init<const std::string&, Teuchos::ParameterList& >() );

	typedef MultiLevelSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > MLSolverType;
	class_<MLSolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
		( "MultiLevelSolver",
			init<Teuchos::ParameterList&, double, int >() );

}


}  // namespace Python.

}  // namespace Kratos.

#endif // KRATOS_PYTHON defined
