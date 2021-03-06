/*
 * SimpleVecNavigator.h
 *
 *  Created on: Dec 12, 2013
 *      Author: swenzel
 */

#ifndef SIMPLEVECNAVIGATOR_H_
#define SIMPLEVECNAVIGATOR_H_

#include "PhysicalVolume.h"
#include "Vector3D.h"
#include "Vector3DFast.h"

// the volume path class caches some important properties about where particles are in the geometry hierarchy
class VolumePath
{
	private:
		int fmaxlevel;
		int fcurrentlevel;
		PhysicalVolume const *  * path;
		TransformationMatrix const * * cache_of_globalmatrices;

	public:
		VolumePath(int);

		inline int GetMaxLevel() const {return fmaxlevel;}
		inline int GetCurrentLevel() const {return fcurrentlevel;}

		inline PhysicalVolume const * At(int i) const;
		inline void SetAt(int i, PhysicalVolume const *);

		// better to use pop and push
		inline void Push( PhysicalVolume const *);
		inline PhysicalVolume const * Top() const;
		inline void Pop();

		int Distance( VolumePath const & ) const;

		// clear all information
		void Clear();
		inline int Size() const { return fcurrentlevel; }
		void Print() const;

		void GetGlobalMatrixFromPath( TransformationMatrix * m ) const;
		void GetGlobalMatrixFromPath( FastTransformationMatrix * m ) const;

		inline VolumePath & operator=( VolumePath const & rhs );
};

static
inline
bool
operator==( VolumePath const & lhs, VolumePath const & rhs )
{
	if( lhs.GetMaxLevel()!=rhs.GetMaxLevel() ) return false;
	if( lhs.GetCurrentLevel()!=rhs.GetCurrentLevel() ) return false;
	for(int i=0;i<lhs.GetCurrentLevel();i++){
		if (lhs.At(i) != rhs.At(i)) return false;
	}
	return true;
}

inline
VolumePath & VolumePath::operator=(VolumePath const & rhs)
{
	if( this->fmaxlevel == rhs.fmaxlevel)
	{
		this->fcurrentlevel=rhs.fcurrentlevel;
		for( int i=0; i<this->fmaxlevel; i++ )
		{
			this->path[i]=rhs.path[i];
		}
	}
	return *this;
}

inline
void VolumePath::GetGlobalMatrixFromPath( TransformationMatrix * m ) const
{
	m->SetToIdentity();
	for(int i=0; i<fcurrentlevel; i++ )
	{
		m->Multiply( path[i]->getMatrix() );
	}
}

inline
void VolumePath::GetGlobalMatrixFromPath( FastTransformationMatrix * m ) const
{
	m->SetToIdentity();
	for(int i=0; i<fcurrentlevel; i++ )
	{
		m->Multiply<1,-1>( path[i]->getFastMatrix() );
	}
}


inline
PhysicalVolume const * VolumePath::At(int i) const
{
	return path[i];
}




inline
void
VolumePath::SetAt(int i, PhysicalVolume const * v)
{
	path[i]=v;
}

inline
void
VolumePath::Pop()
{
	if( fcurrentlevel > 0 ) path[fcurrentlevel-1]=0;
	fcurrentlevel--;
}

inline
void
VolumePath::Push( PhysicalVolume const * v)
{
	assert( fcurrentlevel < fmaxlevel );

	path[fcurrentlevel]=v;
	fcurrentlevel++;
}


inline
PhysicalVolume const *
VolumePath::Top() const
{
	return ( fcurrentlevel > 0 )? path[fcurrentlevel-1] : 0;
}

inline
void
VolumePath::Clear()
{
	for(int i=0;i<fmaxlevel;i++) path[0]=0;
	fcurrentlevel=0;
}


class SimpleVecNavigator {
private:
	double * workspace;
	// for transformed points and dirs
	Vectors3DSOA transformedpoints;
	Vectors3DSOA transformeddirs;

	PhysicalVolume const * top;

public:
	SimpleVecNavigator(int, PhysicalVolume const *);
	SimpleVecNavigator(int);
	virtual ~SimpleVecNavigator();


