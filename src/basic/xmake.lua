

target("basic")
    set_kind("headeronly")
    set_targetdir("$(libdir)")

    on_load(function (target)
        function Camel(str)
            return str:sub(1, 1):upper() .. str:sub(2)
        end
        import("core.project.project")
        target:set("basename", "Vks" .. Camel(target:name()))
    end)

    add_rules("targetclean")
    if is_kind("shared") then
        add_defines("VKS_DLL", {public = true})
    end

    -- target files
    add_includedirs("include", {public = true})
    add_headerfiles("include/(**)")
    -- add_headerfiles("src/**.hpp", {install = false})
    -- add_files("src/**.cpp")
target_end()