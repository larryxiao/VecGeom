#ifndef VECGEOM_MANAGEMENT_VOLUMEFACTORY_H_
#define VECGEOM_MANAGEMENT_VOLUMEFACTORY_H_

#include "base/transformation_matrix.h"
#include "volumes/logical_volume.h"
#include "volumes/placed_volume.h"

namespace vecgeom {

class VolumeFactory {

public:

  static VolumeFactory& Instance() {
    static VolumeFactory instance;
    return instance;
  }

  /**
   * Middle templated function call which dispatches specialization based on
   * transformation.
   */
  template<typename VolumeType>
  VPlacedVolume* CreateByTransformation(
      LogicalVolume const *const logical_volume,
      TransformationMatrix const *const matrix,
      const TranslationCode trans_code, const RotationCode rot_code) const;

private:

  VolumeFactory() {}
  VolumeFactory(VolumeFactory const&);
  VolumeFactory& operator=(VolumeFactory const&);

};

template<typename VolumeType>
VPlacedVolume* VolumeFactory::CreateByTransformation(
    LogicalVolume const *const logical_volume,
    TransformationMatrix const *const matrix,
    const TranslationCode trans_code, const RotationCode rot_code) const {

  if (trans_code == 0 && rot_code == 0x1b1) {
    return VolumeType::template Create<0, 0x1b1>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x1b1) {
    return VolumeType::template Create<1, 0x1b1>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x18e) {
    return VolumeType::template Create<0, 0x18e>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x18e) {
    return VolumeType::template Create<1, 0x18e>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x076) {
    return VolumeType::template Create<0, 0x076>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x076) {
    return VolumeType::template Create<1, 0x076>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x16a) {
    return VolumeType::template Create<0, 0x16a>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x16a) {
    return VolumeType::template Create<1, 0x16a>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x155) {
    return VolumeType::template Create<0, 0x155>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x155) {
    return VolumeType::template Create<1, 0x155>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x0ad) {
    return VolumeType::template Create<0, 0x0ad>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x0ad) {
    return VolumeType::template Create<1, 0x0ad>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x0dc) {
    return VolumeType::template Create<0, 0x0dc>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x0dc) {
    return VolumeType::template Create<1, 0x0dc>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x0e3) {
    return VolumeType::template Create<0, 0x0e3>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x0e3) {
    return VolumeType::template Create<1, 0x0e3>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x11b) {
    return VolumeType::template Create<0, 0x11b>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x11b) {
    return VolumeType::template Create<1, 0x11b>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x0a1) {
    return VolumeType::template Create<0, 0x0a1>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x0a1) {
    return VolumeType::template Create<1, 0x0a1>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x10a) {
    return VolumeType::template Create<0, 0x10a>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x10a) {
    return VolumeType::template Create<1, 0x10a>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x046) {
    return VolumeType::template Create<0, 0x046>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x046) {
    return VolumeType::template Create<1, 0x046>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x062) {
    return VolumeType::template Create<0, 0x062>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x062) {
    return VolumeType::template Create<1, 0x062>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x054) {
    return VolumeType::template Create<0, 0x054>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x054) {
    return VolumeType::template Create<1, 0x054>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x111) {
    return VolumeType::template Create<0, 0x111>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x111) {
    return VolumeType::template Create<1, 0x111>(logical_volume, matrix);
  }
  if (trans_code == 0 && rot_code == 0x200) {
    return VolumeType::template Create<0, 0x200>(logical_volume, matrix);
  }
  if (trans_code == 1 && rot_code == 0x200) {
    return VolumeType::template Create<1, 0x200>(logical_volume, matrix);
  }

  // No specialization
  return VolumeType::template Create<1, 0>(logical_volume, matrix);

}

} // End namespace vecgeom

#endif // VECGEOM_MANAGEMENT_VOLUMEFACTORY_H_