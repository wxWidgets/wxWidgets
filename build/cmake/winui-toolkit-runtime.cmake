#############################################################################
# Name:        build/cmake/winui-toolkit-runtime.cmake
# Purpose:     Explicit native Toolkit runtime preparation and deployment
# Licence:     wxWindows licence
#############################################################################

include_guard(GLOBAL)

# Including this file does not enable a feature, acquire packages, change the
# ordinary WinUI projection/payload, or attach anything to a shipping target.
# After winui.cmake, an integrating caller must explicitly prepare the runtime:
#
# wx_winui3_prepare_toolkit_runtime(
#     ACCEPT_EXPERIMENTAL_PACKAGES ACCEPT_WIN2D_PACKAGE_LICENSE)
# wx_winui3_configure_toolkit_target(<library-or-executable>)
# wx_winui3_deploy_toolkit_runtime(<executable>)
#
# Deployment must precede wx_winui3_deploy_runtime() for the executable. All
# executables sharing that output directory must use the same Toolkit payload.
# No install/export policy is selected here; the payload inventory is available
# through wx_winui3_get_toolkit_runtime_payload() for explicit packaging.
# Optional TOOLKIT_LICENSE_FILE/TOOLKIT_LICENSE_SHA256 and
# WIN2D_LICENSE_FILE/WIN2D_LICENSE_SHA256 add reviewed local notices. They do
# not substitute for, or silently rewrite, the original package declarations.

function(wx_winui3_toolkit_checked_path out_var path)
    get_filename_component(absolute "${path}" ABSOLUTE
        BASE_DIR "${CMAKE_BINARY_DIR}")
    file(TO_CMAKE_PATH "${absolute}" absolute)
    # These paths also become literals in generated CMake driver files.
    if(absolute MATCHES "[\r\n;\"$]" OR absolute STREQUAL "")
        message(FATAL_ERROR "wxWinUI Toolkit: unsafe generated-script path: ${path}")
    endif()
    set(${out_var} "${absolute}" PARENT_SCOPE)
endfunction()

function(wx_winui3_toolkit_check_license out_var path expected_hash)
    if("${path}" STREQUAL "")
        message(FATAL_ERROR "wxWinUI Toolkit: a reviewed local license/notice is required")
    endif()
    wx_winui3_toolkit_checked_path(license_file "${path}")
    string(TOLOWER "${expected_hash}" expected_hash)
    string(LENGTH "${expected_hash}" hash_length)
    if(NOT hash_length EQUAL 64 OR NOT expected_hash MATCHES "^[0-9a-f]+$" OR
            NOT EXISTS "${license_file}" OR IS_DIRECTORY "${license_file}")
        message(FATAL_ERROR "wxWinUI Toolkit: missing license file or SHA256: ${path}")
    endif()
    file(SHA256 "${license_file}" actual_hash)
    if(NOT actual_hash STREQUAL expected_hash)
        message(FATAL_ERROR "wxWinUI Toolkit: license/notice hash mismatch: ${license_file}")
    endif()
    file(SIZE "${license_file}" license_size)
    if(license_size EQUAL 0)
        message(FATAL_ERROR "wxWinUI Toolkit: empty license/notice: ${license_file}")
    endif()
    set(${out_var} "${license_file}" PARENT_SCOPE)
endfunction()

function(wx_winui3_toolkit_check_nuspec_license package nuspec accept_win2d)
    if(package STREQUAL "Microsoft.Graphics.Win2D")
        if(NOT nuspec MATCHES "<requireLicenseAcceptance>true</requireLicenseAcceptance>" OR
                NOT nuspec MATCHES "<licenseUrl>http://www.microsoft.com/web/webpi/eula/eula_win2d_10012014.htm</licenseUrl>")
            message(FATAL_ERROR "wxWinUI Toolkit: unexpected Win2D package licensing metadata")
        endif()
        if(NOT accept_win2d)
            message(FATAL_ERROR
                "wxWinUI Toolkit: Microsoft.Graphics.Win2D 1.4.0 declares requireLicenseAcceptance=true; "
                "explicit ACCEPT_WIN2D_PACKAGE_LICENSE is required")
        endif()
    elseif(package MATCHES "^XamlToolkit\\.(Labs\\.)?WinUI(\\.(Converters|Controls|Helpers|Animations|Interactivity))?\\.Native$")
        if(NOT nuspec MATCHES "<license type=\"expression\">MIT</license>" OR
                NOT nuspec MATCHES "<requireLicenseAcceptance>false</requireLicenseAcceptance>")
            message(FATAL_ERROR "wxWinUI Toolkit: unexpected license declaration: ${package}")
        endif()
    else()
        message(FATAL_ERROR "wxWinUI Toolkit: unrecognized package licensing policy: ${package}")
    endif()
