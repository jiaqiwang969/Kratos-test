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
//   Last Modified by:    $Author: pooyan $
//   Date:                $Date: 2008-10-29 14:26:54 $
//   Revision:            $Revision: 1.1 $
//
//


#if !defined(KRATOS_MODEL_PART_IO_H_INCLUDED )
#define  KRATOS_MODEL_PART_IO_H_INCLUDED



// System includes
#include <string>
#include <fstream> 


// External includes 


// Project includes
#include "includes/define.h"
#include "includes/io.h"


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
  
  /// An IO class for reading and writing a modelpart
  /** This class writes all modelpart data including the meshes.
  */
  class ModelPartIO : public IO
  {
  public:
      ///@name Type Definitions
      ///@{
      
      /// Pointer definition of ModelPartIO
      KRATOS_CLASS_POINTER_DEFINITION(ModelPartIO);

      typedef IO BaseType;

      typedef BaseType::NodeType NodeType;
  
      typedef BaseType::MeshType MeshType;

      typedef BaseType::NodesContainerType NodesContainerType;
  
      typedef BaseType::PropertiesContainerType PropertiesContainerType;
  
      typedef BaseType::ElementsContainerType ElementsContainerType;
  
      typedef BaseType::ConditionsContainerType ConditionsContainerType;
  
      typedef BaseType::ConnectivitiesContainerType ConnectivitiesContainerType;
  
      typedef std::vector<std::ofstream*> OutputFilesContainerType;
    
      typedef std::size_t SizeType;
  
      ///@}
      ///@name Life Cycle 
      ///@{ 
      
      /// Constructor with  filenames.
      ModelPartIO(std::string const& Filename) 
	: mNumberOfLines(1)
	, mInputBaseName(Filename), mOutputBaseName(Filename)
	, mInputFilename(Filename + ".mdpa"), mOutputFilename(Filename + ".mdpa")
	, mInput(mInputFilename.c_str()),  mOutput(mOutputFilename.c_str())
      {
	  if(!mInput)
	    KRATOS_ERROR(std::invalid_argument, "Error opening input file : ", mInputFilename.c_str());
	  if(!mOutput)
	    KRATOS_ERROR(std::invalid_argument, "Error opening output file : ", mOutputFilename.c_str());

      }
      
      /// Constructor with input and output filenames.
      ModelPartIO(std::string const& InputFilename, std::string const& OutputFilename) 
	: mNumberOfLines(1)
	, mInputBaseName(InputFilename), mOutputBaseName(OutputFilename)
	, mInputFilename(InputFilename + ".mdpa"), mOutputFilename(OutputFilename + ".mdpa")
	, mInput(mInputFilename.c_str()), mOutput(mOutputFilename.c_str())
      {
	  if(!mInput)
	    KRATOS_ERROR(std::invalid_argument, "Error opening input file : ", mInputFilename.c_str());
	  if(!mOutput)
	    KRATOS_ERROR(std::invalid_argument, "Error opening output file : ", mOutputFilename.c_str());

      }


      /// Constructor with filenames.
//       ModelPartIO(std::string const& InputFilename, std::string const& OutputFilename)
// 	: mNumberOfLines(0), mInput(std::ifstream(InputFilename.c_str())), mOutput(std::ofstream(OutputFilename.c_str()))
//       {
//       }


      /// Destructor.
      virtual ~ModelPartIO(){}
      

      ///@}
      ///@name Operators 
      ///@{
      
      
      ///@}
      ///@name Operations
      ///@{

      virtual bool ReadNode(NodeType& rThisNode)
      {
        KRATOS_ERROR(std::logic_error, "Calling base class member. Please check the definition of derived class.", "")
      }
      
      virtual bool ReadNodes(NodesContainerType& rThisNodes)
      {
	KRATOS_TRY
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "Nodes")
	      ReadNodesBlock(rThisNodes);
	    else 
	      SkipBlock(word);
	  }
	KRATOS_CATCH("")
      }

      virtual void WriteNodes(NodesContainerType const& rThisNodes)
      {
	mOutput << "Begin Nodes" << std::endl;
	for(NodesContainerType::const_iterator i_node = rThisNodes.begin() ; i_node != rThisNodes.end() ; i_node++)
	  mOutput << i_node->Id() << "\t" << i_node->X()  << "\t" << i_node->Y() << "\t" << i_node->Z() << std::endl;
	mOutput << "End Nodes" << std::endl;
      }

      virtual void ReadProperties(Properties& rThisProperties)
      {
        KRATOS_ERROR(std::logic_error, "Calling base class member. Please check the definition of derived class", "")
      }

      virtual void ReadProperties(PropertiesContainerType& rThisProperties)
      {
	KRATOS_TRY
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "Properties")
	      ReadPropertiesBlock(rThisProperties);
	    else 
	      SkipBlock(word);
	  }
	KRATOS_CATCH("")
      }
      
      virtual void WriteProperties(PropertiesContainerType& rThisProperties)
      {
	for(PropertiesContainerType::const_iterator i_properties = rThisProperties.begin() ; i_properties != rThisProperties.end() ; i_properties++)
	  {
	    mOutput << "Begin Properties " << i_properties->Id() << std::endl;
	    i_properties->PrintData(mOutput);
	    mOutput << std::endl;
	    mOutput << "End Properties" << std::endl;
	  }
      }
      
      virtual void ReadElement(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, Element::Pointer& pThisElements)
      {
        KRATOS_ERROR(std::logic_error, "Calling base class member. Please check the definition of derived class", "")
      }

      virtual void ReadElements(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ElementsContainerType& rThisElements)
      {
	KRATOS_TRY
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "Elements")
	      ReadElementsBlock(rThisNodes,rThisProperties,rThisElements);
	    else
	      SkipBlock(word);
	  }
	KRATOS_CATCH("")
      }

      virtual std::size_t  ReadElementsConnectivities(ConnectivitiesContainerType& rElementsConnectivities)
      {
	KRATOS_TRY
	std::size_t number_of_elements = 0;
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "Elements")
	      number_of_elements += ReadElementsConnectivitiesBlock(rElementsConnectivities);
	    else
	      SkipBlock(word);
	  }
	return number_of_elements;

	KRATOS_CATCH("")
      }

      virtual void WriteElements(ElementsContainerType const& rThisElements)
      {
	mOutput << "Begin Elements" << std::endl;
	mOutput << "End Elements" << std::endl;
      }

      virtual void ReadConditions(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ConditionsContainerType& rThisConditions)
      {
	KRATOS_TRY
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "Conditions")
	      ReadConditionsBlock(rThisNodes,rThisProperties,rThisConditions);
	    else
	      SkipBlock(word);
	  }
	KRATOS_CATCH("")
      }

      virtual std::size_t  ReadConditionsConnectivities(ConnectivitiesContainerType& rConditionsConnectivities)
      {
	KRATOS_TRY
	std::size_t number_of_elements = 0;
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "Conditions")
	      number_of_elements += ReadConditionsConnectivitiesBlock(rConditionsConnectivities);
	    else
	      SkipBlock(word);
	  }
	return number_of_elements;
	KRATOS_CATCH("")
      }

      virtual void WriteConditions(ConditionsContainerType const& rThisConditions)
      {
	mOutput << "Begin Conditions" << std::endl;
	mOutput << "End Conditions" << std::endl;
      }

      virtual void ReadInitialValues(NodesContainerType& rThisNodes, ElementsContainerType& rThisElements, ConditionsContainerType& rThisConditions)
      {
	KRATOS_TRY
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "NodalData")
	      ReadNodalDataBlock(rThisNodes);
	    else if(word == "ElementalData")
	      ReadElementalDataBlock(rThisElements);
	    else if(word == "ConditionalData")
	      ReadConditionalDataBlock(rThisConditions);
	    else
	      SkipBlock(word);
	  }
	KRATOS_CATCH("")
       }

