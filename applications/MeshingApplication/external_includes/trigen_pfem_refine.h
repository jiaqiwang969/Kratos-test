//   
//   Project Name:        Kratos       
//   Last Modified by:    $Author: rrossi $
//   Date:                $Date: 2009-01-22 17:13:57 $
//   Revision:            $Revision: 1.5 $
//
//


#if !defined(KRATOS_TRIGEN_PFEM_MODELER_H_INCLUDED )
#define  KRATOS_TRIGEN_PFEM_MODELER_H_INCLUDED

 

// System includes
#include <string>
#include <iostream> 
#include <stdlib.h>

#if !defined(KRATOS_TRIANGLE_EXTERNAL_H_INCLUDED)
#define  KRATOS_TRIANGLE_EXTERNAL_H_INCLUDED
#include "triangle.h" 
#endif

#include <boost/timer.hpp>

// Project includes
#include "includes/define.h"
#include "includes/model_part.h"
#include "geometries/triangle_2d_3.h"
#include "meshing_application.h"
#include "processes/node_erase_process.h" 
#include "spatial_containers/spatial_containers.h"



namespace Kratos
{
	extern "C" {
		void triangulate(char *, struct triangulateio *, struct triangulateio *,struct triangulateio *);
		//void trifree();
	}


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
	class TriGenPFEMModeler  
	{
	public:
		///@name Type Definitions
		///@{

		/// Pointer definition of TriGenModeler
		KRATOS_CLASS_POINTER_DEFINITION(TriGenPFEMModeler);

		///@}
		///@name Life Cycle 
		///@{ 

		/// Default constructor.
		TriGenPFEMModeler() :
		    mJ(ZeroMatrix(2,2)), //local jacobian
		    mJinv(ZeroMatrix(2,2)), //inverse jacobian
		    mC(ZeroVector(2)), //dimension = number of nodes
		    mRhs(ZeroVector(2)){}
		    //mpNodeEraseProcess(NULL){} //dimension = number of nodes

		/// Destructor.
		virtual ~TriGenPFEMModeler(){}


		///@}
		///@name Operators 
		///@{


		///@}
		///@name Operations
		///@{


		//*******************************************************************************************
		//*******************************************************************************************
		void ReGenerateMesh(
			ModelPart& ThisModelPart , 
			Element const& rReferenceElement, 
			Condition const& rReferenceBoundaryCondition,
			NodeEraseProcess& node_erase,
			double my_alpha = 1.4)
		{

			KRATOS_TRY
			if (ThisModelPart.NodesBegin()->SolutionStepsDataHas(IS_FREE_SURFACE)==false )
				KRATOS_ERROR(std::logic_error,"Add  ----IS_FREE_SURFACE---- variable!!!!!! ERROR","");
			if (ThisModelPart.NodesBegin()->SolutionStepsDataHas(IS_STRUCTURE)==false )
				KRATOS_ERROR(std::logic_error,"Add  ----IS_STRUCTURE---- variable!!!!!! ERROR","");
			if (ThisModelPart.NodesBegin()->SolutionStepsDataHas(IS_BOUNDARY)==false )
				KRATOS_ERROR(std::logic_error,"Add  ----IS_BOUNDARY---- variable!!!!!! ERROR","");
			if (ThisModelPart.NodesBegin()->SolutionStepsDataHas(IS_FLUID)==false )
				KRATOS_ERROR(std::logic_error,"Add  ----IS_FLUID---- variable!!!!!! ERROR","");

			KRATOS_WATCH("Trigen PFEM Refining Mesher")
			boost::timer auxiliary;
			//clearing elements
			ThisModelPart.Elements().clear();
			ThisModelPart.Conditions().clear();
			//creating the containers for the input and output
			struct triangulateio in;
			struct triangulateio out;
			struct triangulateio vorout;

			initialize_triangulateio(in);
			initialize_triangulateio(out);
			initialize_triangulateio(vorout);
			
			//assigning the size of the input containers
						
			in.numberofpoints = ThisModelPart.Nodes().size();
			in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));

			//reorder the id's and give the coordinates to the mesher
			ModelPart::NodesContainerType::iterator nodes_begin = ThisModelPart.NodesBegin();
			for(unsigned int i = 0; i<ThisModelPart.Nodes().size(); i++)
			{
				int base = i*2;
				//int base = ((nodes_begin + i)->Id()   -  1 ) * 2;

				//from now on it is consecutive
				(nodes_begin + i)->Id() = i+1;

				in.pointlist[base] = (nodes_begin + i)->X();
				in.pointlist[base+1] = (nodes_begin + i)->Y();
			}
			//in.numberoftriangles = ThisModelPart.Elements().size();
			//in.trianglelist = (int*) malloc(in.numberoftriangles * 3 * sizeof(int));

