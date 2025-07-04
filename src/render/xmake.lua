

target("render")
    set_kind("$(kind)")
    set_targetdir("$(libdir)")

    on_load(function (target)
        function Camel(str)
            return str:sub(1, 1):upper() .. str:sub(2)
        end
        import("core.project.project")
        target:set("basename", "Vks" .. Camel(target:name()))
    end)

    add_deps("basic")
    add_rules("targetclean")
    add_defines("VKS_RENDER_EXPORTS", "VULKAN_HPP_STORAGE_SHARED_EXPORT")
    add_packages("vulkan-memory-allocator-hpp", "spdlog")

    add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")

    -- version
    -- set_configdir("./")
    -- add_configfiles("*.rc.in")
    -- add_files("*.rc")
    -- target files
    add_includedirs("include", {public = true})
    add_headerfiles("include/(**)")
    -- add_headerfiles("src/**.hpp", "src/**.h", {install = false})
    add_files("src/**.cpp")
target_end()