//       void ReadGeometries(NodesContainerType& rThisNodes, GeometriesContainerType& rResults);

      virtual void ReadMesh(MeshType & rThisMesh)
      {
        KRATOS_ERROR(std::logic_error, "ModelPartIO does not implement this method.", "")
      }

      virtual void WriteMesh(MeshType & rThisMesh)
      {
	WriteProperties(rThisMesh.Properties());
	WriteNodes(rThisMesh.Nodes());
	WriteElements(rThisMesh.Elements());
	WriteConditions(rThisMesh.Conditions());
      }

      virtual void ReadModelPart(ModelPart & rThisModelPart)
      {
	KRATOS_TRY
	ResetInput();
	std::string word;
	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "ModelPartData")
		 ReadModelPartDataBlock(rThisModelPart);
	    else if(word == "Properties")
	      ReadPropertiesBlock(rThisModelPart.rProperties());
	    else if(word == "Nodes")
	      ReadNodesBlock(rThisModelPart);
	    else if(word == "Elements")
	      ReadElementsBlock(rThisModelPart);
	    else if(word == "Conditions")
	      ReadConditionsBlock(rThisModelPart);
	    else if(word == "NodalData")
	      ReadNodalDataBlock(rThisModelPart.Nodes());
	    else if(word == "ElementalData")
	      ReadElementalDataBlock(rThisModelPart.Elements());
	    else if(word == "ConditionalData")
	      ReadConditionalDataBlock(rThisModelPart.Conditions());
	    
	  }
	std::cout << "lines read : " << mNumberOfLines;
	std::cout << std::endl;
	KRATOS_CATCH("")
      }

      


      virtual void WriteModelPart(ModelPart & rThisModelPart)
      {
	mOutput << "Begin ModelPartData" << std::endl;
	mOutput << "End ModelPartData" << std::endl;
	WriteMesh(rThisModelPart.GetMesh());
      }

    virtual void DivideInputToPartitions(SizeType NumberOfPartitions, 
					 PartitionIndicesContainerType const& NodesPartitions, 
					 PartitionIndicesContainerType const& ElementsPartitions, 
					 PartitionIndicesContainerType const& ConditionsPartitions)
      {
	KRATOS_TRY
	ResetInput();
	std::string word;
	OutputFilesContainerType output_files;
	
	for(SizeType i = 0 ; i < NumberOfPartitions ; i++)
	  {
	    std::stringstream buffer;
	    buffer << mOutputBaseName << "_" << i << ".mdpa";
	    std::ofstream* p_ofstream = new std::ofstream(buffer.str().c_str());
	    if(!(*p_ofstream))
	      KRATOS_ERROR(std::invalid_argument, "Error opening output file : ", buffer.str());

	    output_files.push_back(p_ofstream);  
	  }

	while(true)
	  {
	    ReadWord(word);
	    if(mInput.eof())
	      break;
	    ReadBlockName(word);
	    if(word == "ModelPartData")
		 DivideModelPartDataBlock(output_files);
	    else if(word == "Properties")
	      DividePropertiesBlock(output_files);
	    else if(word == "Nodes")
	      DivideNodesBlock(output_files, NodesPartitions);
	    else if(word == "Elements")
	      DivideElementsBlock(output_files, ElementsPartitions);
	    else if(word == "Conditions")
	      DivideConditionsBlock(output_files, ConditionsPartitions);
	    else if(word == "NodalData")
	      SkipBlock("NodalData");
	    else if(word == "ElementalData")
	      SkipBlock("ElementalData");
	    else if(word == "ConditionalData")
	      SkipBlock("ConditionalData");
	    
	  }
	std::cout << "lines read : " << mNumberOfLines;
	std::cout << std::endl;

	for(SizeType i = 0 ; i < NumberOfPartitions ; i++)
	  delete output_files[i];
	KRATOS_CATCH("")
      }

    void DivideNodalDataBlock(OutputFilesContainerType& OutputFiles, 
			      PartitionIndicesContainerType const& NodesPartitions)
{
      KRATOS_TRY

      std::string word;
      
      WriteInAllFiles(OutputFiles, "Begin NodalData \n");

      SizeType id;
	
      while(!mInput.eof())
      {
	ReadWord(word);
	if(CheckEndBlock("Nodes", word))
	  break;

	ExtractValue(word, id);

	if(id > NodesPartitions.size())
	{
	  std::stringstream buffer;
	  buffer << "Invalid node id : " << id;
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	}

	std::string node_data;
	node_data += word + '\t'; // id
	ReadWord(word);
	node_data += word + '\t'; // x
	ReadWord(word);
	node_data += word + '\t'; // y
	ReadWord(word);
	node_data += word + '\n'; // z

	for(SizeType i = 0 ; i < NodesPartitions[id-1].size() ; i++)
	{
	  SizeType partition_id = NodesPartitions[id-1][i];
	  if(partition_id > OutputFiles.size())
	  {
	    std::stringstream buffer;
	    buffer << "Invalid prtition id : " << partition_id;
	    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }

	  *(OutputFiles[partition_id]) << node_data;
	}
	  
      }

      WriteInAllFiles(OutputFiles, "End NodalData\n");

      KRATOS_CATCH("")
    }

      ///@}
      ///@name Access
      ///@{ 
      
      
      ///@}
      ///@name Inquiry
      ///@{
      
      
      ///@}      
      ///@name Input and output
      ///@{