endfunction()

function(wx_winui3_toolkit_cached_package out_var package version archive_hash)
    # Cache-only: do not masquerade as PROBE_TOOLKIT_ALPHA or relax the normal
    # stable-package validator. The existing acquisition path can populate this
    # exact content-addressed cache separately. Never repair/remove user data.
    string(TOLOWER "${archive_hash}" archive_hash)
    string(SUBSTRING "${archive_hash}" 0 32 hash_key)
    wx_winui3_toolkit_checked_path(package_root "${wxWINUI3_PACKAGE_ROOT}")
    set(package_dir "${package_root}/${package}.${version}-${hash_key}")
    if(NOT IS_DIRECTORY "${package_dir}")
        message(FATAL_ERROR
            "wxWinUI Toolkit: exact verified package is absent: ${package_dir}. "
            "Populate the pinned package cache explicitly; runtime preparation never downloads.")
    endif()
    wx_winui3_validate_nupkg_integrity(valid "${package_dir}" "${archive_hash}")
    if(NOT valid)
        message(FATAL_ERROR "wxWinUI Toolkit: package extraction integrity failed: ${package_dir}")
    endif()
    # The extraction manifest remains usable when an archive has been pruned,
    # but a present archive must also still have the exact pinned bytes.
    if(EXISTS "${package_dir}.nupkg")
        file(SHA256 "${package_dir}.nupkg" actual_archive_hash)
        if(NOT actual_archive_hash STREQUAL archive_hash)
            message(FATAL_ERROR "wxWinUI Toolkit: archive hash mismatch: ${package_dir}.nupkg")
        endif()
    endif()
    set(${out_var} "${package_dir}" PARENT_SCOPE)
endfunction()

