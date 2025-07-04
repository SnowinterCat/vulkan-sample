

target("platform")
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
    add_defines("VKS_PLATFORM_EXPORTS")

    -- version
    -- set_configdir("./")
    -- add_configfiles("*.rc.in")
    -- add_files("*.rc")
    -- target files
    add_includedirs("include", {public = true})
    add_headerfiles("include/(**)")
    -- add_headerfiles("src/**.hpp", {install = false})
    add_files("src/**.cpp")

    -- platform
    if not is_plat("windows") then
        remove_headerfiles("include/**/win/**")
        remove_files("src/win/**")
    end
    if not is_plat("linux") then
        remove_headerfiles("include/**/unix/**")
        remove_files("src/unix/**")
    end
    if not is_plat("android") then
        remove_headerfiles("include/**/andr/**")
        remove_files("src/andr/**")
    end
target_end()