# ####################################################################
# # PhysX may be used as an alternative engine for better simulation ##
# # performance and GPU hardware acceleration if cuda is available.  ##
# #####################################################################

FetchContent_Declare(
    PhysX
    GIT_REPOSITORY "https://github.com/NVIDIA-Omniverse/PhysX.git"
    GIT_TAG "106.5-physx-5.5.1"
)
FetchContent_GetProperties(PhysX)

if(NOT physx_POPULATED)
    FetchContent_Populate(PhysX)
    message(WARNING "PhysX source dir: ${physx_SOURCE_DIR}")
    message(WARNING "PhysX binary dir: ${physx_BINARY_DIR}")

    # todo: figure out this doesn't work
    # We need to call this script so that packman can download NVidia's proprietary PhysXgpu shared binary
    execute_process(
        WORKING_DIRECTORY "${physx_SOURCE_DIR}/physx/"
        COMMAND "yes 1 | generate_projects.sh"
        COMMAND_ECHO STDOUT)

    # Hardcoded for now..
    set(PHYSX_ROOT_DIR ${physx_SOURCE_DIR}/physx/)
    set(TARGET_BUILD_PLATFORM "linux")

    # Dirty hack, please look away just for this one
    macro(physx_workaround VAR access value current_list_file stack)
        if("${access}" STREQUAL "MODIFIED_ACCESS")
            set(${VAR} 0)
        endif()
    endmacro()

    variable_watch(PUBLIC_RELEASE physx_workaround)

    set(PX_GENERATE_SOURCE_DISTRO 1)
    set(PX_GENERATE_STATIC_LIBRARIES 1)
    set(PX_BUILDPVDRUNTIME 1)
    set(PX_BUILDSNIPPETS 0) # No we don't want any fancy demos :c
    set(PX_SUPPORT_GPU_PHYSX ON)

    set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})

    add_subdirectory(${physx_SOURCE_DIR}/physx/compiler/public/ ${physx_BINARY_DIR}/physx/)
endif()

link_directories(${physx_SOURCE_DIR}/physx/bin/linux.x86_64/release)