#ifndef VECGEOM_BASE_TYPES_H_
#define VECGEOM_BASE_TYPES_H_

namespace vecgeom {

#ifdef VECGEOM_FLOAT_PRECISION
typedef float Precision;
#else
typedef double Precision;
#endif

enum ImplType { kVc, kCuda, kScalar, kCilk };

template <ImplType it>
struct Impl;

template <typename Type>
class Vector3D;

template <typename Type>
class SOA3D;

template <typename Type>
class Container;

template <typename Type>
class Vector;

template <typename Type>
class Array;

class LogicalVolume;

class VPlacedVolume;

typedef VPlacedVolume VUSolid;

class VUnplacedVolume;

class TransformationMatrix;

class GeoManager;

#ifdef VECGEOM_CUDA
class CudaManager;
#endif

} // End namespace vecgeom

#ifdef VECGEOM_COMPARISON
class TGeoShape;
class VUSolid;
#endif

#endif // VECGEOM_BASE_TYPES_H_