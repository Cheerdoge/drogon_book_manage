add_rules("mode.debug", "mode.release")

add_requires("drogon", {configs = {sqlite3 = true}})
add_requires("openssl")

target("book_manage")
    set_kind("binary")
    set_languages("c++17")
    add_files("main.cc")
    add_files("controllers/*.cc")
    add_files("models/*.cc")
    add_files("filters/*.cc")
    add_files("utils/*.cc")
    add_includedirs(".")
    add_packages("drogon", "openssl")
    if is_plat("windows") then
        add_cxflags("/utf-8")
    end
    after_build(function (target)
        os.cp("$(projectdir)/config.json", target:targetdir() .. "/config.json")
        local srcdb = "$(projectdir)/library.db"
        if os.isfile(srcdb) then
            os.cp(srcdb, target:targetdir() .. "/library.db")
        end
    end)