	void
	DistToNextBoundary( PhysicalVolume const *, Vectors3DSOA const & /*points*/,
												Vectors3DSOA const & /*dirs*/,
												double const * /*steps*/,
												double * /*distance*/,
			    			    			    			const PhysicalVolume ** nextnode, int np ) const;

	void
	DistToNextBoundaryUsingUnplacedVolumes( PhysicalVolume const *, Vectors3DSOA const & /*points*/,
												Vectors3DSOA const & /*dirs*/,
												double const * /*steps*/,
												double * /*distance*/,
												const PhysicalVolume ** nextnode, int np );

	void
	DistToNextBoundaryUsingUnplacedVolumesButSpecializedMatrices( PhysicalVolume const *, Vectors3DSOA const & /*points*/,
													Vectors3DSOA const & /*dirs*/,
													double const * /*steps*/,
													double * /*distance*/,
													const PhysicalVolume ** nextnode, int np );


	void
	DistToNextBoundaryUsingROOT( PhysicalVolume const *,
								 double const * /*points*/,
								 double const * /*dirs*/,
								 double const * /*steps*/,
								 double * /*distance*/,
								 const PhysicalVolume ** nextnode, int np ) const;

	void
	DistToNextBoundaryUsingUSOLIDS( PhysicalVolume const *, Vectors3DSOA const & /*points*/,
									Vectors3DSOA const & /*dirs*/,
									double const * /*steps*/,
									double * /*distance*/,
									const PhysicalVolume ** nextnode, int np ) const;



	PhysicalVolume const *
	LocatePoint(PhysicalVolume const *, Vector3D const & globalpoint, Vector3D & localpoint, VolumePath &path, TransformationMatrix *, bool top=true) const;

	/* a non-recursive version */
	inline
	__attribute__((always_inline))
	PhysicalVolume const *
	LocatePoint_iterative(PhysicalVolume const *, Vector3D const & globalpoint, Vector3D & localpoint, VolumePath &path, TransformationMatrix *, bool top=true) const;

	/* a non-recursive version */
	inline
	__attribute__((always_inline))
	PhysicalVolume const *
	LocatePoint_iterative(PhysicalVolume const *, Vector3DFast const & globalpoint, Vector3DFast & localpoint, VolumePath &path, FastTransformationMatrix *, bool top=true) const;


	PhysicalVolume const *
	LocatePoint(PhysicalVolume const *, Vector3D const & globalpoint, Vector3D & localpoint, VolumePath &path, bool top=true) const;

	/*
	PhysicalVolume const *
	// this location starts from the a localpoint in the reference frame of inpath.Top() to find the new location ( if any )
	// we might need some more input here ( like the direction )
	LocateLocatePointFromPath(Vector3D const & point, Vector3D &,  VolumePath const & inpath, VolumePath & newpath, TransformationMatrix * ) const;
	*/

	PhysicalVolume const *
	// this location starts from the a localpoint in the reference frame of inpath.Top() to find the new location ( if any )
	// we might need some more input here ( like the direction )
	LocateLocalPointFromPath_Relative(Vector3D const & point, Vector3D & localpoint, VolumePath & path, TransformationMatrix * ) const;


	PhysicalVolume const *
	// this location starts from the a localpoint in the reference frame of inpath.Top() to find the new location ( if any )
	// we might need some more input here ( like the direction )
	LocateLocalPointFromPath_Relative_Iterative(Vector3D const & point, Vector3D & localpoint, VolumePath & path, TransformationMatrix * ) const;


	PhysicalVolume const *
	// this location starts from the a localpoint in the reference frame of inpath.Top() to find the new location ( if any )
	// we might need some more input here ( like the direction )
	LocateLocalPointFromPath_Relative_Iterative(Vector3DFast const & point, Vector3DFast & localpoint,
			VolumePath & path, FastTransformationMatrix *) const;


	// PhysicalVolume const *
	// this variant applies if we know already that we are going down ( because next boundary is daughter volume )
	// LocatePointFromPath_RelativeDown(...);

	// PhysicalVolume const *
	// this variant applies if we know already that we are going up ( because next boundary is current volume itself )
	// NOTE: HERE WE COULD SPECIALIZE FOR DIVISIONS OR REGULAR CELLS
	// LocatePointFromPath_RelativeUp(...);

