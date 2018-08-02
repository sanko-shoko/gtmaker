# SIMPLESP_FOUND
# SIMPLESP_INCLUDE_DIR
#

find_path (SIMPLESP_DIR NAMES simplesp.h
    PATHS
    	"${SIMPLESP_ROOT_DIR}"
    	/usr/
    	/usr/local/
)
set(SIMPLESP_INCLUDE_DIR ${SIMPLESP_DIR})

