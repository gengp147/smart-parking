
set(LIB_NAME ai_sl)

file(GLOB_RECURSE TRACK_SOURCES_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/track/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/track/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/track/*.h)

file(GLOB_RECURSE INOUT_PARKING_SOURCES_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/veh_in_out/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/veh_in_out/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/veh_in_out/*.h)

set(SPD_SDK_SRC
        interfaces/sl_spd_sdk.cc

        utils/argument_parser.cc
        utils/argument_parser.h

        models_processor/char_code.h
        models_processor/lp_detection.cpp
        models_processor/lp_detection.h
        models_processor/lp_recognition.cpp
        models_processor/lp_recognition.h
        models_processor/model_process_base.cpp
        models_processor/model_process_base.h
        models_processor/reid.cc
        models_processor/reid.h
        models_processor/vehicle_detection.cc
        models_processor/vehicle_detection.h


        utils/utils.cpp
        utils/utils.h
        utils/sl_logger.cc
        utils/sl_logger.h
        utils/struct_2_string.cc
        utils/struct_2_string.h
        utils/struct_2_stream.cc
        utils/struct_2_stream.h
        utils/function_timer.h
        utils/base64.cpp
        utils/base64.h

        workflow/workflow.cc
        workflow/workflow.h
        workflow/veh_det_module.cc
        workflow/veh_det_module.h
        workflow/vlp_rec_module.cc
        workflow/vlp_rec_module.h
        workflow/vlp_det_module.cc
        workflow/vlp_det_module.h
        workflow/veh_reid_module.cc
        workflow/veh_reid_module.h
        workflow/access_module.cc
        workflow/access_module.h
        workflow/mot_module.cc
        workflow/mot_module.h
        workflow/inout_module.cc
        workflow/inout_module.h
        workflow/illegal_parking_module.cc
        workflow/illegal_parking_module.h
        workflow/private_data_module.h

        managers/project_manager.cc
        managers/project_manager.h
        managers/device_manager.cc
        managers/device_manager.h
        managers/param_manager.cc
        managers/param_manager.h
        managers/cached_frames_manager.h
        managers/cached_frames_manager.cc
        managers/private_data_manager.h
        managers/private_data_manager.cc
        managers/aih_eventinfo_convert.cc
        managers/aih_eventinfo_convert.h

        access/access_detector.cc
        access/access_detector.h

        image_processor/image_ive_common.h
        image_processor/image_process.cc
        image_processor/image_process.h
#
        illegal_parking/awry_parking.cc
        illegal_parking/illegal_parking_algorithm.cc
        illegal_parking/illegal_parking_algorithm.h
        illegal_parking/capture_record_container.cc
        illegal_parking/capture_record_container.h
        illegal_parking/no_time_parking.cc
        illegal_parking/illegal_history_record.cc
        illegal_parking/illegal_history_record.h
        illegal_parking/cross_space_check.cc
        illegal_parking/no_parking_area.cc

        ${TRACK_SOURCES_LIST}
        ${INOUT_PARKING_SOURCES_LIST}
        workflow/private_data_module.h
        utils/detections_save.h
)



add_library(${LIB_NAME} SHARED ${SPD_SDK_SRC})
target_compile_options(${LIB_NAME} PRIVATE
        -fvisibility=hidden
)
set(VERSION_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/ai_si_export.map)
#target_link_options(${LIB_NAME} PRIVATE
#        "-Wl,--version-script=${VERSION_SCRIPT}"
#)
set_target_properties(${LIB_NAME} PROPERTIES
        LINK_FLAGS "-Wl,--version-script=${VERSION_SCRIPT}"
)
target_compile_features(${LIB_NAME} PRIVATE cxx_std_11)
message(STATUS " CMAKE_SOURCE_DIR： ${CMAKE_SOURCE_DIR}")
target_include_directories(${LIB_NAME} PUBLIC
        $ENV{DDK_PATH}/acllib/include/
        /usr/local/Ascend/ascend-toolkit/svp_latest/acllib/include/
#        /usr/local/Ascend/ascend-toolkit/svp_latest/atc/third_party/opencv/include/opencv4
        ${CMAKE_SOURCE_DIR}/third_party/glibc/opencv/include/opencv4
        ${CMAKE_SOURCE_DIR}/third_party/glibc/spdlog/include
        ${CMAKE_SOURCE_DIR}/third_party/glibc/vulcan/include
        ${CMAKE_SOURCE_DIR}/third_party/glibc/jsoncpp/include
        ${CMAKE_SOURCE_DIR}/third_party/glibc/hisi_sdk/inc
        ${CMAKE_SOURCE_DIR}/third_party/acl
        ${CMAKE_SOURCE_DIR}/third_party/
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
if (target MATCHES "Simulator_Function")
    target_link_libraries(${LIB_NAME} funcsim)
elseif (target MATCHES "Simulator_Instruction" OR target MATCHES "Simulator_Performance")
    target_link_libraries(${LIB_NAME} instsim)
else()
    target_link_libraries(${LIB_NAME} svp_acl ss_mpi_km pthread securec dl protobuf-c stdc++ m
            vulcan
            jsoncpp
            sonlilog
    )
    set(OPENCV_LIB ${CMAKE_SOURCE_DIR}/third_party/glibc/opencv/lib/lib412)
    set(SS_IVE_LIB ${CMAKE_SOURCE_DIR}/third_party/glibc/hisi_sdk/inc/lib)
    target_link_libraries(${LIB_NAME}
            ${OPENCV_LIB}/libopencv_core.so.412
            ${OPENCV_LIB}/libopencv_imgcodecs.so.412
            ${OPENCV_LIB}/libopencv_imgproc.so.412
            ${OPENCV_LIB}/libopencv_video.so.412

            ${OPENCV_LIB}/libopencv_calib3d.so.412
            ${OPENCV_LIB}/libopencv_dnn.so.412
            ${OPENCV_LIB}/libopencv_features2d.so.412
            ${OPENCV_LIB}/libopencv_flann.so.412
            ${SS_IVE_LIB}/libss_mpi.so
            ${SS_IVE_LIB}/libss_mpi_isp.so

            ${SS_IVE_LIB}/libot_mpi_isp.so
            ${SS_IVE_LIB}/libcalcflicker.so

            ${SS_IVE_LIB}/libss_mpi_ive.so
                    ${SS_IVE_LIB}/libacs.so
                    ${SS_IVE_LIB}/libldci.so

            ${SS_IVE_LIB}/libdehaze.so
                    ${SS_IVE_LIB}/libdrc.so
                    ${SS_IVE_LIB}/libir_auto.so
                    ${SS_IVE_LIB}/libfileformat.so
                    ${SS_IVE_LIB}/libextend_stats.so

            ${SS_IVE_LIB}/libheif.so
            ${SS_IVE_LIB}/libss_mpi_ae.so
            ${SS_IVE_LIB}/libss_mpi_cipher.so
            ${SS_IVE_LIB}/libss_mpi_isp.so
            ${SS_IVE_LIB}/libss_mpi_km.so
            ${SS_IVE_LIB}/libss_mpi_otp.so
            ${SS_IVE_LIB}/libss_mpi_sysbind.so
            ${SS_IVE_LIB}/libss_mpi_sysmem.so
            ${SS_IVE_LIB}/libbnr.so
    )


endif ()


