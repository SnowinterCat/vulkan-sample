target("test_vulkan")
    set_kind("binary")
    set_targetdir("$(testdir)")

    add_packages("spdlog", "imgui", "libsdl3",
        is_plat("android") and nil or "vulkansdk",
        has_config("has_std_out_ptr") and nil or "out_ptr",
        has_config("has_std_expected") and nil or "tl_expected" 
    )

    -- add_includedirs("include", {public = true})
    -- add_headerfiles("include/(**)")
    -- add_headerfiles("src/(**.hpp)", {install = false})
    add_files("src/**.cpp")

    after_build(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoclean(target)
    end)
target_end()