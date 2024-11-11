package("vulkan-memory-allocator-hpp")
    set_kind("library", {headeronly = true})
    set_homepage("https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/")
    set_description("C++ bindings for VulkanMemoryAllocator.")
    set_license("CC0")

    add_urls("https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/archive/refs/tags/$(version).tar.gz",
             "https://github.com/YaaZ/VulkanMemoryAllocator-Hpp.git")
    add_versions("v3.0.0", '2f062b1631af64519d09e7b319c2ba06d7de3c9c5589fb7109a3f4e341cee2b7')
    add_versions("v3.0.1-1", '0e0c374751d5ca6123d0ae0df756693f0674412d7c758ec4a39a5a9dcc412911')
    add_versions("v3.0.1-3", '66a3d4be3bc1404c844b5a36aadeb6b366878e7cf1efe899eb0a0095f3871aae')
    add_versions("v3.1.0", 'f5cbce9b8efdb0de721f22b172c76184979e93ee45a008bfe515533d5d2e2d36')

    add_configs("modules", {description = "Build with C++20 modules support.", default = false, type = "boolean"})

    add_deps("vulkan-memory-allocator")

    on_install("windows", "linux", "macosx", "mingw", "android", "iphoneos", function (package)
        if not package:config("modules") then
            if package:gitref() or package:version():ge("3.0.1") then
                package:add("deps", "vulkansdk >= 1.3.234")
            else
                package:add("deps", "vulkansdk < 1.3.234")
            end
            os.cp("include", package:installdir())
        else
            local vulkan_dep = "vulkansdk"
            package:add("deps", "vulkansdk >= 1.3.275")
            io.writefile("xmake.lua", format([[
                add_requires("vulkan-memory-allocator", "%s >= 1.3.275")
                target("vulkan-memory-allocator-hpp")
                    set_kind("moduleonly")
                    set_languages("c++20")
                    add_headerfiles("include/(**.hpp)")
                    add_includedirs("include")
                    add_files("src/*.cppm", {public = true})
                    add_packages("vulkan-memory-allocator", "%s")
            ]], vulkan_dep, vulkan_dep))
            local configs = {}
            import("package.tools.xmake").install(package, configs)
        end
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                int version = VMA_VULKAN_VERSION;
            }
        ]]}, {includes = "vk_mem_alloc.hpp", configs = {languages = "c++14"} }))
    end)