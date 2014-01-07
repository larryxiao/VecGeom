#include "LibraryCuda.cuh"
#include "Kernel.h"
#include "Box.h"

namespace kernel {
namespace box {

__global__
void ContainsWrapper(const Vector3D<CudaFloat> box_pos,
                     const Vector3D<CudaFloat> box_dim,
                     SOA3D<double> const points, bool *output) {

  const int index = ThreadIndex();
  if (index >= points.size()) return;
  output[index] = Contains<kCuda>(box_pos, box_dim,
                                  VectorAsFloatDevice(points[index]));
}

__global__
void DistanceToInWrapper(const Vector3D<CudaFloat> dimensions,
                         TransMatrix<float> const * const trans_matrix,
                         SOA3D<double> const pos, SOA3D<double> const dir,
                         double *distance) {

  const int index = ThreadIndex();
  if (index >= pos.size()) return;
  distance[index] = DistanceToIn<kCuda>(dimensions, trans_matrix,
                                        VectorAsFloatDevice(pos[index]),
                                        VectorAsFloatDevice(dir[index]));
}

} // End namespace box
} // End namespace kernel

template <>
void Box::Contains<kCuda>(SOA3D<double> const &points, bool *output) const {

  const int blocks_per_grid = BlocksPerGrid(points.size());
  kernel::box::ContainsWrapper<<<threads_per_block, blocks_per_grid>>>(
    VectorAsFloatHost(trans_matrix->Translation()),
    VectorAsFloatHost(dimensions),
    points,
    output
  );

}

template <>
void Box::DistanceToIn<kVc>(SOA3D<double> const &pos, SOA3D<double> const &dir,
                            double *distance) const {

  const int blocks_per_grid = BlocksPerGrid(pos.size());
  kernel::box::DistanceToInWrapper<<<threads_per_block, blocks_per_grid>>>(
    VectorAsFloatHost(dimensions),
    trans_matrix_cuda,
    pos,
    dir,
    distance
  );

}