			// "P" suppresses the output .poly file. Saves disk space, but you 				
			//lose the ability to maintain constraining segments  on later refinements of the mesh. 
			// "B" Suppresses boundary markers in the output .node, .poly, and .edge output files
			// "n" outputs a list of triangles neighboring each triangle.
			// "e" outputs edge list (i.e. all the "connectivities")
			//char options[] = "PBn";
			char options[] = "Pne";
			triangulate(options, &in, &out, &vorout);
			//print out the mesh generation time
			std::cout<<"mesh generation time = "<<auxiliary.elapsed();
			//number of newly generated triangles
			int el_number=out.numberoftriangles;
			//prepairing for alpha shape passing : creating necessary arrays
			//list of preserved elements is created: at max el_number can be preserved (all elements)
			std::vector<int> preserved_list(el_number);
			array_1d<double,3> x1,x2,x3,xc;
			int number_of_preserved_elems=0;
			int point_base;
			//loop for passing alpha shape			
			for(int el = 0; el< el_number; el++)
			{
				int base = el * 3;

				//coordinates
				point_base = (out.trianglelist[base] - 1)*2;
				x1[0] = out.pointlist[point_base]; 
				x1[1] = out.pointlist[point_base+1]; 

				point_base = (out.trianglelist[base+1] - 1)*2;
				x2[0] = out.pointlist[point_base]; 
				x2[1] = out.pointlist[point_base+1]; 

				point_base = (out.trianglelist[base+2] - 1)*2;
				x3[0] = out.pointlist[point_base]; 
				x3[1] = out.pointlist[point_base+1]; 

				//here we shall temporarily save the elements and pass them afterwards to the alpha shape
				Geometry<Node<3> > temp;
				temp.push_back( *((ThisModelPart.Nodes()).find( out.trianglelist[base]).base()	) );
				temp.push_back( *((ThisModelPart.Nodes()).find( out.trianglelist[base+1]).base()	) );
				temp.push_back( *((ThisModelPart.Nodes()).find( out.trianglelist[base+2]).base()	) );

				int number_of_structure_nodes = int( temp[0].FastGetSolutionStepValue(IS_STRUCTURE) );
				number_of_structure_nodes += int( temp[1].FastGetSolutionStepValue(IS_STRUCTURE) );
				number_of_structure_nodes += int( temp[2].FastGetSolutionStepValue(IS_STRUCTURE) );

				//check the number of nodes of boundary
				int nfs = int( temp[0].FastGetSolutionStepValue(IS_FREE_SURFACE) );
				nfs += int( temp[1].FastGetSolutionStepValue(IS_FREE_SURFACE) );
				nfs += int( temp[2].FastGetSolutionStepValue(IS_FREE_SURFACE) );
				
				//check the number of nodes of boundary
				int nfluid = int( temp[0].FastGetSolutionStepValue(IS_FLUID) );
				nfluid += int( temp[1].FastGetSolutionStepValue(IS_FLUID) );
				nfluid += int( temp[2].FastGetSolutionStepValue(IS_FLUID) );
				//first check that we are working with fluid elements, otherwise throw an error
				//if (nfluid!=3)
				//	KRATOS_ERROR(std::logic_error,"THATS NOT FLUID or NOT TRIANGLE!!!!!! ERROR","");
				//otherwise perform alpha shape check

				
				if(number_of_structure_nodes!=3) //if it is = 3 it is a completely fixed element -> do not add it
				{
					if (nfs != 0 || nfluid != 3)  //in this case it is close to the surface so i should use alpha shape 
					{
						
						if( AlphaShape(my_alpha,temp) && number_of_structure_nodes!=3) //if alpha shape says to preserve
						{
							preserved_list[el] = true;
							number_of_preserved_elems += 1;
														
						}
					}
					else //internal triangle --- should be ALWAYS preserved
					{							
						double bigger_alpha = my_alpha*10.0;
						if( AlphaShape(bigger_alpha,temp) && number_of_structure_nodes!=3) 
							{
							preserved_list[el] = true;
							number_of_preserved_elems += 1;							
							}
					}				
				}
				else 
					preserved_list[el] = false;
			}
			//freeing memory 

			////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////                 END OF THE FIRST PHASE -FREE SURF IDENT            ////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////


			///////////////////////////////////////////////////////////////////////////////////////////////////
			//												///
			//				REMOVING THE CLOSE NODES					///
			///////////////////////////////////////////////////////////////////////////////////////////////////
			//save the wish size for all the nodes
			std::vector< double> h_sizes(in.numberofpoints);
			std::vector< int> preserved(in.numberofpoints);

			for(int i = 0; i<in.numberofpoints; i++)
			{
				preserved[i] = true;
				h_sizes[i] = (nodes_begin+i)->FastGetSolutionStepValue(NODAL_H);
			}
			
			KRATOS_WATCH( "ffff ");
			//now we shall erase the nodes that are closer than the prescribed distance (NODAL_H)
			for(int edge_it = 0; edge_it < out.numberofedges; edge_it++) //looping over all edges 
 			{
					int base = edge_it * 2;
 					
 					int index1 = out.edgelist[base]  - 1;
 					int index2 = out.edgelist[base+1] -1;
 					
					//if one of the two verteces is marked for removal don't do anything more
 					if( preserved[ index1  ] == true && preserved[ index2  ] == true ) 
 					{
 						double dx = out.pointlist[index1*2]-out.pointlist[index2*2];
 						double dy = out.pointlist[index1*2+1]-out.pointlist[index2*2+1];
 						double edge_lenght2 = dx*dx + dy*dy;
 						
 						double& h1 = h_sizes[index1];
 						double& h2 = h_sizes[index2];
 						double wish_size = 0.5*( h1 + h2 );
 						wish_size *= wish_size;

 						
 						if( edge_lenght2  < 0.5*wish_size) //a node should be removed
 						{
							if( h1 < h2) //we should remove the first
 							{
 								//if the first is not boundary remove it else try to remove the second
										
								if( out.pointmarkerlist[index1] != 1)
 									preserved[ index1  ] = false;
 								else if( out.pointmarkerlist[index2] != 1)
 									preserved[ index2  ] = false;
 							}
 							else //trying to remove the second
 							{
 								if( out.pointmarkerlist[index2] != 1)
 									preserved[ index2  ] = false;
 								else if( out.pointmarkerlist[index1] != 1)
 									preserved[ index1  ] = false;
 							}
 						//KRATOS_WATCH("removing node");
 									
 						}
 						
 					}
 			}
			unsigned int n_nodes_after_removal=0;
			//NOW WE SHALL COUNT HOW MANY NODES REMAIN AFTER REMOVAL
			for(int i = 0; i<in.numberofpoints; i++)
			{
				if (preserved[i] == true)
					n_nodes_after_removal++;

			}

