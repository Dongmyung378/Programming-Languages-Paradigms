
stri = let
    c = clockwise
    w = allWhite
    b = allBlack in
    (c (c (c (w 1)(b 1)(b 1)(b 1))(c (b 1)(b 1)(b 1)(w 1))(b 2)(b 2))(c (b 2)(b 2)(c (w 1)(w 1)(w 1)(w 1))(c (w 1)(b 1)(b 1)(b 1)))(w 4)(c (b 2)(c (w 1)(w 1)(b 1)(b 1))(c (b 1)(w 1)(b 1)(b 1))(b 2)))
stro = let
    c = clockwise
    a = anticlockwise
    w = allWhite
    b = allBlack in
    (a (c (c (b 1)(b 1)(b 1)(b 1))(c (b 1)(b 1)(b 1)(b 1))(w 2)(b 2))(c (b 2)(c (b 1)(w 1)(w 1)(b 1))(c (b 1)(b 1)(w 1)(b 1))(b 2))(w 4)(c (b 2)(w 2)(c (w 1)(w 1)(w 1)(w 1))(c (b 1)(b 1)(b 1)(b 1))))

strA = (blur stri == stro)

main :: IO ()
main = do
    if strA then putStrLn("Success") else putStrLn("Fail")