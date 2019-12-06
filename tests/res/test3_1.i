type IntArray6 is array[6] int

type Struct1 is record
    var a:float
end

type Struct1Array3 is array[3] Struct1

func main():int is
    var arr1:IntArray6
    arr1[1] := 11
    arr1[2] := 12
    printInt(arr1[1])
    var arr2: Struct1Array3
    var str1: Struct1
    arr2[0] := str1
    arr2[1] := str1
    arr2[0].a := 13.37
    printInt(arr2[1].a)

    var i: int is 0
    while i <= arr2[1].a loop
        printInt(i * 10)
        i := i + 1
    end
    for j in reverse 5 .. 0 loop
        printInt(j * 10)
    end
    printInt(i*10000)
    //if arr[1].a == 1337
    return 0
end