			//now we shall remove the "bad nodes" from the model part
			//ModelPart::NodesContainerType::iterator nodes_begin = ThisModelPart.NodesBegin();
			nodes_begin = ThisModelPart.NodesBegin();
			for(unsigned int i = 0; i<ThisModelPart.Nodes().size(); i++)
			{
				if (preserved[i]==false && ((nodes_begin+i)->FastGetSolutionStepValue(IS_STRUCTURE)==0.0)  )				
					(nodes_begin+i)->GetValue(ERASE_FLAG)=1.0;
			}
			KRATOS_WATCH("Number of nodes before erasing")
			KRATOS_WATCH(ThisModelPart.Nodes().size())	

			node_erase.Execute();				

			KRATOS_WATCH("Number of nodes after erasing")
			KRATOS_WATCH(ThisModelPart.Nodes().size())	
			
			//freeing the memory
			clean_triangulateio(in);
			clean_triangulateio(out);
			clean_triangulateio(vorout);
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//												  	//
			//		Now we shall pass the Alpha Shape for the second time, having the "bad nodes" removed	//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//creating the containers for the input and output
			struct triangulateio in_mid;
			struct triangulateio out_mid;
			struct triangulateio vorout_mid;

			initialize_triangulateio(in_mid);
			initialize_triangulateio(out_mid);
			initialize_triangulateio(vorout_mid);
			
			//assigning the size of the input containers
						
			in_mid.numberofpoints = ThisModelPart.Nodes().size();
			in_mid.pointlist = (REAL *) malloc(in_mid.numberofpoints * 2 * sizeof(REAL));

			//reorder the id's and give the coordinates to the mesher
			nodes_begin = ThisModelPart.NodesBegin();
			for(unsigned int i = 0; i<ThisModelPart.Nodes().size(); i++)
			{ 
				int base = i*2;
				//int base = ((nodes_begin + i)->Id()   -  1 ) * 2;

				//from now on it is consecutive
				(nodes_begin + i)->Id() = i+1;

				in_mid.pointlist[base] = (nodes_begin + i)->X();
				in_mid.pointlist[base+1] = (nodes_begin + i)->Y();
			}
			//in_mid.numberoftriangles = ThisModelPart.Elements().size();
			//in_mid.trianglelist = (int*) malloc(in_mid.numberoftriangles * 3 * sizeof(int));
			
			// "P" suppresses the output .poly file. Saves disk space, but you 				
			//lose the ability to maintain constraining segments  on later refinements of the mesh. 
			// "B" Suppresses boundary markers in the output .node, .poly, and .edge output files
			// "n" outputs a list of triangles neighboring each triangle.
			// "e" outputs edge list (i.e. all the "connectivities")
			char options1[] = "Pne";
			triangulate(options1, &in_mid, &out_mid, &vorout_mid);
			//print out the mesh generation time
			std::cout<<"mesh generation time = "<<auxiliary.elapsed();
			//number of newly generated triangles
			el_number=out_mid.numberoftriangles;

			
			//prepairing for alpha shape passing : creating necessary arrays
			//list of preserved elements is created: at max el_number can be preserved (all elements)
			std::vector<int> preserved_list1(el_number);
			//preserved_list.resize(el_number);

			//array_1d<double,3> x1,x2,x3,xc;

			//int number_of_preserved_elems=0;
			number_of_preserved_elems=0;
			//int point_base;
			//loop for passing alpha shape			
			for(int el = 0; el< el_number; el++)
			{
				int base = el * 3;

				//coordinates
				point_base = (out_mid.trianglelist[base] - 1)*2;
				x1[0] = out_mid.pointlist[point_base]; 
				x1[1] = out_mid.pointlist[point_base+1]; 

				point_base = (out_mid.trianglelist[base+1] - 1)*2;
				x2[0] = out_mid.pointlist[point_base]; 
				x2[1] = out_mid.pointlist[point_base+1]; 

				point_base = (out_mid.trianglelist[base+2] - 1)*2;
				x3[0] = out_mid.pointlist[point_base]; 
				x3[1] = out_mid.pointlist[point_base+1]; 

				//here we shall temporarily save the elements and pass them afterwards to the alpha shape
				Geometry<Node<3> > temp;

				temp.push_back( *((ThisModelPart.Nodes()).find( out_mid.trianglelist[base]).base()	) );
				temp.push_back( *((ThisModelPart.Nodes()).find( out_mid.trianglelist[base+1]).base()	) );
				temp.push_back( *((ThisModelPart.Nodes()).find( out_mid.trianglelist[base+2]).base()	) );

				int number_of_structure_nodes = int( temp[0].FastGetSolutionStepValue(IS_STRUCTURE) );
				number_of_structure_nodes += int( temp[1].FastGetSolutionStepValue(IS_STRUCTURE) );
				number_of_structure_nodes += int( temp[2].FastGetSolutionStepValue(IS_STRUCTURE) );

				//check the number of nodes of boundary
				int nfs = int( temp[0].FastGetSolutionStepValue(IS_FREE_SURFACE) );
				nfs += int( temp[1].FastGetSolutionStepValue(IS_FREE_SURFACE) );
				nfs += int( temp[2].FastGetSolutionStepValue(IS_FREE_SURFACE) );
				
				//check the number of nodes of boundary
				int nfluid = int( temp[0].FastGetSolutionStepValue(IS_FLUID) );
				nfluid += int( temp[1].FastGetSolutionStepValue(IS_FLUID) );
				nfluid += int( temp[2].FastGetSolutionStepValue(IS_FLUID) );
				//first check that we are working with fluid elements, otherwise throw an error
				//if (nfluid!=3)
				//	KRATOS_ERROR(std::logic_error,"THATS NOT FLUID or NOT TRIANGLE!!!!!! ERROR","");
				//otherwise perform alpha shape check

				
				if(number_of_structure_nodes!=3) //if it is = 3 it is a completely fixed element -> do not add it
				{
					if (nfs != 0 || nfluid != 3)  //in this case it is close to the surface so i should use alpha shape 
					{
						
						if( AlphaShape(my_alpha,temp) && number_of_structure_nodes!=3) //if alpha shape says to preserve
						{
							preserved_list1[el] = true;
							number_of_preserved_elems += 1;
														
						}
					}
					else //internal triangle --- should be ALWAYS preserved
					{							
						double bigger_alpha = my_alpha*10.0;
						if( AlphaShape(bigger_alpha,temp) && number_of_structure_nodes!=3) 
							{
							preserved_list1[el] = true;
							number_of_preserved_elems += 1;							
							}
					}				
				}
				else 
					preserved_list1[el] = false;
			}
			//freeing memory 