function(wx_winui3_prepare_toolkit_runtime)
    cmake_parse_arguments(PARSE_ARGV 0 ARG
        "ACCEPT_EXPERIMENTAL_PACKAGES;ACCEPT_WIN2D_PACKAGE_LICENSE"
        "TOOLKIT_LICENSE_FILE;TOOLKIT_LICENSE_SHA256;WIN2D_LICENSE_FILE;WIN2D_LICENSE_SHA256"
        "")
    if(ARG_UNPARSED_ARGUMENTS OR ARG_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "wxWinUI Toolkit: invalid preparation arguments")
    endif()
    get_property(prepared GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PREPARED)
    if(prepared AND ARGC EQUAL 0)
        return()
    endif()
    if(NOT WIN32 OR NOT MSVC OR NOT WXWINUI OR NOT wxUSE_WINUI3 OR
            NOT wxWINUI3_ARCH MATCHES "^(x86|x64|arm64)$" OR
            NOT wxWINUI3_DEPLOY_RESOURCES_PRI OR
            NOT COMMAND wx_winui3_prepare_runtime_pri OR
            NOT COMMAND wx_winui3_validate_nupkg_integrity OR
            NOT wxWINUI3_DEPLOY_SCRIPT_CONTENT)
        message(FATAL_ERROR
            "wxWinUI Toolkit: initialize the MSVC WinUI runtime with generated PRI deployment first")
    endif()
    if(NOT ARG_ACCEPT_EXPERIMENTAL_PACKAGES)
        message(FATAL_ERROR
            "wxWinUI Toolkit: explicit experimental-package acceptance is required")
    endif()
    # Preserve original nuspecs and factual declarations even if the caller
    # supplies no additional notices. Do not invent an acceptance requirement
    # for MIT Toolkit packages, or relabel Win2D's legacy license metadata.
    set(toolkit_license "")
    set(win2d_license "")
    if(ARG_TOOLKIT_LICENSE_FILE OR ARG_TOOLKIT_LICENSE_SHA256)
        wx_winui3_toolkit_check_license(toolkit_license
            "${ARG_TOOLKIT_LICENSE_FILE}" "${ARG_TOOLKIT_LICENSE_SHA256}")
    endif()
    if(ARG_WIN2D_LICENSE_FILE OR ARG_WIN2D_LICENSE_SHA256)
        wx_winui3_toolkit_check_license(win2d_license
            "${ARG_WIN2D_LICENSE_FILE}" "${ARG_WIN2D_LICENSE_SHA256}")
    endif()
    string(SHA256 preparation_key
        "${wxBINARY_DIR}|${wxWINUI3_ARCH}|${wxWINUI3_PACKAGE_ROOT}|${toolkit_license}|${ARG_TOOLKIT_LICENSE_SHA256}|${win2d_license}|${ARG_WIN2D_LICENSE_SHA256}|${ARG_ACCEPT_WIN2D_PACKAGE_LICENSE}")
    if(prepared)
        get_property(previous_key GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_KEY)
        if(NOT previous_key STREQUAL preparation_key)
            message(FATAL_ERROR "wxWinUI Toolkit: conflicting runtime preparations in one build")
        endif()
        return()
    endif()

    set(toolkit_version "0.8.0-alpha")
    set(pins
        "XamlToolkit.Labs.WinUI|A6B345F647D99931E93FBD20A555697D939642499BDA88D128FE797F69086ED2"
        "XamlToolkit.WinUI|76A6D0A2ABCB7D9F44511ECD0E62C82EFB25FC6BC079732FB11DA6AA2D9B49DF"
        "XamlToolkit.WinUI.Converters|D4593EC6564A66FBCF3654730B85A92AFE7AE395C3ED398AC7AE1EDE016CC7F5"
        "XamlToolkit.WinUI.Controls|82D0D3B8034FDDE4F3036B71C4F6F9DC3DB1D218C31F030BF8389BC7C93C52DE"
        "XamlToolkit.WinUI.Helpers|2A0CED054FF4337872963D0A6E5CEE568808CA53A9E9C1C148EE2FF166B185F3"
        "XamlToolkit.WinUI.Animations|FE3F7299E55CEBF07368FAEAE7C52F7580F7FC0EC99FECD9CD18F63A57B47AC8"
        "XamlToolkit.WinUI.Interactivity|3733AC07DDFF57EE76A569AED76F9803BD2DAAF0301EDF8897C349B97CFD8C65")
    set(toolkit_packages)
    foreach(pin IN LISTS pins)
        string(REPLACE "|" ";" fields "${pin}")
        list(GET fields 0 component)
        list(APPEND toolkit_packages "${component}.Native")
    endforeach()
    set(extra_metadata)
    set(extra_src)
    set(extra_dst)
    set(inventory "contract=wx-toolkit-runtime-v1\nrid=win-${wxWINUI3_ARCH}\n")
    foreach(pin IN LISTS pins)
        string(REPLACE "|" ";" fields "${pin}")
        list(GET fields 0 component)
        list(GET fields 1 archive_hash)
        set(package "${component}.Native")
        wx_winui3_toolkit_cached_package(package_dir "${package}"
            "${toolkit_version}" "${archive_hash}")
        wx_winui3_read_nuspec(nuspec "${package_dir}" "${package}" "${toolkit_version}")
        wx_winui3_toolkit_check_nuspec_license("${package}" "${nuspec}" FALSE)
        wx_winui3_require_nuspec_dependency("${nuspec}" "${package}"
            "Microsoft.WindowsAppSDK.WinUI" "${wxWINUI3_WINUI_VERSION}" MINIMUM)
        string(REGEX MATCHALL "<dependency[^>]+>" dependencies "${nuspec}")
        foreach(dependency IN LISTS dependencies)
            if(NOT dependency MATCHES "id=\"([^\"]+)\"[^>]*version=\"([^\"]+)\"")
                message(FATAL_ERROR "wxWinUI Toolkit: unreadable dependency in ${package}")
            endif()
            set(dependency_id "${CMAKE_MATCH_1}")
            set(dependency_version "${CMAKE_MATCH_2}")
            if(dependency_id STREQUAL "Microsoft.WindowsAppSDK.WinUI")
                continue()
            elseif(dependency_id STREQUAL "Microsoft.Graphics.Win2D" AND
                    package STREQUAL "XamlToolkit.WinUI.Controls.Native" AND
                    dependency_version STREQUAL "1.4.0")
                continue()
            endif()
            list(FIND toolkit_packages "${dependency_id}" dependency_index)
            if(dependency_index EQUAL -1 OR NOT dependency_version STREQUAL toolkit_version)
                message(FATAL_ERROR "wxWinUI Toolkit: dependency outside the pinned closure: ${dependency}")
            endif()
        endforeach()
        list(APPEND extra_metadata "${package_dir}/lib/native/${component}.winmd")
        list(APPEND extra_src
            "${package_dir}/runtimes/win-${wxWINUI3_ARCH}/native/${component}.dll"
            "${package_dir}/lib/native/${component}.winmd"
            "${package_dir}/${package}.nuspec")
        list(APPEND extra_dst "${component}.dll" "${component}.winmd"
            "licenses/${package}/${package}.nuspec")
        if(component STREQUAL "XamlToolkit.Labs.WinUI" OR
                component STREQUAL "XamlToolkit.WinUI.Controls")
            set(component_pri "${package_dir}/lib/native/${component}.pri")
            if(NOT EXISTS "${component_pri}")
                message(FATAL_ERROR "wxWinUI Toolkit: required PRI missing: ${component_pri}")
            endif()
            list(APPEND wxWINUI3_COMPONENT_PRI_FILES "${component_pri}")
        endif()
        string(APPEND inventory "${package}=${toolkit_version}|${archive_hash}\n")
    endforeach()

    set(win2d_hash "E13866088B3C50205AAE36E9E4CB3C23B21D11C4B4590DDCBEAAFA9EA2C79F48")
    wx_winui3_toolkit_cached_package(win2d_dir "Microsoft.Graphics.Win2D" "1.4.0" "${win2d_hash}")
    wx_winui3_read_nuspec(win2d_nuspec "${win2d_dir}" "Microsoft.Graphics.Win2D" "1.4.0")
    wx_winui3_toolkit_check_nuspec_license("Microsoft.Graphics.Win2D"
        "${win2d_nuspec}" "${ARG_ACCEPT_WIN2D_PACKAGE_LICENSE}")
    wx_winui3_require_nuspec_dependency("${win2d_nuspec}" "Microsoft.Graphics.Win2D"
        "Microsoft.WindowsAppSDK.WinUI" "${wxWINUI3_WINUI_VERSION}" MINIMUM)
    list(APPEND extra_metadata "${win2d_dir}/lib/uap10.0/Microsoft.Graphics.Canvas.winmd")
    list(APPEND extra_src
        "${win2d_dir}/runtimes/win-${wxWINUI3_ARCH}/native/Microsoft.Graphics.Canvas.dll"
        "${win2d_dir}/lib/uap10.0/Microsoft.Graphics.Canvas.winmd"
        "${win2d_dir}/Microsoft.Graphics.Win2D.nuspec")
    list(APPEND extra_dst "Microsoft.Graphics.Canvas.dll" "Microsoft.Graphics.Canvas.winmd"
        "licenses/Microsoft.Graphics.Win2D/Microsoft.Graphics.Win2D.nuspec")
    string(APPEND inventory "Microsoft.Graphics.Win2D=1.4.0|${win2d_hash}\n")
    if(toolkit_license)
        get_filename_component(license_name "${toolkit_license}" NAME)
        list(APPEND extra_src "${toolkit_license}")
        list(APPEND extra_dst "licenses/XamlToolkit/${license_name}")
        string(APPEND inventory "Toolkit.notice=${ARG_TOOLKIT_LICENSE_SHA256}\n")
    endif()
    if(win2d_license)
        get_filename_component(license_name "${win2d_license}" NAME)
        list(APPEND extra_src "${win2d_license}")
        list(APPEND extra_dst "licenses/Microsoft.Graphics.Win2D/${license_name}")
        string(APPEND inventory "Win2D.notice=${ARG_WIN2D_LICENSE_SHA256}\n")
    endif()

    # Register against local copies of the normal lists, retaining its collision
    # checks without publishing Toolkit files into default runtime deployment.
    list(LENGTH extra_src extra_count)
    math(EXPR extra_last "${extra_count} - 1")
    foreach(index RANGE ${extra_last})
        list(GET extra_src ${index} src)
        list(GET extra_dst ${index} dst)
        wx_winui3_toolkit_checked_path(src "${src}")
        wx_winui3_add_payload_file("${src}" "${dst}")
    endforeach()

    wx_winui3_toolkit_checked_path(runtime_root "${wxBINARY_DIR}/winui/toolkit-runtime")
    set(projection_root "${runtime_root}/cppwinrt")
    set(projection_inputs ${wxWINUI3_CPPWINRT_INPUTS} ${extra_metadata})
    wx_winui3_compute_cppwinrt_fingerprint(projection_fingerprint
        INPUTS ${projection_inputs} WINDOWS_WINMD "${wxWINUI3_WINDOWS_WINMD}"
        TOOL "${wxWINUI3_CPPWINRT_TOOL}")
    set(projection_dir "${projection_root}/${projection_fingerprint}")
    set(input_list "${runtime_root}/inputs-${projection_fingerprint}.txt")
    file(MAKE_DIRECTORY "${runtime_root}")
    file(WRITE "${input_list}" "")
    foreach(input IN LISTS projection_inputs)
        wx_winui3_toolkit_checked_path(input "${input}")
        file(APPEND "${input_list}" "${input}\n")
    endforeach()
    add_custom_target(wxwinui_toolkit_cppwinrt
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
            "${wxWINUI3_CPPWINRT_TOOL}" "${wxSOURCE_DIR}/build/cmake/winui-cppwinrt.cmake"
        COMMENT "Verifying the native Toolkit runtime projection" VERBATIM)
    set_property(TARGET wxwinui_toolkit_cppwinrt PROPERTY FOLDER "Build Infrastructure")

    set(base_pri "${wxWINUI3_PAYLOAD_STAGE_DIR}/resources.pri")
    set(wxWINUI3_PAYLOAD_FINGERPRINT_INPUT "contract=wx-toolkit-runtime-pri-v1\n")
    foreach(pri IN LISTS wxWINUI3_COMPONENT_PRI_FILES)
        file(SHA256 "${pri}" pri_hash)
        get_filename_component(pri_name "${pri}" NAME)
        string(APPEND wxWINUI3_PAYLOAD_FINGERPRINT_INPUT "${pri_name}=${pri_hash}\n")
    endforeach()
    string(APPEND wxWINUI3_PAYLOAD_FINGERPRINT_INPUT
        "config=${wxWINUI3_PRI_CONFIG_HASH}\nmakepri=${wxWINUI3_MAKEPRI_HASH}\n"
        "sdk=${wxWINUI3_WINDOWS_SDK_VERSION}\n")
    string(SHA256 wxWINUI3_PAYLOAD_SOURCE_HASH "${wxWINUI3_PAYLOAD_FINGERPRINT_INPUT}")
    wx_winui3_prepare_runtime_pri(pri_stage)
    set(toolkit_pri "${pri_stage}/resources.pri")
    list(FIND wxWINUI3_RUNTIME_PAYLOAD_DST "resources.pri" pri_index)
    if(pri_index EQUAL -1)
        message(FATAL_ERROR "wxWinUI Toolkit: missing base PRI payload entry")
    endif()
    list(REMOVE_AT wxWINUI3_RUNTIME_PAYLOAD_SRC ${pri_index})
    list(INSERT wxWINUI3_RUNTIME_PAYLOAD_SRC ${pri_index} "${toolkit_pri}")
    string(FIND "${wxWINUI3_DEPLOY_SCRIPT_CONTENT}" "${base_pri}" pri_script_index)
    if(pri_script_index EQUAL -1)
        message(FATAL_ERROR "wxWinUI Toolkit: base deployment does not reference its configured PRI")
    endif()
    string(REPLACE "${base_pri}" "${toolkit_pri}"
        deploy_body "${wxWINUI3_DEPLOY_SCRIPT_CONTENT}")

    set(inventory_file "${runtime_root}/wxwinui-toolkit-packages.txt")
    file(WRITE "${inventory_file}" "${inventory}")
    list(APPEND extra_src "${inventory_file}")
    list(APPEND extra_dst "licenses/XamlToolkit/wxwinui-toolkit-packages.txt")
    wx_winui3_add_payload_file("${inventory_file}" "licenses/XamlToolkit/wxwinui-toolkit-packages.txt")
    set(notices_file "${runtime_root}/wxwinui-toolkit-notices.txt")
    file(WRITE "${notices_file}"
        "Package licensing metadata for the exact pins in wxwinui-toolkit-packages.txt.\n"
        "The original nuspecs are deployed alongside this notice.\n"
        "This inventory does not replace any license text or certify redistribution compliance.\n\n"
        "XamlToolkit native packages 0.8.0-alpha: author lgztx96.\n"
        "Declared license expression: MIT; requireLicenseAcceptance=false.\n"
        "Declared license URL: https://licenses.nuget.org/MIT\n"
        "Declared source: https://github.com/lgztx96/CommunityToolkit.WinUI.git\n\n"
        "Microsoft.Graphics.Win2D 1.4.0: author Microsoft.\n"
        "Copyright: Microsoft Corporation. All rights reserved.\n"
        "Declared requireLicenseAcceptance=true; explicitly accepted by the configuring caller.\n"
        "Declared license URL: http://www.microsoft.com/web/webpi/eula/eula_win2d_10012014.htm\n"
        "The package does not declare an MIT license expression; do not infer one from the Toolkit packages.\n")
    list(APPEND extra_src "${notices_file}")
    list(APPEND extra_dst "licenses/XamlToolkit/wxwinui-toolkit-notices.txt")
    wx_winui3_add_payload_file("${notices_file}" "licenses/XamlToolkit/wxwinui-toolkit-notices.txt")
    # Recheck all selected Toolkit bytes before deployment, catching changes
    # after configure as well as preserving the provenance of external notices.
    set(deploy_content "# Generated native Toolkit runtime deployment.\n")
    foreach(src IN LISTS extra_src)
        file(SHA256 "${src}" src_hash)
        string(APPEND deploy_content
            "file(SHA256 \"${src}\" actual_hash)\n"
            "if(NOT actual_hash STREQUAL \"${src_hash}\")\n"
            "    message(FATAL_ERROR \"Toolkit payload changed after configure: ${src}\")\n"
            "endif()\n")
    endforeach()
    file(SHA256 "${toolkit_pri}" toolkit_pri_hash)
    string(APPEND deploy_content
        "file(SHA256 \"${toolkit_pri}\" actual_hash)\n"
        "if(NOT actual_hash STREQUAL \"${toolkit_pri_hash}\")\n"
        "    message(FATAL_ERROR \"Toolkit merged PRI changed after configure\")\n"
        "endif()\n${deploy_body}\n"
        # The base script preserves application-owned PRI files. Toolkit cannot
        # silently succeed with an unrelated index: keep it intact and fail.
        "file(SHA256 \"\${WX_WINUI_DEPLOY_DIR}/resources.pri\" actual_hash)\n"
        "if(NOT actual_hash STREQUAL \"${toolkit_pri_hash}\")\n"
        "    message(FATAL_ERROR \"Toolkit requires its merged resources.pri; the application-owned PRI was preserved\")\n"
        "endif()\n")
    list(LENGTH extra_src extra_count)
    math(EXPR extra_last "${extra_count} - 1")
    foreach(index RANGE ${extra_last})
        list(GET extra_src ${index} src)
        list(GET extra_dst ${index} dst)
        get_filename_component(dst_dir "${dst}" DIRECTORY)
        string(APPEND deploy_content
            "file(MAKE_DIRECTORY \"\${WX_WINUI_DEPLOY_DIR}/${dst_dir}\")\n"
            "execute_process(COMMAND \"\${CMAKE_COMMAND}\" -E copy_if_different\n"
            "    \"${src}\" \"\${WX_WINUI_DEPLOY_DIR}/${dst}\" RESULT_VARIABLE copy_result)\n"
            "if(NOT copy_result EQUAL 0)\n"
            "    message(FATAL_ERROR \"Toolkit deployment failed: ${dst}\")\n"
            "endif()\n")
    endforeach()
    set(deploy_script "${runtime_root}/wxWinUIDeployToolkitRuntime.cmake")
    file(WRITE "${deploy_script}" "${deploy_content}")
    set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_KEY "${preparation_key}")
    set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PROJECTION "${projection_dir}")
    set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_DEPLOY_SCRIPT "${deploy_script}")
    set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PAYLOAD_SRC "${wxWINUI3_RUNTIME_PAYLOAD_SRC}")
    set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PAYLOAD_DST "${wxWINUI3_RUNTIME_PAYLOAD_DST}")
    set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PREPARED TRUE)