	// scalar interface
	void FindNextBoundaryAndStep(
			TransformationMatrix * /* globalm */,
			Vector3D const & /*point*/,
			Vector3D const & /*dir*/,
			VolumePath const & /*inpath*/,
			VolumePath & /*outpath*/,
			Vector3D & /*newpoint*/,
			double & ) const;

	void FindNextBoundaryAndStep_iterative(
				TransformationMatrix * /* globalm */,
				Vector3D const & /*point*/,
				Vector3D const & /*dir*/,
				VolumePath const & /*inpath*/,
				VolumePath & /*outpath*/,
				Vector3D & /*newpoint*/,
				double & ) const;

	// new version using fast vectors
	void FindNextBoundaryAndStep_iterative(
					FastTransformationMatrix * /* globalm */,
					Vector3DFast const & /*point*/,
					Vector3DFast const & /*dir*/,
					VolumePath const & /*inpath*/,
					VolumePath & /*outpath*/,
					Vector3DFast & /*newpoint*/,
					double & ) const;


	// vector interface
	void FindNextBoundaryAndStep(
			Vectors3DSOA const & /*point*/,
			Vectors3DSOA const & /*dir*/,
			VolumePath const & /*inpath*/,
			VolumePath & /*outpath*/,
			double /* pstep */,
			double & /* safety */,
			double & /* distance */) const {};

};

inline
PhysicalVolume const * SimpleVecNavigator::LocatePoint_iterative(PhysicalVolume const * vol,
		Vector3D const & point, Vector3D & localpoint, VolumePath & path, TransformationMatrix * globalm, bool top) const
{
	PhysicalVolume const * candvolume = vol;
	Vector3D tmp=point;

	if( top ) candvolume = ( vol->UnplacedContains( point ) )? vol : 0;

	if( candvolume )
	{
		path.Push( candvolume );
		PhysicalVolume::DaughterContainer_t const * dlist = candvolume->GetDaughters();
		PhysicalVolume::DaughterContainerConstIterator_t iter;
		bool godeeper=true;
		while( godeeper && dlist->size() > 0 )
		{
			godeeper=false;
			for(iter=dlist->begin(); iter!=dlist->end(); iter++)
			{
				PhysicalVolume const * nextvol=(*iter);
				Vector3D transformedpoint;
				if(nextvol->Contains(tmp, transformedpoint, globalm))
				{
					path.Push( nextvol );
					// this is no longer the top ( so setting top to false )
					tmp = transformedpoint;
					candvolume = nextvol;
					// we have found volume in this hierarchy, can go deeper and override daughterlist
					dlist = candvolume->GetDaughters();
					godeeper=true;
					break;
				}
			}
		}
	}
	// set localpoint
	localpoint=tmp;
	return candvolume;
}

inline
PhysicalVolume const * SimpleVecNavigator::LocatePoint_iterative(PhysicalVolume const * vol,
		Vector3DFast const & point, Vector3DFast & localpoint, VolumePath & path, FastTransformationMatrix * globalm, bool top) const
{
	PhysicalVolume const * candvolume = vol;
	Vector3DFast tmp=point;

	if( top ) candvolume = ( vol->UnplacedContains( point ) )? vol : 0;

	if( candvolume )
	{
		path.Push( candvolume );
		PhysicalVolume::DaughterContainer_t const * dlist = candvolume->GetDaughters();
		PhysicalVolume::DaughterContainerConstIterator_t iter;
		bool godeeper=true;
		while( godeeper && dlist->size() > 0 )
		{
			godeeper=false;
			for(iter=dlist->begin(); iter!=dlist->end(); iter++)
			{
				PhysicalVolume const * nextvol=(*iter);
				Vector3DFast transformedpoint;
				if(nextvol->Contains(tmp, transformedpoint, globalm))
				{
					path.Push( nextvol );
					// this is no longer the top ( so setting top to false )
					tmp = transformedpoint;
					candvolume = nextvol;
					// we have found volume in this hierarchy, can go deeper and override daughterlist
					dlist = candvolume->GetDaughters();
					godeeper=true;
					break;
				}
			}
		}
	}
	// set localpoint
	localpoint=tmp;
	return candvolume;
}

#endif /* SIMPLEVECNAVIGATOR_H_ */
