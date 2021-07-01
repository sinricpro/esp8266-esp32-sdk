#pragma once

#include "SinricProVersion.h"

#define DO_CONCATE(A, B, C, D) A ## _ ## B ## _ ## C ## _ ## D
#define CONCATE(A, B, C, D) DO_CONCATE(A, B, C, D)

#define SINRICPRO_NAMESPACE CONCATE(SINRICPRO, SINRICPRO_VERSION_MAJOR, SINRICPRO_VERSION_MINOR, SINRICPRO_VERSION_REVISION)
#define NAMESPACE_STR STR(SINRICPRO_NAMESPACE)


/**
 * @brief SinricPro namespace
 * 
 */
namespace SINRICPRO_NAMESPACE {}