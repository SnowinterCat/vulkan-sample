-- 工程设置
set_project("vulkan-sample")
set_version("0.0.1", {soname = true})

set_configvar("LEGAL_COPYRIGHT", "Copyright (C) 2024 SnowinterCat")
set_configvar("PROJECT_NAME", "vulkan-sample")

-- 全局设置
option("stdc",   {showmenu = true, default = 23, values = {23, 17}})
option("stdcxx", {showmenu = true, default = 23, values = {23, 20, 17}})
function stdc_ver() 
    return get_config("stdc") and "c" .. get_config("stdc") or nil
end
function stdcxx_ver() 
    return get_config("stdcxx") and "c++" .. get_config("stdcxx") or nil
end

set_languages(stdc_ver(), stdcxx_ver())
set_warnings("allextra")
set_encodings("utf-8")
set_exceptions("cxx")

-- 添加编译选项
add_rules("mode.release", "mode.debug", "mode.releasedbg", "mode.minsizerel")
add_rules("plugin.compile_commands.autoupdate", {lsp = "clangd", outputdir = ".vscode"})

-- 编译设置
option("3rd_kind",     {showmenu = true, default = "shared", values = {"static", "shared"}})
option("outputdir",    {showmenu = true, default = path.join(os.projectdir(), "bin"), type = "string"})

-- 隐藏设置、隐藏目标、打包命令
includes("lua/hideoptions.lua")
includes("lua/hidetargets.lua")
includes("lua/pack.lua")

-- some of the third-party use our own configurations
add_repositories("myrepo 3rd", {rootdir = os.scriptdir()})
-- header-only libraries
add_requires(
    -- tools
    "vulkansdk",
    "vulkan-memory-allocator",
    "vulkan-memory-allocator-hpp"
)
-- normal libraries
add_requires("libsdl", {configs = {shared = is_config("third_party_kind", "shared")}})
add_requires("fmt", {configs = {shared = is_config("third_party_kind", "shared"), header_only = false}})
add_requires("spdlog", {configs = {shared = is_config("third_party_kind", "shared"), header_only = false, fmt_external = true, wchar = true, wchar_filename = false, console_wchar = true}})
-- normal libraries' dependencies configurations
add_requireconfs("**.libsdl", {configs = {shared = is_config("third_party_kind", "shared")}})
add_requireconfs("**.fmt", {configs = {shared = is_config("third_party_kind", "shared"), header_only = false}})
add_requireconfs("**.spdlog", {configs = {shared = is_config("third_party_kind", "shared"), header_only = false, fmt_external = true, wchar = true, wchar_filename = false, console_wchar = true}})

-- subdirectories
includes("src/*/xmake.lua")
includes("exec/*/xmake.lua")
includes("test/*/xmake.lua")
