target("test_tr")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(testdir)")

    add_packages("spdlog")

    add_includedirs("include", {public = true})
    add_headerfiles("include/(**)")
    add_headerfiles("src/(**.hpp)", {install = false})
    add_files("src/**.cpp")

    before_build(function (target)
        local headers = target:headerfiles()
        local sources = target:sourcefiles()
        function removecomment(str)
            local finstr = ""
            local last = 0
            repeat
                local _, endpos1 = string.find(str, '[^\\]"', last + 1)
                if endpos1 == nil then
                    break
                end
                local _, endpos2 = string.find(str, '[^\\]"', endpos1 + 1)
                if endpos2 == nil then
                    break
                end
                finstr = finstr .. string.sub(str, last + 1, endpos1 - 1)
                last = endpos2
            until false
            return finstr .. string.sub(str, last + 1)
        end
        for _, headerfile in ipairs(sources) do
            print(headerfile)
            local data = io.readfile(headerfile)
            for block in data:gmatch('%"(.-)%"_tr') do
                print(removecomment(block))
            end
        end
    end)
    after_build(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoclean(target)
    end)
target_end()