			clean_triangulateio(in_mid);
			clean_triangulateio(vorout_mid);

			//NOW WE SHALL PERFORM ADAPTIVE REMESHING, i.e. insert and remove nodes based upon mesh quality
			// and prescribed sizes
			struct triangulateio in2;
			struct triangulateio out2;
			struct triangulateio vorout2;
			
			initialize_triangulateio(in2);
			initialize_triangulateio(out2);
			initialize_triangulateio(vorout2);

//			in2.firstnumber = 1;
			in2.numberofpoints = ThisModelPart.Nodes().size();
			in2.pointlist = (REAL *) malloc(in2.numberofpoints * 2 * sizeof(REAL));

			//writing the input point list
			for(unsigned int i = 0; i<ThisModelPart.Nodes().size(); i++)
			{
				int base = i*2;
				in2.pointlist[base] = (nodes_begin + i)->X();
				in2.pointlist[base+1] = (nodes_begin + i)->Y();
			}
			in2.numberoftriangles=number_of_preserved_elems;

			in2.trianglelist = new int[in2.numberoftriangles * 3];
			in2.trianglearealist = new double[in2.numberoftriangles];

			int counter = 0;
			for (int el=0; el<el_number; el++)
			{
				if( preserved_list1[el] == true )
				{
					//saving the list of ONLY preserved triangles, the ones that passed alpha-shape check
					int new_base = counter*3;
					int old_base = el*3;
					//copying in case it is preserved					
					in2.trianglelist[new_base] = out_mid.trianglelist[old_base];
					in2.trianglelist[new_base+1] = out_mid.trianglelist[old_base+1];
					in2.trianglelist[new_base+2] = out_mid.trianglelist[old_base+2];

					//calculate the prescribed h
					double prescribed_h = (nodes_begin + out_mid.trianglelist[old_base]-1)->FastGetSolutionStepValue(NODAL_H);
					prescribed_h += (nodes_begin + out_mid.trianglelist[old_base+1]-1)->FastGetSolutionStepValue(NODAL_H);
					prescribed_h += (nodes_begin + out_mid.trianglelist[old_base+2]-1)->FastGetSolutionStepValue(NODAL_H);
					prescribed_h *= 0.3333;
					//if h is the height of a equilateral triangle, the area is 1/2*h*h					
					in2.trianglearealist[counter] = 0.5*(1.5*prescribed_h*1.5*prescribed_h);
					counter += 1;
				}
			
			}
			
			clean_triangulateio(out_mid);
			//here we generate a new mesh adding/removing nodes, by initializing "q"-quality mesh and "a"-area constraint switches
			//
			// MOST IMPORTANT IS "r" switch, that refines previously generated mesh!!!!!!!!!!(that is the one given inside in2)
			//char mesh_regen_opts[] = "YYJaqrn";
			char mesh_regen_opts[] = "YYJq1.4arn";
			triangulate(mesh_regen_opts, &in2, &out2, &vorout2);
			KRATOS_WATCH("Adaptive remeshing executed")
			//and now we shall find out where the new nodes belong to
			//defintions for spatial search
			typedef Node<3> PointType;
			typedef Node<3>::Pointer PointPointerType;
			typedef std::vector<PointType::Pointer>           PointVector;
			//typedef std::vector<PointType::Pointer>::iterator PointIterator;
			typedef PointVector::iterator PointIterator;
			typedef std::vector<double>               DistanceVector;
			typedef std::vector<double>::iterator     DistanceIterator;

			typedef Bucket<3, PointType, PointVector, PointPointerType, PointIterator, DistanceIterator > BucketType;

			typedef Tree< KDTreePartition<BucketType> > kd_tree; //Kdtree;

			int step_data_size = ThisModelPart.GetNodalSolutionStepDataSize();

			//creating an auxiliary list for the new nodes 
			PointVector list_of_new_nodes;
			
			//node to get the DOFs from					
			Node<3>::DofsContainerType& reference_dofs = (ThisModelPart.NodesBegin())->GetDofs();

