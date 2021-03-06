//   
//   Project Name:        Kratos       
//   Last modified by:    $Author: anonymous $
//   Date:                $Date: 2008-11-05 17:07:44 $
//   Revision:            $Revision: 1.4 $
//
//

// System includes 

#if defined(KRATOS_PYTHON)
// External includes 
#include <boost/python.hpp>


// Project includes 
#include "includes/define.h"
#include "meshing_application.h"
#include "custom_python/add_meshers_to_python.h"
#include "custom_python/add_processes_to_python.h"
#include "custom_python/add_custom_utilities_to_python.h"


 
namespace Kratos
{

namespace Python
{

  using namespace boost::python;


  
  BOOST_PYTHON_MODULE(KratosMeshingApplication)
  {

	  class_<KratosMeshingApplication, 
			  KratosMeshingApplication::Pointer, 
			  bases<KratosApplication>, boost::noncopyable >("KratosMeshingApplication")
			;
		AddMeshersToPython();
		AddProcessesToPython();
		AddCustomUtilitiesToPython();
		
  }
  
  
}  // namespace Python.
  
}  // namespace Kratos.

#endif // KRATOS_PYTHON defined



