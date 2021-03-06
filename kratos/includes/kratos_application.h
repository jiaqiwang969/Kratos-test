/*
==============================================================================
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi
pooyan@cimne.upc.edu
rrossi@cimne.upc.edu
CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain

Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNER.

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
 
//   

//   Project Name:        Kratos       

//   Last Modified by:    $Author: rrossi $

//   Date:                $Date: 2007-03-06 10:30:33 $

//   Revision:            $Revision: 1.4 $

//

//





#if !defined(KRATOS_KRATOS_APPLICATION_H_INCLUDED )

#define  KRATOS_KRATOS_APPLICATION_H_INCLUDED







// System includes

#include <string>

#include <iostream> 





// External includes 





// Project includes

#include "includes/define.h"

#include "includes/kratos_components.h"

#include "includes/element.h"

#include "includes/condition.h"





namespace Kratos

{



  ///@name Kratos Globals

  ///@{ 

  

  ///@} 

  ///@name Type Definitions

  ///@{ 

  

  ///@} 

  ///@name  Enum's

  ///@{

      

  ///@}

  ///@name  Functions 

  ///@{

      

  ///@}

  ///@name Kratos Classes

  ///@{

  

  /// Short class definition.

  /** Detail class definition.

  */

  class KratosApplication

    {

    public:

      ///@name Type Definitions

      ///@{

      

      /// Pointer definition of KratosApplication

      KRATOS_CLASS_POINTER_DEFINITION(KratosApplication);

  

      ///@}

      ///@name Life Cycle 

      ///@{ 

      

      /// Default constructor.
		KratosApplication();
/*	  KratosApplication() :
			mpVariableData(KratosComponents<VariableData>::pGetComponents()),
			mpElements(KratosComponents<Element>::pGetComponents()), 
			mpConditions(KratosComponents<Condition>::pGetComponents()),
		  	mCondition3D(0, Element::GeometryType::Pointer(new Triangle3D<Node<3> >(Element::GeometryType::PointsArrayType(3, Node<3>())))),
			mCondition2D(0, Element::GeometryType::Pointer(new Geometry<Node<3> >(Element::GeometryType::PointsArrayType(2, Node<3>()))))
	  {}
*/


      /// Copy constructor.

		KratosApplication(KratosApplication const& rOther) : mpVariableData(rOther.mpVariableData),

			mpElements(rOther.mpElements), 

			mpConditions(rOther.mpConditions){}



      /// Destructor.

      virtual ~KratosApplication(){}

      



      ///@}

      ///@name Operators 

      ///@{

      

      

      ///@}

      ///@name Operations

      ///@{



      virtual void Register()

	  {

		  RegisterVariables();

	  }

      
	//ATTENTION: could make problems
       //static void RegisterVariables();
     void RegisterVariables();

      ///@}

      ///@name Access

      ///@{



	//template<class TComponentType>

	//	typename KratosComponents<TComponentType>::ComponentsContainerType& GetComponents()

	//{

	//	return KratosComponents<TComponentType>::GetComponents();

	//}



	

	

	// I have to see why the above version is not working for multi thread ... 

	// Anyway its working with these functions.Pooyan.

		 KratosComponents<VariableData>::ComponentsContainerType& GetVariables()

		{

			return *mpVariableData;

		}



		 KratosComponents<Element>::ComponentsContainerType& GetElements()

		{

			return *mpElements;

		}



		 KratosComponents<Condition>::ComponentsContainerType& GetConditions()

		{

			return *mpConditions;

		}



		void SetComponents(KratosComponents<VariableData>::ComponentsContainerType const& VariableDataComponents)

		{

//			KRATOS_WATCH("!!!!!!!!!!!!!!!!!!!!! SETTING COMPONENETS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")

		  for(KratosComponents<VariableData>::ComponentsContainerType::iterator i = mpVariableData->begin() ;

		  i != mpVariableData->end() ; i++)

//  for(KratosComponents<VariableData>::ComponentsContainerType::iterator i = KratosComponents<VariableData>::GetComponents().begin() ;

//		  i != KratosComponents<VariableData>::GetComponents().end() ; i++)

	  {

		  KratosComponents<VariableData>::ComponentsContainerType::const_iterator i_variable = VariableDataComponents.find(i->second.get().Name());

		  if(i_variable == VariableDataComponents.end()) 

			  KRATOS_ERROR(std::logic_error, "This variable is not registered in Kernel : ",   i_variable->second.get());

		  unsigned int variable_key = i_variable->second.get().Key();

			if(variable_key == 0)

			  KRATOS_ERROR(std::logic_error, "This variable is not initialized in Kernel : ",   i_variable->second.get());

				

//			KRATOS_WATCH(i_variable->second.get());

//			KRATOS_WATCH(i->second.get().Key());

//			KRATOS_WATCH(variable_key);

			i->second.get().SetKey(variable_key);

	  }

//			KRATOS_WATCH("!!!!!!!!!!!!!!!!!!!!! END SETTING COMPONENETS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")

		}




		void SetComponents(KratosComponents<Element>::ComponentsContainerType const& ElementComponents)

		{
			// It's better to make a loop over new components and add them if they are NOT already exist in application. Or make an ERROR for incompatibility between applications.

			mpElements->insert(ElementComponents.begin(), ElementComponents.end());

		}

      

		void SetComponents(KratosComponents<Condition>::ComponentsContainerType const& ConditionComponents)

		{

			mpConditions->insert(ConditionComponents.begin(), ConditionComponents.end());

		}

      

      

      ///@}

      ///@name Inquiry

      ///@{

      

      

      ///@}      

      ///@name Input and output

      ///@{



      /// Turn back information as a string.

      virtual std::string Info() const

      {

	return "KratosApplication";

      }

      

      /// Print information about this object.

      virtual void PrintInfo(std::ostream& rOStream) const

      {

	rOStream << Info();

      }



      /// Print object's data.

      virtual void PrintData(std::ostream& rOStream) const

      {

		rOStream << "Variables:" << std::endl;

		KratosComponents<VariableData>().PrintData(rOStream);

		rOStream << std::endl;

		rOStream << "Elements:" << std::endl;

		KratosComponents<Element>().PrintData(rOStream);

		rOStream << std::endl;

		rOStream << "Conditions:" << std::endl;

		KratosComponents<Condition>().PrintData(rOStream);

      }

      

            

      ///@}      

      ///@name Friends

      ///@{

      

            

      ///@}

      

    protected:

      ///@name Protected static Member Variables 

      ///@{ 

       

      ///@} 

      ///@name Protected member Variables 

      ///@{ 
		const Condition  mCondition3D; 

		const Condition  mCondition2D;        

        

		KratosComponents<VariableData>::ComponentsContainerType* mpVariableData;

		KratosComponents<Element>::ComponentsContainerType* mpElements;

		KratosComponents<Condition>::ComponentsContainerType* mpConditions;

        

        

      ///@} 

      ///@name Protected Operators

      ///@{ 

        

        

      ///@} 

      ///@name Protected Operations

      ///@{ 

        

        

      ///@} 

      ///@name Protected  Access 

      ///@{ 

        

        

      ///@}      

      ///@name Protected Inquiry 

      ///@{ 

        

        

      ///@}    

      ///@name Protected LifeCycle 

      ///@{ 

      

            

      ///@}

      

    private:

      ///@name Static Member Variables 

      ///@{ 

        

        

      ///@} 

      ///@name Member Variables 

      ///@{ 

        

      ///@} 

      ///@name Private Operators

      ///@{ 

        

        

      ///@} 

      ///@name Private Operations

      ///@{ 

        

        

      ///@} 

      ///@name Private  Access 

      ///@{ 

        

        

      ///@}    

      ///@name Private Inquiry 

      ///@{ 

        

        

      ///@}    

      ///@name Un accessible methods 

      ///@{ 

      

      /// Assignment operator.

      KratosApplication& operator=(KratosApplication const& rOther);



        

      ///@}    

        

    }; // Class KratosApplication 



  ///@} 

  

  ///@name Type Definitions       

  ///@{ 

  

  

  ///@} 

  ///@name Input and output 

  ///@{ 

        

 

  /// input stream function

  inline std::istream& operator >> (std::istream& rIStream, 

				    KratosApplication& rThis);



  /// output stream function

  inline std::ostream& operator << (std::ostream& rOStream, 

				    const KratosApplication& rThis)

    {

      rThis.PrintInfo(rOStream);

      rOStream << std::endl;

      rThis.PrintData(rOStream);



      return rOStream;

    }

  ///@} 

  

  

}  // namespace Kratos.



#endif // KRATOS_KRATOS_APPLICATION_H_INCLUDED  defined 





