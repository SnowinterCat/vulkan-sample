target("luancher")
    set_kind("binary")
    set_targetdir("$(bindir)")

    add_deps("config")
    -- add_packages("")

    -- 强制静态编译
    if is_config("runtimes", "c++_shared") then
        set_runtimes("c++_static")
    elseif is_config("runtimes", "stdc++_shared") then
        set_runtimes("stdc++_static")
    elseif is_config("runtimes", "MD") then
        set_runtimes("MT")
    elseif is_config("runtimes", "MDd") then
        set_runtimes("MTd")
    end

    -- add_includedirs("include", {public = true})
    -- add_headerfiles("include/(**)")
    add_headerfiles("src/(**.hpp)", {install = false})
    add_files("src/**.cpp")

    after_build(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoclean(target)
    end)
target_end()