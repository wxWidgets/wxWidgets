#############################################################################
# Name:        build/cmake/winui-toolkit-probe.cmake
# Purpose:     Opt-in standalone native Toolkit consumption probe
# Licence:     wxWindows licence
#############################################################################

# Include after winui.cmake has defined the normal standalone smoke target.
# Nothing below changes wxcore, the normal projection, the installed package,
# or the runtime payload used by ordinary samples and tests.
option(wxWINUI3_BUILD_TOOLKIT_PROBE
    "Build the experimental, standalone native Toolkit activation probe" OFF)
if(NOT wxWINUI3_BUILD_TOOLKIT_PROBE)
    return()
endif()

if(NOT TARGET wx_winui_runtime_smoke OR NOT MSVC)
    message(FATAL_ERROR "The Toolkit probe requires the MSVC WinUI test build")
endif()
if(NOT wxWINUI3_DEPLOY_RESOURCES_PRI)
    message(FATAL_ERROR "The Toolkit probe requires generated resource deployment")
endif()

function(wx_winui3_add_toolkit_probe)
    # Exact archive hashes recorded by the read-only feasibility inspection.
    # These packages are experimental components, not a production dependency.
    set(toolkit_version "0.8.0-alpha")
    set(toolkit_pins
        "XamlToolkit.Labs.WinUI|A6B345F647D99931E93FBD20A555697D939642499BDA88D128FE797F69086ED2"
        "XamlToolkit.WinUI|76A6D0A2ABCB7D9F44511ECD0E62C82EFB25FC6BC079732FB11DA6AA2D9B49DF"
        "XamlToolkit.WinUI.Converters|D4593EC6564A66FBCF3654730B85A92AFE7AE395C3ED398AC7AE1EDE016CC7F5"
        "XamlToolkit.WinUI.Controls|82D0D3B8034FDDE4F3036B71C4F6F9DC3DB1D218C31F030BF8389BC7C93C52DE"
        "XamlToolkit.WinUI.Helpers|2A0CED054FF4337872963D0A6E5CEE568808CA53A9E9C1C148EE2FF166B185F3"
        "XamlToolkit.WinUI.Animations|FE3F7299E55CEBF07368FAEAE7C52F7580F7FC0EC99FECD9CD18F63A57B47AC8"
        "XamlToolkit.WinUI.Interactivity|3733AC07DDFF57EE76A569AED76F9803BD2DAAF0301EDF8897C349B97CFD8C65")
    set(extra_metadata)
    set(extra_payload)
    set(extra_notices)
    set(probe_inventory "scope=experimental-consumption-not-production\n")
    set(base_pri_stage "${wxWINUI3_PAYLOAD_STAGE_DIR}")

    foreach(pin IN LISTS toolkit_pins)
        string(REPLACE "|" ";" fields "${pin}")
        list(GET fields 0 component)
        list(GET fields 1 archive_hash)
        set(package "${component}.Native")
        wx_winui3_find_nuget_package(package_dir "${package}"
            "${toolkit_version}" "lib/native/${component}.winmd"
            "${archive_hash}" PROBE_TOOLKIT_ALPHA)
        string(TOLOWER "${archive_hash}" expected_hash)
        wx_winui3_validate_nupkg_integrity(package_valid
            "${package_dir}" "${expected_hash}")
        if(NOT package_valid)
            message(FATAL_ERROR "Toolkit probe requires an archive-verified extraction: ${package_dir}")
        endif()
        wx_winui3_read_nuspec(nuspec "${package_dir}" "${package}"
            "${toolkit_version}")
        wx_winui3_require_nuspec_dependency("${nuspec}" "${package}"
            "Microsoft.WindowsAppSDK.WinUI" "${wxWINUI3_WINUI_VERSION}" MINIMUM)
        if(NOT nuspec MATCHES "<license[^>]*>MIT</license>")
            message(FATAL_ERROR "Unexpected Toolkit package license: ${package}")
        endif()
        list(APPEND extra_metadata "${package_dir}/lib/native/${component}.winmd")
        list(APPEND extra_payload
            "${package_dir}/runtimes/win-${wxWINUI3_ARCH}/native/${component}.dll"
            "${package_dir}/lib/native/${component}.winmd")
        list(APPEND extra_notices "${package_dir}/${package}.nuspec")
        if(EXISTS "${package_dir}/lib/native/${component}.pri")
            list(APPEND wxWINUI3_COMPONENT_PRI_FILES
                "${package_dir}/lib/native/${component}.pri")
        endif()
        string(APPEND probe_inventory "${package}=${toolkit_version}|${archive_hash}\n")
    endforeach()

    # Controls declares this transitive dependency. Preserve its own package
    # provenance: its nuspec license URL is not the Toolkit MIT declaration.
    set(win2d_hash "E13866088B3C50205AAE36E9E4CB3C23B21D11C4B4590DDCBEAAFA9EA2C79F48")
    wx_winui3_find_nuget_package(win2d_dir "Microsoft.Graphics.Win2D" "1.4.0"
        "lib/uap10.0/Microsoft.Graphics.Canvas.winmd" "${win2d_hash}")
    string(TOLOWER "${win2d_hash}" expected_hash)
    wx_winui3_validate_nupkg_integrity(win2d_valid "${win2d_dir}" "${expected_hash}")
    if(NOT win2d_valid)
        message(FATAL_ERROR "Toolkit probe requires an archive-verified Win2D extraction")
    endif()
    wx_winui3_read_nuspec(win2d_nuspec "${win2d_dir}" "Microsoft.Graphics.Win2D" "1.4.0")
    wx_winui3_require_nuspec_dependency("${win2d_nuspec}" "Microsoft.Graphics.Win2D"
        "Microsoft.WindowsAppSDK.WinUI" "${wxWINUI3_WINUI_VERSION}" MINIMUM)
    list(APPEND extra_metadata "${win2d_dir}/lib/uap10.0/Microsoft.Graphics.Canvas.winmd")
    list(APPEND extra_payload
        "${win2d_dir}/runtimes/win-${wxWINUI3_ARCH}/native/Microsoft.Graphics.Canvas.dll"
        "${win2d_dir}/lib/uap10.0/Microsoft.Graphics.Canvas.winmd")
    list(APPEND extra_notices "${win2d_dir}/Microsoft.Graphics.Win2D.nuspec")
    string(APPEND probe_inventory "Microsoft.Graphics.Win2D=1.4.0|${win2d_hash}\n")

    foreach(file IN LISTS extra_metadata extra_payload extra_notices)
        if(NOT EXISTS "${file}" OR file MATCHES "[\r\n;]")
            message(FATAL_ERROR "Toolkit probe payload is missing or unsafe: ${file}")
        endif()
    endforeach()

    # A private projection prevents a Toolkit experiment from regenerating
    # the projection against which all wxWidgets libraries were compiled.
    set(probe_root "${wxBINARY_DIR}/winui/toolkit-probe")
    set(projection_root "${probe_root}/cppwinrt")
    set(projection_inputs ${wxWINUI3_CPPWINRT_INPUTS} ${extra_metadata})
    wx_winui3_compute_cppwinrt_fingerprint(projection_fingerprint
        INPUTS ${projection_inputs}
        WINDOWS_WINMD "${wxWINUI3_WINDOWS_WINMD}"
        TOOL "${wxWINUI3_CPPWINRT_TOOL}")
    set(projection_dir "${projection_root}/${projection_fingerprint}")
    set(input_list "${probe_root}/inputs-${projection_fingerprint}.txt")
    file(MAKE_DIRECTORY "${probe_root}")
    file(WRITE "${input_list}" "")
    foreach(input IN LISTS projection_inputs)
        file(APPEND "${input_list}" "${input}\n")
    endforeach()
    add_custom_target(wx_winui_toolkit_probe_cppwinrt
        COMMAND "${CMAKE_COMMAND}"
            "-DWX_CPPWINRT_OUTPUT_ROOT=${projection_root}"
            "-DWX_CPPWINRT_FINGERPRINT=${projection_fingerprint}"
            "-DWX_CPPWINRT_INPUT_LIST=${input_list}"
            "-DWX_CPPWINRT_WINDOWS_WINMD=${wxWINUI3_WINDOWS_WINMD}"
            "-DWX_CPPWINRT_TOOL=${wxWINUI3_CPPWINRT_TOOL}"
            "-DWX_CPPWINRT_REQUIRED_RELATIVE=winrt/XamlToolkit.Labs.WinUI.h"
            -P "${wxSOURCE_DIR}/build/cmake/winui-cppwinrt.cmake"
        BYPRODUCTS "${projection_dir}/winrt/XamlToolkit.Labs.WinUI.h"
            "${projection_dir}/wxwinui-projection-manifest.txt"
        DEPENDS ${projection_inputs} "${wxWINUI3_WINDOWS_WINMD}"
            "${wxWINUI3_CPPWINRT_TOOL}"
            "${wxSOURCE_DIR}/build/cmake/winui-cppwinrt.cmake"
        COMMENT "Verifying the isolated Toolkit consumption projection"
        VERBATIM)

    # Reuse the existing transactional PRI worker with function-local inputs.
    # Its content-addressed result cannot replace the ordinary runtime PRI.
    set(wxWINUI3_PAYLOAD_FINGERPRINT_INPUT "contract=wx-toolkit-probe-pri-v1\n")
    foreach(component_pri IN LISTS wxWINUI3_COMPONENT_PRI_FILES)
        file(SHA256 "${component_pri}" pri_hash)
        get_filename_component(pri_name "${component_pri}" NAME)
        string(APPEND wxWINUI3_PAYLOAD_FINGERPRINT_INPUT "${pri_name}=${pri_hash}\n")
    endforeach()
    string(APPEND wxWINUI3_PAYLOAD_FINGERPRINT_INPUT
        "config=${wxWINUI3_PRI_CONFIG_HASH}\nmakepri=${wxWINUI3_MAKEPRI_HASH}\n"
        "sdk=${wxWINUI3_WINDOWS_SDK_VERSION}\n")
    string(SHA256 wxWINUI3_PAYLOAD_SOURCE_HASH "${wxWINUI3_PAYLOAD_FINGERPRINT_INPUT}")
    wx_winui3_prepare_runtime_pri(probe_pri_stage)
    set(deploy_script "${probe_root}/deploy-toolkit-probe.cmake")
    string(REPLACE "${base_pri_stage}/resources.pri"
        "${probe_pri_stage}/resources.pri"
        deploy_content "${wxWINUI3_DEPLOY_SCRIPT_CONTENT}")
    foreach(file IN LISTS extra_payload extra_notices)
        get_filename_component(file_name "${file}" NAME)
        string(APPEND deploy_content
            "\nexecute_process(COMMAND \"\${CMAKE_COMMAND}\" -E copy_if_different\n"
            "    \"${file}\" \"\${WX_WINUI_DEPLOY_DIR}/${file_name}\"\n"
            "    RESULT_VARIABLE toolkit_copy_result)\n"
            "if(NOT toolkit_copy_result EQUAL 0)\n"
            "    message(FATAL_ERROR \"Toolkit probe deployment failed: ${file_name}\")\n"
            "endif()\n")
    endforeach()
    file(WRITE "${probe_root}/package-inventory.txt" "${probe_inventory}")
    string(APPEND deploy_content
        "\nexecute_process(COMMAND \"\${CMAKE_COMMAND}\" -E copy_if_different\n"
        "    \"${probe_root}/package-inventory.txt\" \"\${WX_WINUI_DEPLOY_DIR}/package-inventory.txt\"\n"
        "    RESULT_VARIABLE toolkit_copy_result)\n"
        "if(NOT toolkit_copy_result EQUAL 0)\n"
        "    message(FATAL_ERROR \"Toolkit probe inventory deployment failed\")\n"
        "endif()\n")
    file(WRITE "${deploy_script}" "${deploy_content}")

    add_executable(wx_winui_toolkit_activation_probe EXCLUDE_FROM_ALL
        "${wxSOURCE_DIR}/tests/winui/runtime-smoke.cpp"
        "${wxSOURCE_DIR}/tests/winui/toolkit-activation-probe.h")
    target_compile_definitions(wx_winui_toolkit_activation_probe PRIVATE
        WX_WINUI_TOOLKIT_ACTIVATION_PROBE=1)
    target_include_directories(wx_winui_toolkit_activation_probe PRIVATE
        "${projection_dir}" "${wxWINUI3_CPPWINRT_INCLUDE_DIR}"
        "${wxWINUI3_WINUI_PACKAGE_DIR}/include"
        "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/include"
        "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/include"
        "${wxWINUI3_RUNTIME_PACKAGE_DIR}/include")
    target_link_libraries(wx_winui_toolkit_activation_probe PRIVATE
        "${wxWINUI3_BOOTSTRAP_LIB}" "${wxWINUI3_DISPATCHING_LIB}"
        windowsapp runtimeobject ole32)
    target_compile_options(wx_winui_toolkit_activation_probe PRIVATE /bigobj)
    set_target_properties(wx_winui_toolkit_activation_probe PROPERTIES
        CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON FOLDER "Tests"
        RUNTIME_OUTPUT_DIRECTORY "${probe_root}/bin/$<CONFIG>$<1:/>")
    add_custom_target(wx_winui_toolkit_probe_deploy
        COMMAND "${CMAKE_COMMAND}"
            "-DWX_WINUI_DEPLOY_DIR=$<TARGET_FILE_DIR:wx_winui_toolkit_activation_probe>"
            -P "${deploy_script}"
        COMMENT "Deploying the isolated Toolkit consumption payload"
        VERBATIM)
    add_dependencies(wx_winui_toolkit_activation_probe
        wx_winui_toolkit_probe_cppwinrt wx_winui_toolkit_probe_deploy)
    add_test(NAME wx_winui_toolkit_activation_probe
        COMMAND wx_winui_toolkit_activation_probe --toolkit-activation-probe)
    set_tests_properties(wx_winui_toolkit_activation_probe PROPERTIES
        TIMEOUT 60 LABELS "winui;experimental;toolkit-probe")
endfunction()

wx_winui3_add_toolkit_probe()
