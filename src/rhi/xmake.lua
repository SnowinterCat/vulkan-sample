target("rhi")
    set_kind("$(kind)")
    set_targetdir("$(libdir)")

    add_deps("config")
    add_packages(
        is_plat("android") and nil or "vulkansdk",
        "vulkan-memory-allocator-hpp"
    )

    add_includedirs("include", {public = true})
    add_headerfiles("include/(**)")
    -- add_headerfiles("src/(**.hpp)", {install = false})
    add_files("src/**.cpp")

    on_load(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoname(target)
        auto().library_autodefine(target)
    end)
    after_build(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoclean(target)
    end)
target_end()