#pragma once

#include "libMeshReducedNamespace.h"

namespace Salamander
{
struct ParticleData
{
  /// the location where the particle will be placed
  Point position;
  /// the velocity that the particle will be given
  Point velocity;
  /// the number of physical particles that this particle represents
  Real weight;
  /// the charge of the physical particle this computational particle represents
  Real charge;
  /// the mass of the physical particle this computational particle represents
  Real mass;
  /// the element that this particle will be placed into
  const Elem * elem;
};
}
