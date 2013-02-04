function table.append(dest, src)
    for i, v in ipairs(src) do
        table.insert(dest, v)
    end
end