endfunction()

function(wx_winui3_configure_toolkit_target target_name)
    if(NOT ARGC EQUAL 1 OR NOT TARGET "${target_name}")
        message(FATAL_ERROR "wxWinUI Toolkit: configure_target requires an existing target")
    endif()
    wx_winui3_prepare_toolkit_runtime()
    get_target_property(target_type "${target_name}" TYPE)
    get_target_property(imported "${target_name}" IMPORTED)
    if(imported OR NOT target_type MATCHES "^(EXECUTABLE|STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY)$")
        message(FATAL_ERROR "wxWinUI Toolkit: unsupported compile target ${target_name}")
    endif()
    get_target_property(configured "${target_name}" wxWINUI3_TOOLKIT_CONFIGURED)
    if(configured)
        return()
    endif()
    get_property(projection GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PROJECTION)
    target_include_directories("${target_name}" BEFORE PRIVATE "${projection}")
    target_compile_options("${target_name}" PRIVATE /bigobj)
    add_dependencies("${target_name}" wxwinui_toolkit_cppwinrt)
    set_property(TARGET "${target_name}" PROPERTY wxWINUI3_TOOLKIT_CONFIGURED TRUE)
endfunction()

function(wx_winui3_deploy_toolkit_runtime target_name)
    if(NOT ARGC EQUAL 1 OR NOT TARGET "${target_name}")
        message(FATAL_ERROR "wxWinUI Toolkit: deploy_runtime requires an executable target")
    endif()
    wx_winui3_prepare_toolkit_runtime()
    get_target_property(target_type "${target_name}" TYPE)
    get_target_property(imported "${target_name}" IMPORTED)
    if(imported OR NOT target_type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR "wxWinUI Toolkit: deployment is only valid for built executables")
    endif()
    get_target_property(deployed "${target_name}" wxWINUI3_TOOLKIT_DEPLOYED)
    if(deployed)
        return()
    endif()
    get_target_property(base_deployed "${target_name}" wxWINUI3_RUNTIME_DEPLOYED)
    if(base_deployed)
        message(FATAL_ERROR
            "wxWinUI Toolkit: select Toolkit deployment before normal deployment for ${target_name}")
    endif()
    get_property(wxWINUI3_DEPLOY_SCRIPT GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_DEPLOY_SCRIPT)
    wx_winui3_deploy_runtime("${target_name}")
    set_property(TARGET "${target_name}" PROPERTY wxWINUI3_TOOLKIT_DEPLOYED TRUE)
endfunction()

function(wx_winui3_get_toolkit_runtime_payload out_sources out_destinations)
    if(NOT ARGC EQUAL 2)
        message(FATAL_ERROR "wxWinUI Toolkit: payload query requires two output variables")
    endif()
    wx_winui3_prepare_toolkit_runtime()
    get_property(sources GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PAYLOAD_SRC)
    get_property(destinations GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PAYLOAD_DST)
    set(${out_sources} "${sources}" PARENT_SCOPE)
    set(${out_destinations} "${destinations}" PARENT_SCOPE)
endfunction()
