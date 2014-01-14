#ifndef LIBRARYCUDA_H
#define LIBRARYCUDA_H

#include "mm_malloc.h"
#include "LibraryGeneric.h"

static const int threads_per_block = 512;

template <>
struct ImplTraits<kCuda> {
  typedef CudaFloat float_t;
  typedef int       int_v;
  typedef CudaFloat float_v;
  typedef bool      bool_v;
  const static bool early_return = true;
  const static bool_v kZero = 0;
  const static bool_v kTrue = true;
  const static bool_v kFalse = false;
};

typedef ImplTraits<kCuda>::int_v  CudaInt;
typedef ImplTraits<kCuda>::bool_v CudaBool;

__host__
inline __attribute__((always_inline))
void CheckCudaError() {
  const cudaError_t err = cudaGetLastError();
  if (err != cudaSuccess) {
    std::cout << "CUDA reported error with message: \""
              << cudaGetErrorString(err) << "\"\n";
    assert(err != cudaSuccess);
  }
}

__device__
inline __attribute__((always_inline))
Vector3D<float> VectorAsFloatDevice(Vector3D<double> const &vec) {
  return Vector3D<float>(__double2float_rd(vec[0]),
                         __double2float_rd(vec[1]),
                         __double2float_rd(vec[2]));
}

__host__
inline __attribute__((always_inline))
Vector3D<float> VectorAsFloatHost(Vector3D<double> const &vec) {
  return Vector3D<float>(float(vec[0]),
                         float(vec[1]),
                         float(vec[2]));
}

__device__
inline __attribute__((always_inline))
int ThreadIndex() {
  return blockIdx.x * blockDim.x + threadIdx.x;
}

__host__
inline __attribute__((always_inline))
int BlocksPerGrid(const int threads) {
  return (threads - 1) / threads_per_block + 1;
}

template <typename Type>
__host__
inline __attribute__((always_inline))
static Type* AllocateOnGPU(const int count) {
  Type *ptr;
  cudaMalloc((void**)&ptr, count*sizeof(Type));
  return ptr;
}

template <typename Type>
__host__
inline __attribute__((always_inline))
void CopyToGPU(Type const * const src, Type * const tgt, const int count) {
  cudaMemcpy(tgt, src, count*sizeof(Type), cudaMemcpyHostToDevice);
}

template <typename Type>
__host__
inline __attribute__((always_inline))
void CopyFromGPU(Type const * const src, Type * const tgt, const int count) {
  cudaMemcpy(tgt, src, count*sizeof(Type), cudaMemcpyDeviceToHost);
}

template <>
CUDA_HEADER_BOTH
inline __attribute__((always_inline))
CudaFloat Abs<kCuda, CudaFloat>(CudaFloat const &val) {
  return abs(val);
}

// Use scalar implementation

// template <typename Type>
// __host__ __device__
// inline __attribute__((always_inline))
// Type CondAssign(const CudaBool &cond,
//                 const Type &thenval, const Type &elseval) {
//   return (cond) ? thenval : elseval;
// }

// template <typename Type>
// __host__ __device__
// inline __attribute__((always_inline))
// void MaskedAssign(const CudaBool &cond,
//                   const Type &thenval, Type &output) {
//   output = (cond) ? thenval : output;
// }

#endif /* LIBRARYCUDA_H */