			double z = 0.0;
			int n_points_before_refinement = in2.numberofpoints;
			//if points were added, we add them as nodes to the ModelPart
			if (out2.numberofpoints > n_points_before_refinement )
			{
			for(int i = n_points_before_refinement; i<out2.numberofpoints;i++)
				{
					int id=i+1;
					int base = i*2;
					double& x= out2.pointlist[base];
					double& y= out2.pointlist[base+1];

					Node<3>::Pointer pnode = ThisModelPart.CreateNewNode(id,x,y,z);
							
					list_of_new_nodes.push_back( pnode );
					
					//std::cout << "new node id = " << pnode->Id() << std::endl;
					//generating the dofs
					for(Node<3>::DofsContainerType::iterator iii = reference_dofs.begin();    iii != reference_dofs.end(); iii++)
					{
						Node<3>::DofType& rDof = *iii;
						Node<3>::DofType::Pointer p_new_dof = pnode->pAddDof( rDof );
						
						(p_new_dof)->FreeDof();
					}
					
				}
			}	
			
			std::cout << "During refinement we added " << out2.numberofpoints-n_points_before_refinement<< " nodes " <<std::endl;
			unsigned int bucket_size = 20;
			//performing the interpolation - all of the nodes in this list will be preserved
			//unsigned int max_results = 50;
			//PointVector results(max_results);
			//DistanceVector results_distances(max_results);
			array_1d<double,3> N;
				
			//WHAT ARE THOSE????
			Node<3> work_point(0,0.0,0.0,0.0);
			unsigned int MaximumNumberOfResults = 500;
			PointVector Results(MaximumNumberOfResults);
			DistanceVector ResultsDistances(MaximumNumberOfResults);

			step_data_size = ThisModelPart.GetNodalSolutionStepDataSize();

			if(out2.numberofpoints-n_points_before_refinement > 0) //if we added points
			{
						
				kd_tree  nodes_tree2(list_of_new_nodes.begin(),list_of_new_nodes.end(),bucket_size);
				nodes_begin = ThisModelPart.NodesBegin();
								
				for(int el = 0; el< in2.numberoftriangles; el++)
				{	
					int base = el * 3;
					//coordinates
					point_base = (in2.trianglelist[base] - 1)*2;
					x1[0] = in2.pointlist[point_base]; 
					x1[1] = in2.pointlist[point_base+1]; 

					point_base = (in2.trianglelist[base+1] - 1)*2;
					x2[0] = in2.pointlist[point_base]; 
					x2[1] = in2.pointlist[point_base+1]; 
					
					point_base = (in2.trianglelist[base+2] - 1)*2;
					x3[0] = in2.pointlist[point_base]; 
					x3[1] = in2.pointlist[point_base+1]; 
		
				
					//find the center and "radius" of the element
					double xc,  yc, radius;	
					CalculateCenterAndSearchRadius( x1[0], x1[1],
									x2[0], x2[1],
									x3[0], x3[1],
									xc,yc,radius);
									
					//find all of the new nodes within the radius
					int number_of_points_in_radius;
					work_point.X() = xc; work_point.Y() = yc; work_point.Z() = 0.0;
					
					number_of_points_in_radius = nodes_tree2.SearchInRadius(work_point, radius, Results.begin(),
							ResultsDistances.begin(),  MaximumNumberOfResults);

					Triangle2D3<Node<3> > geom(
						*( (nodes_begin +  in2.trianglelist[base]-1).base() 	), 
						*( (nodes_begin +  in2.trianglelist[base+1]-1).base() 	), 
						*( (nodes_begin +  in2.trianglelist[base+2]-1).base() 	)
						);	
	
					//check if inside and eventually interpolate
					for( PointIterator it_found = Results.begin(); it_found != Results.begin() + number_of_points_in_radius; it_found++)
					{
						bool is_inside = false;						
						is_inside = CalculatePosition(x1[0], x1[1],
										x2[0], x2[1],
										x3[0], x3[1],
										(*it_found)->X(),(*it_found)->Y(),N);
						
						
						if(is_inside == true)
						{
							Interpolate(  geom,  N, step_data_size, *(it_found.base() ) );
							
						}
					}
				}		
			}

			ThisModelPart.Elements().clear();
			
			//set the coordinates to the original value
			for( PointVector::iterator it =  list_of_new_nodes.begin(); it!=list_of_new_nodes.end(); it++)
			{				
				const array_1d<double,3>& disp = (*it)->FastGetSolutionStepValue(DISPLACEMENT);
				(*it)->X0() = (*it)->X() - disp[0];
				(*it)->Y0() = (*it)->Y() - disp[1];
				(*it)->Z0() = 0.0;	
			}
			//cleaning unnecessary data
			//in2.deinitialize();
			//in2.initialize();
			//free( in2.pointlist );
			
			//add preserved elements to the kratos
			Properties::Pointer properties = ThisModelPart.GetMesh().pGetProperties(1);
			nodes_begin = ThisModelPart.NodesBegin();
			(ThisModelPart.Elements()).reserve(out2.numberoftriangles);
						
