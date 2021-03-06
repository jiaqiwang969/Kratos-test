//   
//   Project Name:        Kratos       
//   Last Modified by:    $Author: antonia $
//   Date:                $Date: 2008-10-10 14:04:56 $
//   Revision:            $Revision: 1.1 $
//
//


#if !defined(KRATOS_KD_TREE_H_INCLUDED )
#define  KRATOS_KD_TREE_H_INCLUDED



// System includes
#include <string>
#include <iostream> 
#include <cstddef>
#include <vector>


// External includes 


// Project includes
//#include "includes/define.h"
#include "tree.h"


namespace Kratos
{
	//template<std::size_t Dimension, class TPointType, class TPointsContainerType>
	//class KDTree;
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
	template< class TLeafType >
	class KDTreePartition : public TreeNode< TLeafType::Dimension,
                                             typename TLeafType::PointType, 
	                                         typename TLeafType::PointerType,
											 typename TLeafType::IteratorType,
											 typename TLeafType::DistanceIteratorType >
	{
    public:
      ///@name Type Definitions
      ///@{
      
      /// Pointer definition of KDTree
      KRATOS_CLASS_POINTER_DEFINITION(KDTreePartition);

	  typedef TLeafType LeafType;

	  typedef typename LeafType::PointType PointType;

	  typedef typename LeafType::ContainerType ContainerType;

	  typedef typename LeafType::IteratorType IteratorType;

	  typedef typename LeafType::DistanceIteratorType DistanceIteratorType;

	  typedef typename LeafType::PointerType PointerType;

	  typedef typename LeafType::DistanceFunction DistanceFunction;

	  enum { Dimension = LeafType::Dimension };

	  typedef TreeNode<Dimension,PointType, PointerType, IteratorType, DistanceIteratorType> TreeNodeType;
	  
	  typedef typename TreeNodeType::CoordinateType CoordinateType;

	  typedef typename TreeNodeType::SizeType SizeType;

	  typedef typename TreeNodeType::IndexType IndexType;

      typedef typename TreeNodeType::SearchStructureType SearchStructureType;

      ///@}
      ///@name Life Cycle 
      ///@{ 
	    

      /// Partition constructor.
	  KDTreePartition(IndexType CutingDimension, CoordinateType Position,
		  CoordinateType LeftEnd, CoordinateType RightEnd,
		  TreeNodeType* pLeftChild = NULL, TreeNodeType* pRightChild = NULL)
		  : mCutingDimension(CutingDimension), mPosition(Position), mLeftEnd(LeftEnd), mRightEnd(RightEnd)
	  {
		  mpChilds[0] = pLeftChild;
		  mpChilds[1] = pRightChild;
	  }

	  virtual void PrintData(std::ostream& rOStream, std::string const& Perfix = std::string()) const
	  {
		  rOStream << Perfix << "Partition at ";
		  switch(mCutingDimension)
		  {
		  case 0:
			  rOStream << "X =";
			  break;
		  case 1:
			  rOStream << "Y =";
			  break;
		  case 2:
			  rOStream << "Z =";
			  break;
		  default:
			  rOStream << mCutingDimension << " in";
			  break;
		  }
		  rOStream << mPosition << " from " << mLeftEnd << " to " << mRightEnd << std::endl;

		  mpChilds[0]->PrintData(rOStream, Perfix + "  ");
		  mpChilds[1]->PrintData(rOStream, Perfix + "  ");

	  }

      /// Destructor.
	  virtual ~KDTreePartition()
	  {
		  delete mpChilds[0];
		  delete mpChilds[1];
	  }

      
      ///@}
      ///@name Operations
      ///@{

	  void SearchNearestPoint(PointType const& rThisPoint, PointerType& rResult, CoordinateType& rResultDistance )
	  {
        SearchStructureType Auxiliar;
        for(SizeType i = 0 ; i < Dimension; i++)
          Auxiliar.residual_distance[i] = 0.00;
        SearchNearestPoint(rThisPoint, rResult, rResultDistance, Auxiliar );
	  }

	  void SearchNearestPoint(PointType const& rThisPoint, PointerType& rResult, CoordinateType& rResultDistance, SearchStructureType& Auxiliar )
	  {
        CoordinateType temp = Auxiliar.residual_distance[mCutingDimension];
		CoordinateType distance_to_partition = rThisPoint[mCutingDimension] - mPosition;
		//CoordinateType distance_to_partition2 = distance_to_partition * distance_to_partition + rResidualDistance;

		if( distance_to_partition < 0.0 ) // The point is in the left partition
		{
		  //searching in the left child
		  mpChilds[0]->SearchNearestPoint(rThisPoint, rResult, rResultDistance, Auxiliar );
          
		  // compare with distance to right partition
          Auxiliar.residual_distance[mCutingDimension] = distance_to_partition * distance_to_partition;
          Auxiliar.distance_to_partition2 = Auxiliar.residual_distance[0];
          for(SizeType i = 1; i < Dimension; i++)
            Auxiliar.distance_to_partition2 += Auxiliar.residual_distance[i];
		  if( rResultDistance > Auxiliar.distance_to_partition2 )
			mpChilds[1]->SearchNearestPoint(rThisPoint, rResult, rResultDistance, Auxiliar );

		}
		else  // The point is in the right partition
		{
		  //Searching in the right child
		  mpChilds[1]->SearchNearestPoint(rThisPoint, rResult, rResultDistance, Auxiliar );
          
		  // compare with distance to left partition
          Auxiliar.residual_distance[mCutingDimension] = distance_to_partition * distance_to_partition;
          Auxiliar.distance_to_partition2 = Auxiliar.residual_distance[0];
          for(SizeType i = 1; i < Dimension; i++)
            Auxiliar.distance_to_partition2 += Auxiliar.residual_distance[i];
		  if( rResultDistance > Auxiliar.distance_to_partition2 )
			mpChilds[0]->SearchNearestPoint( rThisPoint, rResult, rResultDistance, Auxiliar );
		}
        Auxiliar.residual_distance[mCutingDimension] = temp;

	  }

      void SearchInRadius(PointType const& ThisPoint, CoordinateType const& Radius, CoordinateType const& Radius2, IteratorType& Results,
            DistanceIteratorType& ResultsDistances, SizeType& NumberOfResults, SizeType const& MaxNumberOfResults)
      {
        SearchStructureType Auxiliar;
        for(SizeType i = 0 ; i < Dimension; i++)
          Auxiliar.residual_distance[i] = 0.00;
        //Auxiliar.BucketCounter = 0;
        SearchInRadius(ThisPoint, Radius, Radius2, Results, ResultsDistances, NumberOfResults, MaxNumberOfResults, Auxiliar );
        //gBucketCounter = Auxiliar.BucketCounter;
	  }

      void SearchInRadius(PointType const& ThisPoint, CoordinateType const& Radius, CoordinateType const& Radius2, IteratorType& Results, 
            DistanceIteratorType& ResultsDistances, SizeType& NumberOfResults, SizeType const& MaxNumberOfResults, SearchStructureType& Auxiliar )
      {
        CoordinateType temp = Auxiliar.residual_distance[mCutingDimension];
        CoordinateType distance_to_partition = ThisPoint[mCutingDimension] - mPosition;

		  //SizeType n = 0; // Number of founded points

		  if(distance_to_partition < 0) // The point is in the left partition
		  {
			 //searching in the left child
            mpChilds[0]->SearchInRadius(ThisPoint, Radius, Radius2, Results, ResultsDistances, NumberOfResults, MaxNumberOfResults, Auxiliar );

            Auxiliar.residual_distance[mCutingDimension] = distance_to_partition * distance_to_partition;
            Auxiliar.distance_to_partition2 = Auxiliar.residual_distance[0];
            for(SizeType i = 1; i < Dimension; i++)
              Auxiliar.distance_to_partition2 += Auxiliar.residual_distance[i];
            // The points is too near to the wall and the other child is in the searching radius
            if( Radius2 >= Auxiliar.distance_to_partition2 )
              mpChilds[1]->SearchInRadius(ThisPoint, Radius, Radius2, Results, ResultsDistances, NumberOfResults, MaxNumberOfResults, Auxiliar );
		  }
		  else // The point is in the right partition
		  {
			 //searching in the left child
			 mpChilds[1]->SearchInRadius(ThisPoint, Radius, Radius2, Results, ResultsDistances, NumberOfResults, MaxNumberOfResults, Auxiliar );

            Auxiliar.residual_distance[mCutingDimension] = distance_to_partition * distance_to_partition;
            Auxiliar.distance_to_partition2 = Auxiliar.residual_distance[0];
            for(SizeType i = 1; i < Dimension; i++)
              Auxiliar.distance_to_partition2 += Auxiliar.residual_distance[i];
             // The points is too near to the wall and the other child is in the searching radius
             if( Radius2 >= Auxiliar.distance_to_partition2 )
               mpChilds[0]->SearchInRadius(ThisPoint, Radius, Radius2, Results, ResultsDistances, NumberOfResults, MaxNumberOfResults, Auxiliar );
          }
          Auxiliar.residual_distance[mCutingDimension] = temp;
			
	 }

	  void SearchInRadius(PointType const& ThisPoint, CoordinateType const& Radius, CoordinateType const& Radius2, IteratorType& Results, 
			SizeType& NumberOfResults, SizeType const& MaxNumberOfResults)
	  {
        SearchStructureType Auxiliar;
        for(SizeType i = 0 ; i < Dimension; i++)
          Auxiliar.residual_distance[i] = 0.00;
        SearchInRadius(ThisPoint, Radius, Radius2, Results, NumberOfResults, MaxNumberOfResults, Auxiliar );
	  }

	  void SearchInRadius(PointType const& ThisPoint, CoordinateType const& Radius, CoordinateType const& Radius2, IteratorType& Results, 
			SizeType& NumberOfResults, SizeType const& MaxNumberOfResults, SearchStructureType& Auxiliar )
	  {
        CoordinateType temp = Auxiliar.residual_distance[mCutingDimension];
        CoordinateType distance_to_partition = ThisPoint[mCutingDimension] - mPosition;

        if(distance_to_partition < 0) // The point is in the left partition
        {
          //searching in the left child
          mpChilds[0]->SearchInRadius(ThisPoint, Radius, Radius2, Results, NumberOfResults, MaxNumberOfResults, Auxiliar );

          Auxiliar.residual_distance[mCutingDimension] = distance_to_partition * distance_to_partition;
          Auxiliar.distance_to_partition2 = Auxiliar.residual_distance[0];
          for(SizeType i = 1; i < Dimension; i++)
            Auxiliar.distance_to_partition2 += Auxiliar.residual_distance[i];
          // The points is too near to the wall and the other child is in the searching radius
          if( Radius2 >= Auxiliar.distance_to_partition2 )
            mpChilds[1]->SearchInRadius(ThisPoint, Radius, Radius2, Results, NumberOfResults, MaxNumberOfResults, Auxiliar );
          Auxiliar.residual_distance[mCutingDimension] = temp;
        }
        else // The point is in the right partition
        {
          //searching in the left child
          mpChilds[1]->SearchInRadius(ThisPoint, Radius, Radius2, Results, NumberOfResults, MaxNumberOfResults, Auxiliar );

          Auxiliar.residual_distance[mCutingDimension] = distance_to_partition * distance_to_partition;
          Auxiliar.distance_to_partition2 = Auxiliar.residual_distance[0];
          for(SizeType i = 1; i < Dimension; i++)
            Auxiliar.distance_to_partition2 += Auxiliar.residual_distance[i];
          // The points is too near to the wall and the other child is in the searching radius
          if( Radius2 >= Auxiliar.distance_to_partition2 )
            mpChilds[0]->SearchInRadius(ThisPoint, Radius, Radius2, Results, NumberOfResults, MaxNumberOfResults, Auxiliar );
          Auxiliar.residual_distance[mCutingDimension] = temp;
        }

	  }

	  static IteratorType Partition( IteratorType PointsBegin, IteratorType PointsEnd, IndexType& rCuttingDimension, CoordinateType& rCuttingValue )
	  {
		 SizeType n = std::distance(PointsBegin, PointsEnd);
		 // find dimension of maximum spread
		 rCuttingDimension = MaxSpread(PointsBegin, PointsEnd);
		 IteratorType partition = PointsBegin + n / 2;

		 MedianSplit(PointsBegin, partition, PointsEnd, rCuttingDimension, rCuttingValue);

		 return partition;
	  }

      static IteratorType PartitionAverage( IteratorType PointsBegin, IteratorType PointsEnd, IndexType& rCuttingDimension, CoordinateType& rCuttingValue )
      { 
        IteratorType left  = PointsBegin;
        IteratorType right = PointsEnd - 1;
        SizeType size = std::distance(left,right);


        // Find cutting value & dimension
        CoordinateType Values[Dimension];
        for(IndexType i_dim = 0 ; i_dim < Dimension ; i_dim++)
          Values[i_dim] = 0.0;
        for(IteratorType i_point = left ; i_point != right ; i_point++)
          for(IndexType i_dim = 0 ; i_dim < Dimension ; i_dim++)
            Values[i_dim] += (**i_point)[i_dim];
        rCuttingValue = 0.0;
        for(IndexType i_dim = 0 ; i_dim < Dimension ; i_dim++)
        {
          CoordinateType temp = Values[i_dim] / static_cast<CoordinateType>(size);
          if( temp > rCuttingValue ){
            rCuttingValue = temp;
            rCuttingDimension = i_dim;
          }
        }

        // reorder by cutting_dimension
        while (left < right) {
          //if ( (**left)[rCuttingDimension] <= rCuttingValue ) {
          if ( (**left)[rCuttingDimension] < rCuttingValue ) {
            left++; // good where it is.
          } else {
            std::swap(*left,*right);
            right--;
          }
        }
        // here left=right
        /*         IteratorType PartitionPosition; */
        /*         if( (**left)[rCuttingDimension] <= rCuttingValue ) */
        /*           PartitionPosition = left; */
        /*         else */
        /*           PartitionPosition = left-1; */
        IteratorType PartitionPosition = left;

        return PartitionPosition;
      }


	  static CoordinateType Spread(	IteratorType PointsBegin, IteratorType PointsEnd, IndexType CoordinateIndex )
	  {
		 if(PointsBegin == PointsEnd) 
			return CoordinateType(); // If there is no point so the distance is zero!!

		 CoordinateType min = (**PointsBegin)[CoordinateIndex];
		 CoordinateType max = (**PointsBegin)[CoordinateIndex];
		 for (; PointsBegin != PointsEnd; PointsBegin++) 
		 {
			CoordinateType c = (**PointsBegin)[CoordinateIndex];
			if (c < min) 
			   min = c;
			else if (c > max) 
			   max = c;
		 }
		 return (max - min);
	  }
	  
	  // compute dimension of max spread
	  static SizeType MaxSpread( IteratorType PointsBegin, IteratorType PointsEnd )
	  {
		  SizeType max_dimension = 0;					// dimension of max spread
		  CoordinateType max_spread = 0;				// amount of max spread

		  if(PointsBegin == PointsEnd)  // If there is no point return the first coordinate
			  return max_dimension;

		  for (std::size_t d = 0; d < Dimension; d++) {		// compute spread along each dim
			  CoordinateType spread = Spread(PointsBegin, PointsEnd,d);
			  if (spread > max_spread) {			// bigger than current max
				  max_spread = spread;
				  max_dimension = d;
			  }
		  }
		  return max_dimension;
	  }


	  static void MedianSplit( IteratorType PointsBegin, IteratorType PartitionPosition, IteratorType PointsEnd,
			                   IndexType CuttingDimension, CoordinateType& rCuttingValue )
	  {
		 IteratorType left  = PointsBegin;
		 IteratorType right = PointsEnd - 1;
		 while (left < right) { // Iterating to find the partition point

			IteratorType i = left; // selecting left as pivot
			if ((**i)[CuttingDimension] > (**right)[CuttingDimension])
			   std::swap(*i,*right);

			CoordinateType value = (**i)[CuttingDimension];
			IteratorType j = right;
			for(;;) 
			{
			   while ((**(++i))[CuttingDimension] < value);
			   while ((**(--j))[CuttingDimension] > value);
			   if (i < j) std::swap(*i,*j); else break;
			}
			std::swap(*left,*j);

			if (j > PartitionPosition)
			   right = j - 1;
			else if (j < PartitionPosition)
			   left = j + 1;
			else break;
		 }

		 CoordinateType max = (**PointsBegin)[CuttingDimension];
		 IteratorType k = PointsBegin;
		 IteratorType last = PartitionPosition;

		 for(IteratorType i = PointsBegin ; i != PartitionPosition ; ++i)
			if((**i)[CuttingDimension] > max)
			{
			   max = (**i)[CuttingDimension];
			   k = i;
			}

		 if(k != PointsBegin)
			std::swap(--last, k);

		 rCuttingValue = ((**last)[CuttingDimension] + (**PartitionPosition)[CuttingDimension])/2.0;
	  }

	private:

	  IndexType mCutingDimension;
	  CoordinateType mPosition;   // Position of partition
	  CoordinateType mLeftEnd;    // Left extend of partition
	  CoordinateType mRightEnd;   // Right end of partition
	  TreeNodeType* mpChilds[2];  // The mpChilds[0] is the left child 
								  // and mpChilds[1] is the right child.




	public:
	  static TreeNodeType* Construct(IteratorType PointsBegin, IteratorType PointsEnd, PointType HighPoint, PointType LowPoint,	SizeType BucketSize)
	  {
		 SizeType number_of_points = std::distance(PointsBegin,PointsEnd);
		 if (number_of_points == 0)
			return NULL;
		 else if (number_of_points <= BucketSize)
		 {
			return new LeafType(PointsBegin, PointsEnd); 
		 }
		 else 
		 {
			IndexType cutting_dimension;
			CoordinateType cutting_value;

			IteratorType partition = Partition(PointsBegin, PointsEnd, cutting_dimension, cutting_value);
			//IteratorType partition = PartitionAverage(PointsBegin, PointsEnd, cutting_dimension, cutting_value);

			PointType partition_high_point = HighPoint;
			PointType partition_low_point = LowPoint;

			partition_high_point[cutting_dimension] = cutting_value;
			partition_low_point[cutting_dimension] = cutting_value;

			return new KDTreePartition( cutting_dimension, cutting_value, 
				  HighPoint[cutting_dimension], LowPoint[cutting_dimension],
				  Construct(PointsBegin, partition, partition_high_point, LowPoint, BucketSize), 
				  Construct(partition, PointsEnd, HighPoint, partition_low_point, BucketSize) );						

		 }
	  }


	};
  
  
}  // namespace Kratos.

#endif // KRATOS_KD_TREE_H_INCLUDED  defined 


