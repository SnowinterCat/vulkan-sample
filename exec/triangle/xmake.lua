
target("triangle")
    set_kind("binary")
    set_targetdir("$(bindir)")

    add_deps("basic", "thirdParty", "platform", "render", "stl")
    add_rules("targetclean")
    add_packages("libsdl", "spdlog", "vulkan-memory-allocator-hpp")

    add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")

    -- version
    -- set_configdir("./")
    -- add_configfiles("*.rc.in")
    -- add_files("*.rc")
    -- target files
    -- add_includedirs("include", {public = true})
    -- add_headerfiles("include/(**)")
    add_headerfiles("src/**.hpp", {install = false})
    add_files("src/**.cpp")
target_end()