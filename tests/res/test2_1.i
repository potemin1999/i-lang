type StructType1 is record
    var var1: float is 1.0
    var var2: int is 2
    var var3: record
        var v1: float is 33.3
    end
end

routine structFunc(str1:StructType1):StructType1 is
    str1.var1 := str1.var2 + 1
    return str1
end

func func1(a:int):int is
    var res is 0
    if a < 10 then
        res := a
    else
        res := a + func1(a-1)
    end
    return res
end

routine main():int is
    var res is func1(15)
    printInt(res)
    var struct1: StructType1
    var struct2: record
        var s1: StructType1
    end
    var struct3: StructType1
    struct3 := structFunc(struct1)
    struct2.s1.var3.v1 := struct1.var1 + struct1.var2
    return 2
end