			for(int iii = 0; iii< out2.numberoftriangles; iii++)
			{
				int id = iii + 1;
				int base = iii * 3;
				Triangle2D3<Node<3> > geom(
					*( (nodes_begin +  out2.trianglelist[base]-1).base() 	), 
					*( (nodes_begin +  out2.trianglelist[base+1]-1).base() 	), 
					*( (nodes_begin +  out2.trianglelist[base+2]-1).base() 	)
					);


#ifdef _DEBUG
ModelPart::NodesContainerType& ModelNodes = ThisModelPart.Nodes();
				if( *(ModelNodes).find( out2.trianglelist[base]).base() == *(ThisModelPart.Nodes().end()).base() ) 
					KRATOS_ERROR(std::logic_error,"trying to use an inexisting node","");
				if( *(ModelNodes).find( out2.trianglelist[base+1]).base() == *(ThisModelPart.Nodes().end()).base() ) 
					KRATOS_ERROR(std::logic_error,"trying to use an inexisting node","");
				if( *(ModelNodes).find( out2.trianglelist[base+2]).base() == *(ThisModelPart.Nodes().end()).base() ) 
					KRATOS_ERROR(std::logic_error,"trying to use an inexisting node","");
				
#endif

				Element::Pointer p_element = rReferenceElement.Create(id, geom, properties);
				(ThisModelPart.Elements()).push_back(p_element);

			}
			ThisModelPart.Elements().Sort();

			//filling the neighbour list 
			ModelPart::ElementsContainerType::const_iterator el_begin = ThisModelPart.ElementsBegin();
			for(ModelPart::ElementsContainerType::const_iterator iii = ThisModelPart.ElementsBegin();
				iii != ThisModelPart.ElementsEnd(); iii++)
			{
				//Geometry< Node<3> >& geom = iii->GetGeometry();
				int base = ( iii->Id() - 1 )*3;

				(iii->GetValue(NEIGHBOUR_ELEMENTS)).resize(3);
				WeakPointerVector< Element >& neighb = iii->GetValue(NEIGHBOUR_ELEMENTS);
				for(int i = 0; i<3; i++)
				{
					int index = out2.neighborlist[base+i];
					if(index > 0)
						neighb(i) = *((el_begin + index-1).base());
					else
						neighb(i) = Element::WeakPointer();
				}
			}

			//reset the boundary flag
			for(ModelPart::NodesContainerType::const_iterator in = ThisModelPart.NodesBegin(); in!=ThisModelPart.NodesEnd(); in++)
			{
				in->FastGetSolutionStepValue(IS_BOUNDARY) = 0;
			}
			//filling the elemental neighbours list (from now on the elements list can not change)
			ModelPart::ElementsContainerType::iterator elements_end = ThisModelPart.Elements().end();

			ThisModelPart.Elements().Unique();
			
			//now the boundary faces
			for(ModelPart::ElementsContainerType::iterator iii = ThisModelPart.ElementsBegin();	iii != ThisModelPart.ElementsEnd(); iii++)
			{
				int base = ( iii->Id() - 1 )*3;
				
				ModelPart::ElementsContainerType::iterator el_neighb;
				/*each face is opposite to the corresponding node number so
				 0 ----- 1 2 
				 1 ----- 2 0
				 2 ----- 0 1 
				*/

				////finding boundaries and creating the "skin"
				//
				//********************************************************************
				//first face
				el_neighb = (ThisModelPart.Elements()).find( out2.neighborlist[base] );
				if( el_neighb == elements_end )
				{
					//std::cout << "node0" << std::endl;
					//if no neighnour is present => the face is free surface
					iii->GetGeometry()[1].FastGetSolutionStepValue(IS_BOUNDARY) = 1;
					iii->GetGeometry()[2].FastGetSolutionStepValue(IS_BOUNDARY) = 1;
				
					//Generate condition
					Condition::NodesArrayType temp;
					temp.reserve(2);
					temp.push_back(iii->GetGeometry()(1)); 
					temp.push_back(iii->GetGeometry()(2));
				
					Geometry< Node<3> >::Pointer cond = 
						Geometry< Node<3> >::Pointer(new Geometry< Node<3> >(temp) );
					int id = (iii->Id()-1)*3;
					
					Condition::Pointer p_cond = 
						Condition::Pointer(new Condition(id, cond, properties) );	

					ThisModelPart.Conditions().push_back(p_cond);

				}
				
				//********************************************************************
				//second face
				el_neighb = (ThisModelPart.Elements()).find( out2.neighborlist[base+1] );
				//if( el != ThisModelPart.Elements().end() )
				if( el_neighb == elements_end )
				{
					//if no neighnour is present => the face is free surface
					iii->GetGeometry()[2].FastGetSolutionStepValue(IS_BOUNDARY) = 1;
					iii->GetGeometry()[0].FastGetSolutionStepValue(IS_BOUNDARY) = 1;

					//Generate condition
					Condition::NodesArrayType temp;
					temp.reserve(2);
					temp.push_back(iii->GetGeometry()(2)); 
					temp.push_back(iii->GetGeometry()(0));
					
					Geometry< Node<3> >::Pointer cond = 
						Geometry< Node<3> >::Pointer(new Geometry< Node<3> >(temp) );
					int id = (iii->Id()-1)*3+1;
					//
					Condition::Pointer p_cond = 
						Condition::Pointer(new Condition(id, cond, properties) );
					
					ThisModelPart.Conditions().push_back(p_cond);
									

				}

				//********************************************************************
				//third face
				el_neighb = (ThisModelPart.Elements()).find( out2.neighborlist[base+2] );
				if( el_neighb == elements_end )
				{
					//if no neighnour is present => the face is free surface
					iii->GetGeometry()[0].FastGetSolutionStepValue(IS_BOUNDARY) = 1;
					iii->GetGeometry()[1].FastGetSolutionStepValue(IS_BOUNDARY) = 1;
					
//					Generate condition
					Condition::NodesArrayType temp;
					temp.reserve(2);
					temp.push_back(iii->GetGeometry()(0)); 
					temp.push_back(iii->GetGeometry()(1));
					Geometry< Node<3> >::Pointer cond = 
						Geometry< Node<3> >::Pointer(new Geometry< Node<3> >(temp) );
					int id = (iii->Id()-1)*3+2;
					
					Condition::Pointer p_cond = 
						Condition::Pointer(new Condition(id, cond, properties) );
					
					ThisModelPart.Conditions().push_back(p_cond);
					

				}

			
			}
			KRATOS_WATCH("Finished remeshing with Trigen_PFEM_Refine")

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