//       /// Turn back information as a string.
//       virtual std::string Info() const
//  {
//    return "ModelPartIO";
//  }
      
//       /// Print information about this object.
//       virtual void PrintInfo(std::ostream& rOStream) const
//  {
//    rOStream << "ModelPartIO";
//  }
      

//       /// Print object's data.
//       virtual void PrintData(std::ostream& rOStream) const
//  {
//  }
      
            
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

      SizeType mNumberOfLines;
      std::string mInputBaseName;
      std::string mOutputBaseName;
      std::string mInputFilename;
      std::string mOutputFilename;
      std::ifstream mInput;
      std::ofstream mOutput;
        
        
      ///@} 
      ///@name Private Operators
      ///@{ 
        
        
      ///@} 
      ///@name Private Operations
      ///@{

      std::string& ReadBlockName(std::string& rBlockName)
      {
	KRATOS_TRY
	  
	CheckStatement("Begin", rBlockName);
	ReadWord(rBlockName);
	  
	KRATOS_CATCH("")
      }


      std::string& SkipBlock(std::string const& BlockName)
      {
	KRATOS_TRY

	std::string word;
	  
	while(!mInput.eof())
	{
	  ReadWord(word);
	  if(word == "End")
	  {
	    ReadWord(word);
	    if(word == BlockName)
	      break;
	  }
	  
	}

	KRATOS_CATCH("")
      }

      bool CheckEndBlock(std::string const& BlockName, std::string& rWord)
      {
	if(rWord == "End")
	{
	  ReadWord(rWord);
	  CheckStatement(BlockName, rWord);
	  return true;
	}
	
	return false;
      }

      void ReadModelPartDataBlock(ModelPart& rModelPart)
      {
	KRATOS_TRY

	std::string variable_name;
	  
	while(!mInput.eof())
	{
	  ReadWord(variable_name);
	  if(CheckEndBlock("ModelPartData", variable_name))
	    break;
	  if(KratosComponents<Variable<double> >::Has(variable_name))
	    {
	      std::string value;
	      double temp;

	      ReadWord(value); // reading value
	      ExtractValue(value,temp);
	      rModelPart[KratosComponents<Variable<double> >::Get(variable_name)] = temp;
	    }
	  else if(KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
	    {
	      Vector temp_vector; // defining a Vector because for array_1d the operator >> is not defined yet! 
	      ReadVectorialValue(temp_vector);
 	      rModelPart[KratosComponents<Variable<array_1d<double,3> > >::Get(variable_name)] = temp_vector;
	    }
	  else if(KratosComponents<Variable<Matrix> >::Has(variable_name))
	    {
	      ReadVectorialValue(rModelPart[KratosComponents<Variable<Matrix> >::Get(variable_name)]);
	    }
	  else
	  {
	    std::stringstream buffer;
	    buffer << variable_name << " is not a valid variable!!!" << std::endl;
	    buffer << " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }

	  
	}

	KRATOS_CATCH("")
      }

      void ReadNodesBlock(NodesContainerType& rThisNodes)
      {
	KRATOS_TRY

	NodeType temp_node;
	SizeType id;

	std::string word;
	  
	while(!mInput.eof())
	{
	  ReadWord(word);
	  if(CheckEndBlock("Nodes", word))
	    break;

	  ExtractValue(word, temp_node.Id());
	  ReadWord(word);
	  ExtractValue(word, temp_node.X());
	  ReadWord(word);
	  ExtractValue(word, temp_node.Y());
	  ReadWord(word);
	  ExtractValue(word, temp_node.Z());


	  rThisNodes.push_back(temp_node);
	  
	}

	KRATOS_CATCH("")
      }

      void ReadNodesBlock(ModelPart& rModelPart)
      {
	KRATOS_TRY

	SizeType id;
	double x;
	double y;
	double z;
	  
	std::string word;
	
	while(!mInput.eof())
	{
	  ReadWord(word);
	  if(CheckEndBlock("Nodes", word))
	    break;

	  ExtractValue(word, id);
	  ReadWord(word);
	  ExtractValue(word, x);
	  ReadWord(word);
	  ExtractValue(word, y);
	  ReadWord(word);
	  ExtractValue(word, z);

	  rModelPart.CreateNewNode(id,x,y,z);
	}

	KRATOS_CATCH("")
      }

      void ReadPropertiesBlock(PropertiesContainerType& rThisProperties)
      {
	KRATOS_TRY

	SizeType id;
	Properties temp_properties;

	std::string word;
	std::string variable_name;

	ReadWord(word);
	ExtractValue(word, temp_properties.Id());
	  
	while(!mInput.eof())
	{
	  ReadWord(variable_name);
	  if(CheckEndBlock("Properties", variable_name))
	    break;

	  if(KratosComponents<Variable<double> >::Has(variable_name))
	    {
	      std::string value;
	      double temp;

	      ReadWord(value); // reading value
	      ExtractValue(value,temp);
	      temp_properties[KratosComponents<Variable<double> >::Get(variable_name)] = temp;
	    }
	  else if(KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
	    {
	      Vector temp_vector; // defining a Vector because for array_1d the operator >> is not defined yet! 
	      ReadVectorialValue(temp_vector);
 	      temp_properties[KratosComponents<Variable<array_1d<double,3> > >::Get(variable_name)] = temp_vector;
	    }
	  else if(KratosComponents<Variable<Matrix> >::Has(variable_name))
	    {
	      ReadVectorialValue(temp_properties[KratosComponents<Variable<Matrix> >::Get(variable_name)]);
	    }
	  else
	  {
	    std::stringstream buffer;
	    buffer << variable_name << " is not a valid variable!!!" << std::endl;
	    buffer << " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }

	  
	  

	  
	}
	  
      KRATOS_WATCH(temp_properties);
	rThisProperties.push_back(temp_properties);

	KRATOS_CATCH("")
      }

    void ReadElementsBlock(ModelPart& rModelPart)
      {
	ReadElementsBlock(rModelPart.Nodes(), rModelPart.rProperties(), rModelPart.Elements());
      }

    void ReadElementsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ElementsContainerType& rThisElements)
      {
	KRATOS_TRY

	SizeType id;
	SizeType properties_id;
	SizeType node_id;
	

	std::string word;
	std::string element_name;
	  
	ReadWord(element_name);
	if(!KratosComponents<Element>::Has(element_name))
	{
	  std::stringstream buffer;
	  buffer << "Element " << element_name << " is not registered in Kratos.";
	  buffer << " Please check the spelling of the element name and see if the application containing it is registered corectly."; 
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  return;
	}

	Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
	SizeType number_of_nodes = r_clone_element.GetGeometry().size();
	Element::NodesArrayType temp_element_nodes;

	while(!mInput.eof())
	{
	  ReadWord(word); // Reading the element id or End
	  if(CheckEndBlock("Elements", word))
	    break;

	  ExtractValue(word,id);
	  ReadWord(word); // Reading the properties id;
	  ExtractValue(word, properties_id);
	  Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());
	  temp_element_nodes.clear();
	  for(int i = 0 ; i < number_of_nodes ; i++)
	  {
	    ReadWord(word); // Reading the node id;
	    ExtractValue(word, node_id);
	    temp_element_nodes.push_back( *(FindKey(rThisNodes, node_id, "Node").base()));
	  }
	  
	  rThisElements.push_back(r_clone_element.Create(id, temp_element_nodes, p_temp_properties));
	}

	KRATOS_CATCH("")
      }


    void ReadConditionsBlock(ModelPart& rModelPart)
      {
	ReadConditionsBlock(rModelPart.Nodes(), rModelPart.rProperties(), rModelPart.Conditions());
      }

    void ReadConditionsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ConditionsContainerType& rThisConditions)
    {
	KRATOS_TRY

	SizeType id;
	SizeType properties_id;
	SizeType node_id;
	

	std::string word;
	std::string condition_name;
	  
	ReadWord(condition_name);
	if(!KratosComponents<Condition>::Has(condition_name))
	{
	  std::stringstream buffer;
	  buffer << "Condition " << condition_name << " is not registered in Kratos.";
	  buffer << " Please check the spelling of the condition name and see if the application containing it is registered corectly."; 
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  return;
	}

	Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
	SizeType number_of_nodes = r_clone_condition.GetGeometry().size();
	Condition::NodesArrayType temp_condition_nodes;

	while(!mInput.eof())
	{
	  ReadWord(word); // Reading the condition id or End
	  if(CheckEndBlock("Conditions", word))
	    break;

	  ExtractValue(word,id);
	  ReadWord(word); // Reading the properties id;
	  ExtractValue(word, properties_id);
	  Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());
	  temp_condition_nodes.clear();
	  for(int i = 0 ; i < number_of_nodes ; i++)
	  {
	    ReadWord(word); // Reading the node id;
	    ExtractValue(word, node_id);
	    temp_condition_nodes.push_back( *(FindKey(rThisNodes, node_id, "Node").base()));
	  }
	  
	  rThisConditions.push_back(r_clone_condition.Create(id, temp_condition_nodes, p_temp_properties));
	}

	KRATOS_CATCH("")
      }


      void ReadNodalDataBlock(NodesContainerType& rThisNodes)
      {
	KRATOS_TRY

	typedef VariableComponent<VectorComponentAdaptor<array_1d<double, 3> > > array_1d_component_type;

	std::string variable_name;

	ReadWord(variable_name);

	if(KratosComponents<Variable<double> >::Has(variable_name))
	{
	  ReadNodalDofVariableData(rThisNodes, static_cast<Variable<double> const& >(KratosComponents<Variable<double> >::Get(variable_name)));
	}
	else if(KratosComponents<array_1d_component_type>::Has(variable_name))
	{
	  ReadNodalDofVariableData(rThisNodes, static_cast<array_1d_component_type const& >(KratosComponents<array_1d_component_type>::Get(variable_name)));
	}
	else if(KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
	{
	  ReadNodalVectorialVariableData(rThisNodes, static_cast<Variable<array_1d<double, 3> > const& >(KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)), Vector(3));
	}
	else if(KratosComponents<Variable<Matrix> >::Has(variable_name))
	{
	  ReadNodalVectorialVariableData(rThisNodes, static_cast<Variable<Matrix > const& >(KratosComponents<Variable<Matrix> >::Get(variable_name)), Matrix(3,3));
	}
	else
	{
	  std::stringstream buffer;
	  buffer << variable_name << " is not a valid variable!!!" << std::endl;
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	}

	KRATOS_CATCH("")
      }

    template<class TVariableType> 
    void ReadNodalDofVariableData(NodesContainerType& rThisNodes, TVariableType& rVariable)
    {
	KRATOS_TRY

	SizeType id;
	bool is_fixed;
	double nodal_value;

	std::string value;
	
	while(!mInput.eof())
	{
	  ReadWord(value); // reading id
	  if(CheckEndBlock("NodalData", value))
	    break;

	  ExtractValue(value, id);
	  typename NodesContainerType::iterator i_node = FindKey(rThisNodes, id, "Node");

	  // readidng is_fixed
	  ReadWord(value); 
	  ExtractValue(value, is_fixed);
	  if(is_fixed)
	    i_node->Fix(rVariable);

	  // readidng nodal_value
	  ReadWord(value); 
	  ExtractValue(value, nodal_value);

	  i_node->GetSolutionStepValue(rVariable, 0) =  nodal_value;

	  KRATOS_WATCH(*i_node);
	}

	KRATOS_CATCH("")
    }


    template<class TVariableType> 
    void ReadNodalScalarVariableData(NodesContainerType& rThisNodes, TVariableType& rVariable)
    {
	KRATOS_TRY

	SizeType id;
	bool is_fixed;
	typename TVariableType::Type nodal_value;

	std::string value;
	
	while(!mInput.eof())
	{
	  ReadWord(value); // reading id
	  if(CheckEndBlock("NodalData", value))
	    break;

	  ExtractValue(value, id);

	  // readidng is_fixed
	  ReadWord(value);
	  ExtractValue(value, is_fixed);
	  if(is_fixed)
	  {
	    std::stringstream buffer;
	    buffer << "Only double variables or components can be fixed.";
	    buffer <<  " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }
	    
	  

	  // readidng nodal_value
	  ReadWord(value); 
	  ExtractValue(value, nodal_value);

	  FindKey(rThisNodes, id, "Node")->GetSolutionStepValue(rVariable, 0) =  nodal_value;
	}

	KRATOS_CATCH("")
    }



    template<class TVariableType, class TDataType> 
    void ReadNodalVectorialVariableData(NodesContainerType& rThisNodes, TVariableType& rVariable, TDataType Dummy)
    {
	KRATOS_TRY

	SizeType id;
	bool is_fixed;
	TDataType nodal_value;

	std::string value;
	
	while(!mInput.eof())
	{
	  ReadWord(value); // reading id
	  if(CheckEndBlock("NodalData", value))
	    break;

	  ExtractValue(value, id);

	  // readidng is_fixed
	  ReadWord(value);
	  ExtractValue(value, is_fixed);
	  if(is_fixed)
	  {
	    std::stringstream buffer;
	    buffer << "Only double variables or components can be fixed.";
	    buffer <<  " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }
	    
	  

	  // readidng nodal_value
	  ReadVectorialValue(nodal_value);

	  FindKey(rThisNodes, id, "Node")->GetSolutionStepValue(rVariable, 0) =  nodal_value;	}

	KRATOS_CATCH("")
    }

      void ReadElementalDataBlock(ElementsContainerType& rThisElements)
      {
	KRATOS_TRY

	typedef VariableComponent<VectorComponentAdaptor<array_1d<double, 3> > > array_1d_component_type;

	std::string variable_name;

	ReadWord(variable_name);

	if(KratosComponents<Variable<double> >::Has(variable_name))
	{
	  ReadElementalScalarVariableData(rThisElements, static_cast<Variable<double> const& >(KratosComponents<Variable<double> >::Get(variable_name)));
	}
	else if(KratosComponents<array_1d_component_type>::Has(variable_name))
	{
	  ReadElementalScalarVariableData(rThisElements, static_cast<array_1d_component_type const& >(KratosComponents<array_1d_component_type>::Get(variable_name)));
	}
	else if(KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
	{
	  ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<array_1d<double, 3> > const& >(KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)), Vector(3));
	}
	else if(KratosComponents<Variable<Matrix> >::Has(variable_name))
	{
	  ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<Matrix > const& >(KratosComponents<Variable<Matrix> >::Get(variable_name)), Matrix(3,3));
	}
	else
	{
	  std::stringstream buffer;
	  buffer << variable_name << " is not a valid variable!!!" << std::endl;
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	}

	KRATOS_CATCH("")
      }

    template<class TVariableType> 
    void ReadElementalScalarVariableData(ElementsContainerType& rThisElements, TVariableType& rVariable)
    {
	KRATOS_TRY

	SizeType id;
	bool is_fixed;
	double elemental_value;

	std::string value;
	
	while(!mInput.eof())
	{
	  ReadWord(value); // reading id
	  if(CheckEndBlock("ElementalData", value))
	    break;

	  ExtractValue(value, id);

	  // readidng nodal_value
	  ReadWord(value); 
	  ExtractValue(value, elemental_value);

	  FindKey(rThisElements, id, "Element")->GetValue(rVariable) =  elemental_value;
	}

	KRATOS_CATCH("")
    }


    template<class TVariableType, class TDataType> 
    void ReadElementalVectorialVariableData(ElementsContainerType& rThisElements, TVariableType& rVariable, TDataType Dummy)
    {
	KRATOS_TRY

	SizeType id;
	bool is_fixed;
	TDataType elemental_value;

	std::string value;
	
	while(!mInput.eof())
	{
	  ReadWord(value); // reading id
	  if(CheckEndBlock("ElementalData", value))
	    break;

	  ExtractValue(value, id);

	  

	  // readidng nodal_value
	  ReadVectorialValue(elemental_value);
	  ExtractValue(value, elemental_value);

	  FindKey(rThisElements, id, "Element")->GetValue(rVariable) =  elemental_value;
	}

	KRATOS_CATCH("")
    }
      void ReadConditionalDataBlock(ConditionsContainerType& rThisConditions)
      {
	KRATOS_TRY

	typedef VariableComponent<VectorComponentAdaptor<array_1d<double, 3> > > array_1d_component_type;

	std::string variable_name;

	ReadWord(variable_name);

	if(KratosComponents<Variable<double> >::Has(variable_name))
	{
	  ReadConditionalScalarVariableData(rThisConditions, static_cast<Variable<double> const& >(KratosComponents<Variable<double> >::Get(variable_name)));
	}
	else if(KratosComponents<array_1d_component_type>::Has(variable_name))
	{
	  ReadConditionalScalarVariableData(rThisConditions, static_cast<array_1d_component_type const& >(KratosComponents<array_1d_component_type>::Get(variable_name)));
	}
	else if(KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
	{
	  ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<array_1d<double, 3> > const& >(KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)), Vector(3));
	}
	else if(KratosComponents<Variable<Matrix> >::Has(variable_name))
	{
	  ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<Matrix > const& >(KratosComponents<Variable<Matrix> >::Get(variable_name)), Matrix(3,3));
	}
	else
	{
	  std::stringstream buffer;
	  buffer << variable_name << " is not a valid variable!!!" << std::endl;
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	}

	KRATOS_CATCH("")
      }

    template<class TVariableType> 
    void ReadConditionalScalarVariableData(ConditionsContainerType& rThisConditions, TVariableType& rVariable)
    {
	KRATOS_TRY

	SizeType id;
	bool is_fixed;
	double conditional_value;

	std::string value;
	
	while(!mInput.eof())
	{
	  ReadWord(value); // reading id
	  if(CheckEndBlock("ConditionalData", value))
	    break;

	  ExtractValue(value, id);

	  // readidng nodal_value
	  ReadWord(value); 
	  ExtractValue(value, conditional_value);

	  FindKey(rThisConditions, id, "Condition")->GetValue(rVariable) =  conditional_value;
	}

	KRATOS_CATCH("")
    }


    template<class TVariableType, class TDataType> 
    void ReadConditionalVectorialVariableData(ConditionsContainerType& rThisConditions, TVariableType& rVariable, TDataType Dummy)
    {
	KRATOS_TRY

	SizeType id;
	bool is_fixed;
	TDataType conditional_value;

	std::string value;
	
	while(!mInput.eof())
	{
	  ReadWord(value); // reading id
	  if(CheckEndBlock("ConditionalData", value))
	    break;

	  ExtractValue(value, id);

	  

	  // readidng nodal_value
	  ReadVectorialValue(conditional_value);
	  ExtractValue(value, conditional_value);

	  FindKey(rThisConditions, id, "Condition")->GetValue(rVariable) =  conditional_value;
	}

	KRATOS_CATCH("")
    }


    SizeType ReadElementsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities)
      {
	KRATOS_TRY

	SizeType id;
	SizeType properties_id;
	SizeType node_id;
	SizeType number_of_connectivities = 0;
	

	std::string word;
	std::string element_name;
	  
	ReadWord(element_name);
	if(!KratosComponents<Element>::Has(element_name))
	{
	  std::stringstream buffer;
	  buffer << "Element " << element_name << " is not registered in Kratos.";
	  buffer << " Please check the spelling of the element name and see if the application containing it is registered corectly."; 
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  return number_of_connectivities;
	}

	Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
	SizeType number_of_nodes = r_clone_element.GetGeometry().size();
	ConnectivitiesContainerType::value_type temp_element_nodes;

	while(!mInput.eof())
	{
	  ReadWord(word); // Reading the element id or End
	  if(CheckEndBlock("Elements", word))
	    break;

	  ExtractValue(word,id);
	  ReadWord(word); // Reading the properties id;
	  temp_element_nodes.clear();
	  for(int i = 0 ; i < number_of_nodes ; i++)
	  {
	    ReadWord(word); // Reading the node id;
	    ExtractValue(word, node_id);
	    temp_element_nodes.push_back(node_id);
	  }
	  
	  rThisConnectivities.push_back(temp_element_nodes);
	  number_of_connectivities++;
	}

	return number_of_connectivities;

	KRATOS_CATCH("")
      }


    SizeType ReadConditionsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities)
      {
	KRATOS_TRY

	SizeType id;
	SizeType properties_id;
	SizeType node_id;
	SizeType number_of_connectivities = 0;
	

	std::string word;
	std::string condition_name;
	  
	ReadWord(condition_name);
	if(!KratosComponents<Condition>::Has(condition_name))
	{
	  std::stringstream buffer;
	  buffer << "Condition " << condition_name << " is not registered in Kratos.";
	  buffer << " Please check the spelling of the condition name and see if the application containing it is registered corectly."; 
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  return number_of_connectivities;
	}

	Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
	SizeType number_of_nodes = r_clone_condition.GetGeometry().size();
	ConnectivitiesContainerType::value_type temp_condition_nodes;

	while(!mInput.eof())
	{
	  ReadWord(word); // Reading the condition id or End
	  if(CheckEndBlock("Conditions", word))
	    break;

	  ExtractValue(word,id);
	  ReadWord(word); // Reading the properties id;
	  temp_condition_nodes.clear();
	  for(int i = 0 ; i < number_of_nodes ; i++)
	  {
	    ReadWord(word); // Reading the node id;
	    ExtractValue(word, node_id);
	    temp_condition_nodes.push_back(node_id);
	  }
	  
	  rThisConnectivities.push_back(temp_condition_nodes);
	  number_of_connectivities++;
	}

	return number_of_connectivities;

	KRATOS_CATCH("")
      }



    void DivideModelPartDataBlock(OutputFilesContainerType& OutputFiles)
    {
      KRATOS_TRY
      std::string block;
      
      WriteInAllFiles(OutputFiles, "Begin ModelPartData");

      ReadBlock(block, "ModelPartData");
      WriteInAllFiles(OutputFiles, block);

      WriteInAllFiles(OutputFiles, "End ModelPartData\n");
      KRATOS_CATCH("")
    }

    void DividePropertiesBlock(OutputFilesContainerType& OutputFiles)
    {
      KRATOS_TRY

      std::string block;
      
      WriteInAllFiles(OutputFiles, "Begin Properties ");

      ReadBlock(block, "Properties");
      WriteInAllFiles(OutputFiles, block);

      WriteInAllFiles(OutputFiles, "End Properties\n");
      KRATOS_CATCH("")
    }

    void DivideNodesBlock(OutputFilesContainerType& OutputFiles, 
			      PartitionIndicesContainerType const& NodesPartitions)
{
      KRATOS_TRY

      std::string word;
      
      WriteInAllFiles(OutputFiles, "Begin Nodes \n");

      SizeType id;
	
      while(!mInput.eof())
      {
	ReadWord(word);
	if(CheckEndBlock("Nodes", word))
	  break;

	ExtractValue(word, id);

	if(id > NodesPartitions.size())
	{
	  std::stringstream buffer;
	  buffer << "Invalid node id : " << id;
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	}

	std::string node_data;
	node_data += word + '\t'; // id
	ReadWord(word);
	node_data += word + '\t'; // x
	ReadWord(word);
	node_data += word + '\t'; // y
	ReadWord(word);
	node_data += word + '\n'; // z

	for(SizeType i = 0 ; i < NodesPartitions[id-1].size() ; i++)
	{
	  SizeType partition_id = NodesPartitions[id-1][i];
	  if(partition_id > OutputFiles.size())
	  {
	    std::stringstream buffer;
	    buffer << "Invalid prtition id : " << partition_id;
	    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }

	  *(OutputFiles[partition_id]) << node_data;
	}
	  
      }

      WriteInAllFiles(OutputFiles, "End Nodes\n");

      KRATOS_CATCH("")
    }

    void DivideElementsBlock(OutputFilesContainerType& OutputFiles, 
			      PartitionIndicesContainerType const& ElementsPartitions)
      {
      KRATOS_TRY

      std::string word;
      std::string element_name;
	  
      ReadWord(element_name);
      if(!KratosComponents<Element>::Has(element_name))
	{
	  std::stringstream buffer;
	  buffer << "Element " << element_name << " is not registered in Kratos.";
	  buffer << " Please check the spelling of the element name and see if the application containing it is registered corectly."; 
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  return;
	}
      
      Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
      SizeType number_of_nodes = r_clone_element.GetGeometry().size();

      WriteInAllFiles(OutputFiles, "Begin Elements " +  element_name);

      SizeType id;
	
      while(!mInput.eof())
      {
	  ReadWord(word); // Reading the element id or End
	  if(CheckEndBlock("Elements", word))
	    break;

	  ExtractValue(word,id);
	  if(id > ElementsPartitions.size())
	    {
	      std::stringstream buffer;
	      buffer << "Invalid element id : " << id;
	      buffer << " [Line " << mNumberOfLines << " ]";
	      KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	    }

	  std::string element_data;
	  element_data += '\n' + word + '\t'; // id
	  ReadWord(word); // Reading the properties id;
	  element_data += word + '\t'; // properties id

	  for(int i = 0 ; i < number_of_nodes ; i++)
	  {
	    ReadWord(word); // Reading the node id;
	    element_data += word + '\t'; // node id
	  }
	  

	for(SizeType i = 0 ; i < ElementsPartitions[id-1].size() ; i++)
	{
	  SizeType partition_id = ElementsPartitions[id-1][i];
	  if(partition_id > OutputFiles.size())
	  {
	    std::stringstream buffer;
	    buffer << "Invalid prtition id : " << partition_id;
	    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }

	  *(OutputFiles[partition_id]) << element_data;
	}
	  
      }

      WriteInAllFiles(OutputFiles, "\nEnd Elements\n");

      KRATOS_CATCH("")
    }



    void DivideConditionsBlock(OutputFilesContainerType& OutputFiles, 
			      PartitionIndicesContainerType const& ConditionsPartitions)
    {
      KRATOS_TRY

      std::string word;
      std::string condition_name;
	  
      ReadWord(condition_name);
      if(!KratosComponents<Condition>::Has(condition_name))
	{
	  std::stringstream buffer;
	  buffer << "Condition " << condition_name << " is not registered in Kratos.";
	  buffer << " Please check the spelling of the condition name and see if the application containing it is registered corectly."; 
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  return;
	}
      
      Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
      SizeType number_of_nodes = r_clone_condition.GetGeometry().size();

      WriteInAllFiles(OutputFiles, "Begin Conditions " +  condition_name);

      SizeType id;
	
      while(!mInput.eof())
      {
	  ReadWord(word); // Reading the condition id or End
	  if(CheckEndBlock("Conditions", word))
	    break;

	  ExtractValue(word,id);
	  if(id > ConditionsPartitions.size())
	    {
	      std::stringstream buffer;
	      buffer << "Invalid condition id : " << id;
	      buffer << " [Line " << mNumberOfLines << " ]";
	      KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	    }

	  std::string condition_data;
	  condition_data += '\n' + word + '\t'; // id
	  ReadWord(word); // Reading the properties id;
	  condition_data += word + '\t'; // properties id

	  for(int i = 0 ; i < number_of_nodes ; i++)
	  {
	    ReadWord(word); // Reading the node id;
	    condition_data += word + '\t'; // node id
	  }
	  

	for(SizeType i = 0 ; i < ConditionsPartitions[id-1].size() ; i++)
	{
	  SizeType partition_id = ConditionsPartitions[id-1][i];
	  if(partition_id > OutputFiles.size())
	  {
	    std::stringstream buffer;
	    buffer << "Invalid prtition id : " << partition_id;
	    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
	    KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	  }

	  *(OutputFiles[partition_id]) << condition_data;
	}
	  
      }

      WriteInAllFiles(OutputFiles, "\nEnd Conditions\n");

      KRATOS_CATCH("")
    }


    void WriteInAllFiles(OutputFilesContainerType& OutputFiles, std::string const& ThisWord)
    {
      for(SizeType i = 0 ; i < OutputFiles.size() ; i++)
	*(OutputFiles[i]) << ThisWord;
    }


    template<class TContainerType, class TKeyType> 
    typename TContainerType::iterator FindKey(TContainerType& ThisContainer , TKeyType ThisKey, std::string ComponentName)
    {
      typename TContainerType::iterator i_result;
      if((i_result = ThisContainer.find(ThisKey)) == ThisContainer.end())
	{
	  std::stringstream buffer;
	  buffer << ComponentName << " #" << ThisKey << " is not found.";
	  buffer << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	}
	    
      return i_result;
    }
	  

	  

    // Basically it startes to read the character sequence until reaching a
    // "(" and then goes until corresponding ")" which means the vector or
    // matrix value is completely read. It can be used to read any kind of
    // vector or matrix with operator >> defined and writtern in following
    // format: <some definition> "(" ... ")"
   template<class TValueType>
    TValueType& ReadVectorialValue(TValueType& rValue)
    {
      std::stringstream value;

      char c = SkipWhiteSpaces();
      while((c != '(') && !mInput.eof())
	{
	  value << c;
	  c = GetCharacter();
	}
      int open_parantesis = 1;
      while((open_parantesis != 0) && !mInput.eof())
	{
	  value << c;
	  c = GetCharacter();
	  if(c == '(') 
	    open_parantesis++;
	  if(c == ')') 
	    open_parantesis--;
	}
      value << c; // adding the final parantesis
      
      value >>  rValue;
    }

      template<class TValueType>
	TValueType& ExtractValue(std::string rWord, TValueType & rValue)
	{
	  std::stringstream value(rWord);

	  value >> rValue;

	  return rValue;
	}

        
      ModelPartIO& ReadWord(std::string& Word)
	{
	  Word.clear();

	  char c = SkipWhiteSpaces();
	  while(!mInput.eof() && !IsWhiteSpace(c))
	  {
	    Word += c;
	    c = GetCharacter();
	  }

	  return *this;
	}

    ModelPartIO& ReadBlock(std::string& Block, std::string const& BlockName)
	{
	  Block.clear();

	  char c = GetCharacter();
	  std::string word;

	  while(!mInput.eof())
	  {
	    if(c == 'E')
	    {
	      word.clear();
	      while(!mInput.eof() && !IsWhiteSpace(c))
	      {
		word += c;
		c = GetCharacter();
	      }
	      if(CheckEndBlock(BlockName, word))
		break;
	      Block += word;
	    }
	      
	    Block += c;
	    
	    c = GetCharacter();
	  }

	  return *this;
	}


      char SkipWhiteSpaces()
      {
	char c = GetCharacter();
	while(IsWhiteSpace(c))
	  c = GetCharacter();
	return c;
      }

      bool IsWhiteSpace(char C)
      {
	return ((C == ' ') || (C == '\t') || (C == '\r') || (C == '\n'));
      }

      char GetCharacter() //Getting the next character skipping comments
      {
	char c;
	if(mInput.get(c))
	{
	  if(c == '\n')
	    mNumberOfLines++;
	  else if(c == '/') // it may be a comment!
	  {
	    char next_c = mInput.peek();
	    if(next_c == '/') // it's a line comment
	    {
	      while((mInput.get(c)) && (c != '\n')); // so going to the end of line
	      if(!mInput.eof())
		mNumberOfLines++;
	    }
	    else if(next_c == '*') // it's a block comment
	    {
	      while((mInput.get(c)) && !((c == '*') && (mInput.peek() == '/'))) // so going to the end of block
		if(c == '\n')
		  mNumberOfLines++;
	      mInput.get(c);
 	      c = GetCharacter(); // read a new character after comment 
	    }
	  }
	}
	else
	  c = 0;

	return c;
	      
      }

      bool CheckStatement(std::string const& rStatement, std::string const& rGivenWord)
      {
	bool result = false;
	if(rGivenWord != rStatement)
	{
	  std::stringstream buffer;
	  buffer << "A \"" << rStatement << "\" statement was expected but the given statement was \"";
	  buffer <<  rGivenWord << "\"" << " [Line " << mNumberOfLines << " ]";
	  KRATOS_ERROR(std::invalid_argument, buffer.str(), "");
	}
	else
	  result = true;
	
	return result;
      }

      void ResetInput()
      {
 	mInput.clear(); 
	mInput.seekg(0, std::ios_base::beg);
	mNumberOfLines = 1;
      }


      
        
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
      ModelPartIO& operator=(ModelPartIO const& rOther);

      /// Copy constructor.
      ModelPartIO(ModelPartIO const& rOther);

        
      ///@}    
        
    }; // Class ModelPartIO 

  ///@} 
  
  ///@name Type Definitions       
  ///@{ 
  
  
  ///@} 
  ///@name Input and output 
  ///@{ 
        
 
//   /// input stream function
//   inline std::istream& operator >> (std::istream& rIStream, 
//                  ModelPartIO& rThis);

//   /// output stream function
//   inline std::ostream& operator << (std::ostream& rOStream, 
//                  const ModelPartIO& rThis)
//     {
//       rThis.PrintInfo(rOStream);
//       rOStream << std::endl;
//       rThis.PrintData(rOStream);

//       return rOStream;
//     }
//   ///@} 
  
  
}  // namespace Kratos.

#endif // KRATOS_MODEL_PART_IO_H_INCLUDED  defined 
