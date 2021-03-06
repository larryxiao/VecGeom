#ifndef VECGEOM_VOLUMES_UNPLACEDVOLUME_H_
#define VECGEOM_VOLUMES_UNPLACEDVOLUME_H_

#include <string>
#include "base/global.h"
#include "base/transformation_matrix.h"

namespace vecgeom {

class VUnplacedVolume {

private:

  friend class CudaManager;

public:

  /**
   * Uses the virtual print method.
   * \sa print(std::ostream &ps)
   */
  friend std::ostream& operator<<(std::ostream& os, VUnplacedVolume const &vol);

  /**
   * Should return the size of bytes of the deriving class. Necessary for
   * copying to the GPU.
   */
  virtual int memory_size() const =0;

  /**
   * Constructs the deriving class on the GPU and returns a pointer to GPU
   * memory where the object has been instantiated.
   */
  #ifdef VECGEOM_NVCC
  virtual VUnplacedVolume* CopyToGpu() const =0;
  virtual VUnplacedVolume* CopyToGpu(VUnplacedVolume *const gpu_ptr) const =0;
  #endif

  /**
   * C-style printing for CUDA purposes.
   */
  VECGEOM_CUDA_HEADER_BOTH
  virtual void Print() const =0;

  VPlacedVolume* PlaceVolume(
      LogicalVolume const *const volume,
      TransformationMatrix const *const matrix) const;

private:

  /**
   * Print information about the deriving class.
   * \param os Outstream to stream information into.
   */
  virtual void Print(std::ostream &os) const =0;

  virtual VPlacedVolume* SpecializedVolume(
      LogicalVolume const *const volume,
      TransformationMatrix const *const matrix,
      const TranslationCode trans_code, const RotationCode rot_code) const =0;

};

} // End namespace vecgeom

#endif // VECGEOM_VOLUMES_UNPLACEDVOLUME_H_