		/// Turn back information as a string.
		virtual std::string Info() const{return "";}

		/// Print information about this object.
		virtual void PrintInfo(std::ostream& rOStream) const{}

		/// Print object's data.
		virtual void PrintData(std::ostream& rOStream) const{}


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
		 boost::numeric::ublas::bounded_matrix<double,2,2> mJ; //local jacobian
		 boost::numeric::ublas::bounded_matrix<double,2,2> mJinv; //inverse jacobian
		 array_1d<double,2> mC; //center pos
		 array_1d<double,2> mRhs; //center pos
		 //NodeEraseProcess* mpNodeEraseProcess;


		///@} 
		///@name Private Operators
		///@{ 
		//returns false if it should be removed
		bool AlphaShape(double alpha_param, Geometry<Node<3> >& pgeom)
			//bool AlphaShape(double alpha_param, Triangle2D<Node<3> >& pgeom)
		{
			KRATOS_TRY
			
			
			double x0 = pgeom[0].X();
			double x1 = pgeom[1].X();
			double x2 = pgeom[2].X();
			
			double y0 = pgeom[0].Y();
			double y1 = pgeom[1].Y();
			double y2 = pgeom[2].Y();
						
			mJ(0,0)=2.0*(x1-x0);	mJ(0,1)=2.0*(y1-y0);
			mJ(1,0)=2.0*(x2-x0);	mJ(1,1)=2.0*(y2-y0);
			
			
			double detJ = mJ(0,0)*mJ(1,1)-mJ(0,1)*mJ(1,0);
						
			mJinv(0,0) =  mJ(1,1); mJinv(0,1) = -mJ(0,1);
			mJinv(1,0) = -mJ(1,0); mJinv(1,1) =  mJ(0,0);
		
			bounded_matrix<double,2,2> check;
		
			
			if(detJ < 1e-12) 
			{
				//std::cout << "detJ = " << detJ << std::endl;
				////mark as boundary
				pgeom[0].GetSolutionStepValue(IS_BOUNDARY) = 1;
				pgeom[1].GetSolutionStepValue(IS_BOUNDARY) = 1;
				pgeom[2].GetSolutionStepValue(IS_BOUNDARY) = 1;
				return false;
			}
			
			else
			{

				double x0_2 = x0*x0 + y0*y0;
				double x1_2 = x1*x1 + y1*y1; 
				double x2_2 = x2*x2 + y2*y2; 

				//finalizing the calculation of the inverted matrix
				//std::cout<<"MATR INV"<<MatrixInverse(mJ)<<std::endl;
				mJinv /= detJ;
				//calculating the RHS
				mRhs[0] = (x1_2 - x0_2);
				mRhs[1] = (x2_2 - x0_2);

				//calculate position of the center
				noalias(mC) = prod(mJinv,mRhs);

				double radius = sqrt(pow(mC[0]-x0,2)+pow(mC[1]-y0,2));

				//calculate average h
				double h;
				h =  pgeom[0].FastGetSolutionStepValue(NODAL_H);
				h += pgeom[1].FastGetSolutionStepValue(NODAL_H);
				h += pgeom[2].FastGetSolutionStepValue(NODAL_H);
				h *= 0.333333333;
				if (radius < h*alpha_param)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			

			KRATOS_CATCH("")
		}
		//AUXILLIARY FCTNS
		inline void CalculateCenterAndSearchRadius(const double x0, const double y0,
						const double x1, const double y1,
    						const double x2, const double y2,
	  					double& xc, double& yc, double& R
					       )
		{
			xc = 0.3333333333333333333*(x0+x1+x2);
			yc = 0.3333333333333333333*(y0+y1+y2);
			 
			double R1 = (xc-x0)*(xc-x0) + (yc-y0)*(yc-y0);
			double R2 = (xc-x1)*(xc-x1) + (yc-y1)*(yc-y1);
			double R3 = (xc-x2)*(xc-x2) + (yc-y2)*(yc-y2);
			
			R = R1;
			if(R2 > R) R = R2;
			if(R3 > R) R = R3;
			
			R = sqrt(R);
		}
		
		inline double CalculateVol(	const double x0, const double y0,
						const double x1, const double y1,
    						const double x2, const double y2
					  )
		{
			return 0.5*( (x1-x0)*(y2-y0)- (y1-y0)*(x2-x0) );
		}
		
		inline bool CalculatePosition(	const double x0, const double y0,
						const double x1, const double y1,
   						const double x2, const double y2,
						const double xc, const double yc,
						array_1d<double,3>& N		
					  )
		{
			double area = CalculateVol(x0,y0,x1,y1,x2,y2);
			double inv_area = 0.0;
			if(area < 0.000000000001)
			  {
				KRATOS_ERROR(std::logic_error,"element with zero area found","");
			  }
			else
			  {
				inv_area = 1.0 / area;
			  }
			
			  
			  N[0] = CalculateVol(x1,y1,x2,y2,xc,yc) * inv_area;
			  N[1] = CalculateVol(x2,y2,x0,y0,xc,yc) * inv_area;
			  N[2] = CalculateVol(x0,y0,x1,y1,xc,yc) * inv_area;
			  
/*			  N[0] = CalculateVol(x0,y0,x1,y1,xc,yc) * inv_area;
			N[1] = CalculateVol(x1,y1,x2,y2,xc,yc) * inv_area;
			N[2] = CalculateVol(x2,y2,x0,y0,xc,yc) * inv_area;*/
			
			if(N[0] > 0.0 && N[1] > 0.0 && N[2] > 0.0 && N[0] < 1.0 && N[1] < 1.0 && N[2] < 1.0) //if the xc yc is inside the triangle
			//if(N[0] >= 0.0 && N[1] >= 0.0 && N[2] >= 0.0 && N[0] <= 1.0 && N[1] <= 1.0 && N[2] <= 1.0) //if the xc yc is inside the triangle return true
				return true;
			
			return false;
		}	
			
		void Interpolate( Triangle2D3<Node<3> >& geom, const array_1d<double,3>& N, 
				  unsigned int step_data_size,
      				Node<3>::Pointer pnode)
		{
			unsigned int buffer_size = pnode->GetBufferSize();
			//KRATOS_WATCH("Buffer size")
			//KRATOS_WATCH(buffer_size)

			for(unsigned int step = 0; step<buffer_size; step++)
			{	
				
						//getting the data of the solution step
				double* step_data = (pnode)->SolutionStepData().Data(step);
				
											
				double* node0_data = geom[0].SolutionStepData().Data(step);
				double* node1_data = geom[1].SolutionStepData().Data(step);
				double* node2_data = geom[2].SolutionStepData().Data(step);
								
				//copying this data in the position of the vector we are interested in
				for(unsigned int j= 0; j<step_data_size; j++)
				{ 

					step_data[j] = N[0]*node0_data[j] + N[1]*node1_data[j] + N[2]*node2_data[j];
								

				}						
			}
			if (N[0]==0.0 && N[1]==0.0 && N[2]==0.0) 
					KRATOS_ERROR(std::logic_error,"SOMETHING's wrong with the added nodes!!!!!! ERROR","");

			//if ( pnode->FastGetSolutionStepValue(BULK_MODULUS)==0.0) 
			//		KRATOS_ERROR(std::logic_error,"SOMETHING's wrong with the added nodes!!!!!! ERROR","");
			
			//now we assure that the flag variables are set coorect!! since we add nodes inside of the fluid volume only
			//we manually reset the IS_BOUNDARY, IS_FLUID, IS_STRUCTURE, IS_FREE_SURFACE values in a right way
			//not to have values, like 0.33 0.66 resulting if we would have been interpolating them in the same way 		
			//as the normal variables, like Velocity etc		
			
			
			pnode->FastGetSolutionStepValue(IS_BOUNDARY)=0.0;
			pnode->FastGetSolutionStepValue(IS_STRUCTURE)=0.0;
			pnode->GetValue(ERASE_FLAG)=0.0;
			pnode->FastGetSolutionStepValue(IS_FREE_SURFACE)=0.0;
			pnode->FastGetSolutionStepValue(IS_FLUID)=1.0;
		}

		void initialize_triangulateio( triangulateio& tr )
		{
			tr.pointlist                  = (REAL*) NULL;
			tr.pointattributelist         = (REAL*) NULL;
			tr.pointmarkerlist            = (int*) NULL;
			tr.numberofpoints             = 0;
			tr.numberofpointattributes    = 0;
			tr.trianglelist               = (int*) NULL;
			tr.triangleattributelist      = (REAL*) NULL;
			tr.trianglearealist           = (REAL*) NULL;
			tr.neighborlist               = (int*) NULL;
			tr.numberoftriangles          = 0;
			tr.numberofcorners            = 3;
			tr.numberoftriangleattributes = 0;
			tr.segmentlist                = (int*) NULL;
			tr.segmentmarkerlist          = (int*) NULL;
			tr.numberofsegments           = 0;
			tr.holelist                   = (REAL*) NULL;
			tr.numberofholes              = 0;
			tr.regionlist                 = (REAL*) NULL;
			tr.numberofregions            = 0;
			tr.edgelist                   = (int*) NULL;
			tr.edgemarkerlist             = (int*) NULL;
			tr.normlist                   = (REAL*) NULL;
			tr.numberofedges              = 0;
		};

		void clean_triangulateio( triangulateio& tr )
		{
			free(tr.pointlist );
			free(tr.pointattributelist );
			free(tr.pointmarkerlist   );
			free(tr.trianglelist  );
			free(tr.triangleattributelist );
			free(tr.trianglearealist );
			free(tr.neighborlist   );
			free(tr.segmentlist    );
			free(tr.segmentmarkerlist   );
			free(tr.holelist      );
			free(tr.regionlist  );
			free(tr.edgelist   );
			free(tr.edgemarkerlist   );
			free(tr.normlist  );
		};
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
		TriGenPFEMModeler& operator=(TriGenPFEMModeler const& rOther);


		///@}    

	}; // Class TriGenPFEMModeler 

	///@} 

	///@name Type Definitions       
	///@{ 


	///@} 
	///@name Input and output 
	///@{ 


	/// input stream function
	inline std::istream& operator >> (std::istream& rIStream, 
		TriGenPFEMModeler& rThis);

	/// output stream function
	inline std::ostream& operator << (std::ostream& rOStream, 
		const TriGenPFEMModeler& rThis)
	{
		rThis.PrintInfo(rOStream);
		rOStream << std::endl;
		rThis.PrintData(rOStream);

		return rOStream;
	}
	///@} 


}  // namespace Kratos.

#endif // KRATOS_TRIGEN_PFEM_MODELER_H_INCLUDED  defined 


				
