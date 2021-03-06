#include <algorithm>
#include <cassert>
#include "backend/cuda_backend.cuh"
#include "base/array.h"
#include "management/cuda_manager.h"
#include "management/volume_factory.h"

namespace vecgeom {

CudaManager::CudaManager() {
  synchronized = true;
  world_ = NULL;
  world_gpu_ = NULL;
  verbose = 0;
  total_volumes = 0;
}

LogicalVolume const* CudaManager::world() const {
  assert(world_ != NULL);
  return world_;
}

LogicalVolume const* CudaManager::world_gpu() const {
  assert(world_gpu_ != NULL);
  return world_gpu_;
}

LogicalVolume const* CudaManager::Synchronize() {

  if (verbose > 0) std::cerr << "Starting synchronization to GPU.\n";

  // Will return null if no geometry is loaded
  if (synchronized) return world_gpu_;

  CleanGpu();

  // Populate the memory map with GPU addresses

  AllocateGeometry();

  // Create new objects with pointers adjusted to point to GPU memory, then
  // copy them to the allocated memory locations on the GPU.

  if (verbose > 1) std::cerr << "Copying geometry to GPU...";

  if (verbose > 2) std::cerr << "\nCopying logical volumes...";
  for (std::set<LogicalVolume const*>::const_iterator i =
       logical_volumes.begin(); i != logical_volumes.end(); ++i) {

    (*i)->CopyToGpu(
      LookupUnplaced((*i)->unplaced_volume()),
      LookupDaughters((*i)->daughters_),
      LookupLogical(*i)
    );

  }
  if (verbose > 2) std::cerr << " OK\n";

  if (verbose > 2) std::cerr << "Copying unplaced volumes...";
  for (std::set<VUnplacedVolume const*>::const_iterator i =
       unplaced_volumes.begin(); i != unplaced_volumes.end(); ++i) {

    (*i)->CopyToGpu(LookupUnplaced(*i));

  }
  if (verbose > 2) std::cout << " OK\n";

  if (verbose > 2) std::cout << "Copying placed volumes...";
  for (std::set<VPlacedVolume const*>::const_iterator i =
       placed_volumes.begin(); i != placed_volumes.end(); ++i) {

    (*i)->CopyToGpu(
      LookupLogical((*i)->logical_volume()),
      LookupMatrix((*i)->matrix()),
      LookupPlaced(*i)
    );

  }
  if (verbose > 2) std::cout << " OK\n";

  if (verbose > 2) std::cout << "Copying transformation matrices...";
  for (std::set<TransformationMatrix const*>::const_iterator i =
       matrices.begin(); i != matrices.end(); ++i) {

    (*i)->CopyToGpu(LookupMatrix(*i));

  }
  if (verbose > 2) std::cout << " OK\n";

  if (verbose > 2) std::cout << "Copying daughter arrays...";
  for (std::set<Container<Daughter> *>::const_iterator i =
       daughters.begin(); i != daughters.end(); ++i) {

    // First handle C arrays that must now point to GPU locations
    const int daughter_count = (*i)->size();
    Daughter *const daughter_array = new Daughter[daughter_count];
    int j = 0;
    for (Iterator<Daughter> k = (*i)->begin(); k != (*i)->end(); ++k) {
      daughter_array[j] = LookupPlaced(*k);
      j++;
    }
    vecgeom::CopyToGpu(
      daughter_array, LookupDaughterArray(*i), daughter_count*sizeof(Daughter)
    );

    // Create array object wrapping newly copied C arrays
    (*i)->CopyToGpu(LookupDaughterArray(*i), LookupDaughters(*i));

  }
  if (verbose > 1) std::cout << " OK\n";

  synchronized = true;

  world_gpu_ = LookupLogical(world_);

  if (verbose > 0) std::cout << "Geometry synchronized to GPU.\n";

  return world_gpu_;

}

void CudaManager::LoadGeometry(LogicalVolume const *const volume) {

  CleanGpu();

  logical_volumes.clear();
  unplaced_volumes.clear();
  placed_volumes.clear();
  matrices.clear();
  daughters.clear();

  world_ = volume;

  ScanGeometry(volume);

  // Already set by CleanGpu(), but keep it here for good measure
  synchronized = false;

}

void CudaManager::CleanGpu() {

  if (memory_map.size() == 0 && world_gpu_ == NULL) return;

  if (verbose > 1) std::cout << "Cleaning GPU...";

  for (MemoryMap::iterator i = memory_map.begin(); i != memory_map.end(); ++i) {
    FreeFromGpu(i->second);
  }
  memory_map.clear();

  world_gpu_ = NULL;
  synchronized = false;

  if (verbose > 1) std::cout << " OK\n";

}

void CudaManager::AllocateGeometry() {

  if (verbose > 1) std::cout << "Allocating geometry on GPU...";

  if (verbose > 2) {
    size_t free_memory = 0, total_memory = 0;
    CudaAssertError(cudaMemGetInfo(&free_memory, &total_memory));
    std::cout << "\nAvailable memory: " << free_memory << " / "
                                        << total_memory << std::endl;
  }

  {
    if (verbose > 2) std::cout << "Allocating logical volumes...";

    LogicalVolume *gpu_array =
        AllocateOnGpu<LogicalVolume>(
          logical_volumes.size()*sizeof(LogicalVolume)
        );

    for (std::set<LogicalVolume const*>::const_iterator i =
         logical_volumes.begin(); i != logical_volumes.end(); ++i) {

      memory_map[ToCpuAddress(*i)] = ToGpuAddress(gpu_array);
      gpu_array++;

    }

    if (verbose > 2) std::cout << " OK\n";
  }

  {
    if (verbose > 2) std::cout << "Allocating unplaced volumes...";

    for (std::set<VUnplacedVolume const*>::const_iterator i =
         unplaced_volumes.begin(); i != unplaced_volumes.end(); ++i) {

      const GpuAddress gpu_address =
          AllocateOnGpu<GpuAddress*>((*i)->memory_size());
      memory_map[ToCpuAddress(*i)] = gpu_address;

    }

    if (verbose > 2) std::cout << " OK\n";
  }

  {
    if (verbose > 2) std::cout << "Allocating placed volumes...";

    for (std::set<VPlacedVolume const*>::const_iterator i =
         placed_volumes.begin(); i != placed_volumes.end(); ++i) {

      const GpuAddress gpu_address =
          AllocateOnGpu<GpuAddress*>((*i)->memory_size());
      memory_map[ToCpuAddress(*i)] = gpu_address;

    }

    if (verbose > 2) std::cout << " OK\n";
  }

  {
    if (verbose > 2) std::cout << "Allocating transformation matrices...";

    for (std::set<TransformationMatrix const*>::const_iterator i =
         matrices.begin(); i != matrices.end(); ++i) {

      const GpuAddress gpu_address =
          AllocateOnGpu<TransformationMatrix>((*i)->memory_size());
      memory_map[ToCpuAddress(*i)] = ToGpuAddress(gpu_address);

    }

    if (verbose > 2) std::cout << " OK\n";
  }

  {
    if (verbose > 2) std::cout << "Allocating daughter lists...";

    Array<Daughter> *gpu_array =
        AllocateOnGpu<Array<Daughter> >(
          daughters.size()*sizeof(Array<Daughter>)
        );

    Daughter *gpu_c_array =
        AllocateOnGpu<Daughter>(total_volumes*sizeof(Daughter));

    for (std::set<Container<Daughter> *>::const_iterator i =
         daughters.begin(); i != daughters.end(); ++i) {

      memory_map[ToCpuAddress(*i)] = ToGpuAddress(gpu_array);
      memory_map[ToCpuAddress(gpu_array)] = ToGpuAddress(gpu_c_array);
      gpu_array++;
      gpu_c_array += (*i)->size();

    }

    if (verbose > 2) std::cout << " OK\n";
  }

  if (verbose == 2) std::cout << " OK\n";

}

void CudaManager::ScanGeometry(LogicalVolume const *const volume) {

  if (logical_volumes.find(volume) == logical_volumes.end()) {
    logical_volumes.insert(volume);
  }
  if (unplaced_volumes.find(volume->unplaced_volume_)
      == unplaced_volumes.end()) {
    unplaced_volumes.insert(volume->unplaced_volume_);
  }
  if (daughters.find(volume->daughters_) == daughters.end()) {
    daughters.insert(volume->daughters_);
  }
  for (Iterator<Daughter> i = volume->daughters().begin();
       i != volume->daughters().end(); ++i) {
    if (placed_volumes.find(*i) == placed_volumes.end()) {
      placed_volumes.insert(*i);
    }
    if (matrices.find((*i)->matrix_) == matrices.end()) {
      matrices.insert((*i)->matrix_);
    }
    ScanGeometry((*i)->logical_volume());
  }

  total_volumes++;
}

void CudaManager::PrintContent() const {
  std::cout << "-- Logical volumes with daughters:\n";
  for (std::set<LogicalVolume const*>::const_iterator i =
       logical_volumes.begin(); i != logical_volumes.end(); ++i) {
    std::cout << (**i);
  }
  std::cout << "-- Unplaced volumes:\n";
  for (std::set<VUnplacedVolume const*>::const_iterator i =
       unplaced_volumes.begin(); i != unplaced_volumes.end(); ++i) {
    std::cout << (**i) << std::endl;
  }
  std::cout << "-- Placed volumes:\n";
  for (std::set<VPlacedVolume const*>::const_iterator i =
       placed_volumes.begin(); i != placed_volumes.end(); ++i) {
    std::cout << (**i) << std::endl;
  }
  std::cout << "-- Transformation matrices:\n";
  for (std::set<TransformationMatrix const*>::const_iterator i =
       matrices.begin(); i != matrices.end(); ++i) {
    std::cout << (**i) << std::endl;
  }
}

template <typename Type>
typename CudaManager::GpuAddress CudaManager::Lookup(
    Type const *const key) {
  const CpuAddress cpu_address = ToCpuAddress(key);
  GpuAddress output = memory_map[cpu_address];
  assert(output != NULL);
  return output;
}

VUnplacedVolume* CudaManager::LookupUnplaced(
    VUnplacedVolume const *const host_ptr) {
  return static_cast<VUnplacedVolume*>(Lookup(host_ptr));
}

LogicalVolume* CudaManager::LookupLogical(
    LogicalVolume const *const host_ptr) {
  return static_cast<LogicalVolume*>(Lookup(host_ptr));
}

VPlacedVolume* CudaManager::LookupPlaced(
    VPlacedVolume const *const host_ptr) {
  return static_cast<VPlacedVolume*>(Lookup(host_ptr));
}

TransformationMatrix* CudaManager::LookupMatrix(
    TransformationMatrix const *const host_ptr) {
  return static_cast<TransformationMatrix*>(Lookup(host_ptr));
}

Array<Daughter>* CudaManager::LookupDaughters(
    Container<Daughter> *const host_ptr) {
  return static_cast<Array<Daughter>*>(Lookup(host_ptr));
}

Daughter* CudaManager::LookupDaughterArray(
    Container<Daughter> *const host_ptr) {
  Array<Daughter> const *const daughters = LookupDaughters(host_ptr);
  return static_cast<Daughter*>(Lookup(daughters));
}

} // End namespace vecgeom