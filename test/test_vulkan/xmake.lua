target("test_vulkan")
    set_kind("binary")
    set_targetdir("$(testdir)")

    add_packages("spdlog", "imgui", "vulkansdk", "libsdl3")

    -- add_includedirs("include", {public = true})
    -- add_headerfiles("include/(**)")
    -- add_headerfiles("src/(**.hpp)", {install = false})
    add_files("src/**.cpp")

    after_build(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoclean(target)
    end)
target_end()