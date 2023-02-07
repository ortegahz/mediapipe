# NEW
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
# NEW
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "feature",
    "flag_group",
    "flag_set",
    "tool_path",
)

all_link_actions = [ # NEW
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]

def _impl(ctx):
    tool_paths = [
        tool_path(
            name = "gcc",
            path = "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/bin/aarch64-rockchip-linux-gnu-gcc",
        ),
        tool_path(
            name = "ld",
            path = "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/bin/aarch64-rockchip-linux-gnu-ld",
        ),
        tool_path(
            name = "ar",
            path = "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/bin/aarch64-rockchip-linux-gnu-ar",
        ),
        tool_path(
            name = "cpp",
            path = "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/bin/aarch64-rockchip-linux-gnu-c++",
        ),
        tool_path(
            name = "gcov",
            path = "/bin/false",
        ),
        tool_path(
            name = "nm",
            path = "/bin/false",
        ),
        tool_path(
            name = "objdump",
            path = "/bin/false",
        ),
        tool_path(
            name = "strip",
            path = "/bin/false",
        ),
    ]

    features = [ # NEW
        feature(
            name = "default_linker_flags",
            enabled = True,
            flag_sets = [
                flag_set(
                    actions = all_link_actions,
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-lstdc++",
                            ],
                        ),
                    ]),
                ),
            ],
        ),
    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        features = features, # NEW
        cxx_builtin_include_directories = [ # NEW
            "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/aarch64-rockchip-linux-gnu/sysroot/usr/include",
            "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/aarch64-rockchip-linux-gnu/include/c++/9.3.0/bits",
            "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/aarch64-rockchip-linux-gnu/include/c++/9.3.0/aarch64-rockchip-linux-gnu/bits",
            "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/aarch64-rockchip-linux-gnu/include/c++/9.3.0",
            "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/lib/gcc/aarch64-rockchip-linux-gnu/9.3.0/include",
            "/home/manu/softwares/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/lib/gcc/aarch64-rockchip-linux-gnu/9.3.0/include-fixed",
        ],
        toolchain_identifier = "local",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = "aarch64",
        target_libc = "unknown",
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
    )

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)