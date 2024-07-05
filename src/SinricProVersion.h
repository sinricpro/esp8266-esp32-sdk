#pragma once

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Version Configuration
#define SINRICPRO_VERSION_MAJOR     4
#define SINRICPRO_VERSION_MINOR     0
#define SINRICPRO_VERSION_REVISION  0
#define SINRICPRO_VERSION STR(SINRICPRO_VERSION_MAJOR) "." STR(SINRICPRO_VERSION_MINOR) "." STR(SINRICPRO_VERSION_REVISION)
#define SINRICPRO_VERSION_STR "SinricPro (v" SINRICPRO_VERSION ")"
#define SINRICPRO_VERISON_INT SINRICPRO_VERSION_MAJOR * 1000000 + SINRICPRO_VERSION_MINOR * 1000 + SINRICPRO_VERSION_REVISION