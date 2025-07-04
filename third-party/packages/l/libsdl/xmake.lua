package("libsdl")
    set_homepage("https://www.libsdl.org/")
    set_description("Simple DirectMedia Layer")
    set_license("zlib")

    if is_plat("mingw") and is_subhost("msys") then
        add_extsources("pacman::SDL3")
    elseif is_plat("linux") then
        add_extsources("pacman::sdl3", "apt::libsdl3-dev")
    elseif is_plat("macosx") then
        add_extsources("brew::sdl3")
    end

    add_urls("https://www.libsdl.org/release/SDL3-$(version).zip",
             "https://github.com/libsdl-org/SDL/releases/download/preview-$(version)/SDL3-$(version).zip",
             "https://github.com/libsdl-org/SDL/releases/download/release-$(version)/SDL3-$(version).zip", { alias = "archive" })
    add_urls("https://github.com/libsdl-org/SDL.git", { alias = "github" })

    add_versions("archive:3.1.6", "03b6114bc9c0857bb7beca021b4da093c0fe1a4e767dc11820437069832cd27c")

    add_versions("github:3.1.6", "preview-3.1.6")
    add_versions("github:3.1.3", "preview-3.1.3")

    add_deps("cmake")

    add_configs("sdlmain", {description = "Use SDL_main entry point", default = false, type = "boolean", readonly = is_plat("android")})
    if is_plat("linux", "bsd") then
        add_configs("x11", {description = "Enables X11 support (requires it on the system)", default = true, type = "boolean"})
        add_configs("wayland", {description = "Enables Wayland support", default = true, type = "boolean"})
    end

    if is_plat("wasm") then
        add_cxflags("-sUSE_SDL=0")
    end

    on_load(function (package)
        if package:config("sdlmain") then
            package:add("components", "main")
            if package:is_plat("mingw") then
                -- MinGW requires linking mingw32 before SDL3main
                local libsuffix = package:is_debug() and "d" or ""
                package:add("linkorders", "mingw32", "SDL3main" .. libsuffix)
            end
        else
            package:add("defines", "SDL_MAIN_HANDLED")
        end
        package:add("components", "lib")
        if package:is_plat("linux", "bsd") and package:config("x11") then
            package:add("deps", "libxext", {private = true})
        end
        if package:is_plat("linux", "bsd") and package:config("wayland") then
            package:add("deps", "wayland", {private = true})
        end
    end)

    on_component("main", function (package, component)
        local libsuffix = package:is_debug() and "d" or ""
        component:add("links", "SDL3main" .. libsuffix)
        if package:is_plat("windows") then
            component:add("syslinks", "shell32")
        elseif package:is_plat("mingw") then
            component:add("syslinks", "mingw32")
        end
        component:add("deps", "lib")
    end)

    on_component("lib", function (package, component)
        local libsuffix = package:is_debug() and "d" or ""
        if package:config("shared") then
            component:add("links", "SDL3" .. libsuffix)
        else
            component:add("links", (package:is_plat("windows") and "SDL3-static" or "SDL3") .. libsuffix)
            if package:is_plat("windows", "mingw") then
                component:add("syslinks", "user32", "gdi32", "winmm", "imm32", "ole32", "oleaut32", "version", "uuid", "advapi32", "setupapi", "shell32")
            elseif package:is_plat("linux", "bsd") then
                component:add("syslinks", "pthread", "dl")
                if package:is_plat("bsd") then
                    component:add("syslinks", "usbhid")
                end
            elseif package:is_plat("android") then
                component:add("syslinks", "dl", "log", "android", "GLESv1_CM", "GLESv2", "OpenSLES")
            elseif package:is_plat("iphoneos", "macosx") then
                component:add("frameworks", "AudioToolbox", "AVFoundation", "CoreAudio", "CoreVideo", "Foundation", "Metal", "QuartzCore", "CoreFoundation")
		        component:add("syslinks", "iconv")
                if package:is_plat("macosx") then
                    component:add("frameworks", "Cocoa", "Carbon", "ForceFeedback", "IOKit")
                else
                    component:add("frameworks", "CoreBluetooth", "CoreGraphics", "CoreMotion", "OpenGLES", "UIKit")
		        end
                if package:version():ge("2.0.14") then
                    package:add("frameworks", "CoreHaptics", "GameController")
                end
            end
        end
    end)

    on_fetch("linux", "macosx", "bsd", function (package, opt)
        if opt.system then
            -- use sdl3-config
            local sdl3conf = try {function() return os.iorunv("sdl3-config", {"--version", "--cflags", "--libs"}) end}
            if sdl3conf then
                sdl3conf = os.argv(sdl3conf)
                local sdl3ver = table.remove(sdl3conf, 1)
                local result = {version = sdl3ver}
                for _, flag in ipairs(sdl3conf) do
                    if flag:startswith("-L") and #flag > 2 then
                        -- get linkdirs
                        local linkdir = flag:sub(3)
                        if linkdir and os.isdir(linkdir) then
                            result.linkdirs = result.linkdirs or {}
                            table.insert(result.linkdirs, linkdir)
                        end
                    elseif flag:startswith("-I") and #flag > 2 then
                        -- get includedirs
                        local includedir = flag:sub(3)
                        if includedir and os.isdir(includedir) then
                            result.includedirs = result.includedirs or {}
                            table.insert(result.includedirs, includedir)
                        end
                    elseif flag:startswith("-l") and #flag > 2 then
                        -- get links
                        local link = flag:sub(3)
                        result.links = result.links or {}
                        table.insert(result.links, link)
                    elseif flag:startswith("-D") and #flag > 2 then
                        -- get defines
                        local define = flag:sub(3)
                        result.defines = result.defines or {}
                        table.insert(result.defines, define)
                    end
                end

                return result
            end

            -- finding using sdl3-config didn't work, fallback on pkgconfig
            if package.find_package then
                return package:find_package("pkgconfig::sdl3", opt)
            else
                return find_package("pkgconfig::sdl3", opt)
            end
        end
    end)

    on_install(function (package)
        io.replace("src/sensor/android/SDL_androidsensor.c", "ALooper_pollAll", "ALooper_pollOnce", {plain = true})
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DSDL_TEST=OFF")
        local opt
        if package:is_plat("linux", "bsd", "cross") then
            local includedirs = {}
            for _, depname in ipairs({"libxext", "libx11", "xorgproto"}) do
                local dep = package:dep(depname)
                if dep then
                    local depfetch = dep:fetch()
                    if depfetch then
                        for _, includedir in ipairs(depfetch.includedirs or depfetch.sysincludedirs) do
                            table.insert(includedirs, includedir)
                        end
                    end
                end
            end
            if #includedirs > 0 then
                includedirs = table.unique(includedirs)

                local cflags = {}
                opt = opt or {}
                opt.cflags = cflags
                for _, includedir in ipairs(includedirs) do
                    table.insert(cflags, "-I" .. includedir)
                end
                table.insert(configs, "-DCMAKE_INCLUDE_PATH=" .. table.concat(includedirs, ";"))
            end
        elseif package:is_plat("wasm") then
            -- emscripten enables USE_SDL by default which will conflict with the sdl headers
            opt = opt or {}
            opt.cflags = {"-sUSE_SDL=0"}
        end
        import("package.tools.cmake").install(package, configs, opt)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <SDL3/SDL.h>
            int main(int argc, char** argv) {
                SDL_Init(0);
                return 0;
            }
        ]]}, {configs = {defines = "SDL_MAIN_HANDLED"}}));
    end)
