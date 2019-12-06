type Int is int

routine funcGetVoid(a:int) is
end

routine funcIntIncrement(a:int):int is
    return a + 1
end

routine funcFloatIncrement(a:float):float is
    return a + 1
end

func castToInt(a:float):int is
    return a
end

routine main():Int is
    var a:int
    var i1:int
    var i2:int is 2
    var i3:int is 3
    var i4:float
    var i5:float is 5.0
    i2 := 6.0 + i5
    if i2 then
        var i2:float is 1.5
        var i3:float is funcFloatIncrement(i2)
        if castToInt(i2) then
            i4 := funcFloatIncrement(i2) + i3 + 7
        else
            i4 := funcFloatIncrement(i2) + i3 + 5
         end
    else
        var i3:float is 3.3
        i4 := i2 + i3 + 42.0
    end
    i5 := funcIntIncrement(i4) + funcIntIncrement(i1 + i5)
    return i2 + castToInt(i5)
end