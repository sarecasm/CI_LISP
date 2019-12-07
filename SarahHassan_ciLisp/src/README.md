ciLisp Project

* Name: Sarah Hassan
* Lab: ciLisp
* Due: December 9, 2019


Task 1 Submission
* Completed TODO Tasks for Task 1 
* No issues were found 
* Knows it works because terminal produces the right output for add, subtract, divide, etc.
* Updated switch statement  and fixed printRetVal for Task 1:  11/26
* Full sample run test below 


Sample Tests
*"/Users/SarahHassan/Desktop/COMP 232/SarahHassan_ciLisp/cmake-build-debug/cilisp"

> (neg 3)
* Data: -3.000000
* Type: Integer

> (abs -2)
* Data: 2.000000
* Type: Integer

> (exp 2)
* Data: 7.389056
* Type: Integer

> (sqrt 16)
* Data: 4.000000
* Type: Integer

> (add 6 3)
* Data: 9.000000
* Type: Integer

> (sub 7 2)
* Data: 5.000000
* Type: Integer

> (mult 6 2)
* Data: 12.000000
* Type: Integer

> (div 6 3)
* Data: 2.000000
* Type: Integer

> (remainder 5 2)
* Data: 1.000000
* Type: Integer

> (log 100)
* Data: 4.605170
* Type: Integer

> (pow 2 4)
* Data: 16.000000
* Type: Integer

> (max 7 2)
* Data: 7.000000
* Type: Integer

> (min 6 9)
* Data: 6.000000
* Type: Integer

> (exp2 3)
* Data: 8.000000
* Type: Integer

> (cbrt 27)
* Data: 3.000000
* Type: Integer

> (hypot 2 2)
* Data: 2.828427
* Type: Integer
***
Task 2 Submission 
* Completed Task 2
* No issues were found 
* Knows it works because the terminal produces the right output from the given examples on the project.
* Tested both the first and last box examples. No error.  
* Updated the I file, Y file, C file, and H file. 
* I File received two updates in regards to implementing let and symbol.
* Y, H, and C files received a lot of implementations in regards to Task 2.
* Completed 12/3

"/Users/SarahHassan/Desktop/Sarah Hassan ciLisp/CI_LISP/SarahHassan_ciLisp/cmake-build-debug/cilisp"

> (add ((let (abc 1)) (sub 3 abc)) 4)
* Data: 6.000000
* Type: Double

> (mult ((let (a 1) (b 2)) (add a b)) (sqrt 2))
* Data: 4.242641
* Type: Double

> (add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))
* Data: 5.398346
* Type: Double

> ((let (first (sub 5 1)) (second 2)) (add (pow 2 first) (sqrt second)))
* Data: 17.414214
* Type: Double

> (add ((let (abcd 1)) (sub 3 abcd)) 4)
* Data: 6.000000
* Type: Double

> (mult ((let (a 1) (b 2)) (add a b)) (sqrt 2))
* Data: 4.242641
* Type: Double

> (add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))
* Data: 5.398346
* Type: Double

> ((let (first (sub 5 1)) (second 2)) (add (pow 2 first) (sqrt second)))
* Data: 17.414214
* Type: Double

> ((let (a ((let (c 3) (d 4)) (mult c d)))) (sqrt a))
* Data: 3.464102
* Type: Double

***
Task 3 Submission 

* Updated l file by inserting "int" and "double"
* Updated y file by fixing number and let_elem 
* Updated C file in evalSymbolNode, evalNumNode
* Updated h file in regards to adding NUM_TYPE as a parameter for *createSymbol
* No issues were found, only issue is when copying and pasting the sample input and then modifying the int to a double after pasting it, an ERROR: invalid character >><< occurs. But after copying and pasting it again, it works. Or even when typing the whole input out from scratch in the terminal.. it works with no issues found. The issue arises sometimes not all the time. Realized the error when trying to change sample inputs. 
* Knows it works because of the test cases below 
* Completed 12/3 

"/Users/SarahHassan/Desktop/Sarah Hassan ciLisp/CI_LISP/SarahHassan_ciLisp/cmake-build-debug/cilisp"

> ((let (int a 1.25))(add a 1))
* WARNING: precision loss in the assignment for variable a
* Data: 2.000000
* Type: Double

> ((let (int a 5))(add a 1))
* Data: 6.000000
* Type: Double

> ((let (int a 4.25))(sub a 2))
* WARNING: precision loss in the assignment for variable a
* Data: 2.000000
* Type: Double

> ((let (double a 1.25))(add a 1))
* Data: 2.250000
* Type: Double

> ((let (int a 3))(mult a 2))
* Data: 6.000000
* Type: Double

***
Task 4 Submission 

* Updated C file by implementing printFunction & added Print Oper in switch statement in evalFuncNode & updated evalNumNode 
* Updated h file by adding RET_VAL printFunction 
* Updated l file by adding "print" in func 
* No issues were found in printing the s_expr
* Outputs are below for printing Int and Double
* Completed 12/6

"/Users/SarahHassan/Desktop/Sarah Hassan ciLisp/CI_LISP/SarahHassan_ciLisp/cmake-build-debug/cilisp"

> (print 3)
* => 3.000000
* Data: 3.000000
* Type: Integer

> (print 4.5)
* => 4.50
* Data: 4.500000
* Type: Double

> (print 6.25)
* => 6.25
* Data: 6.250000
* Type: Double

> (print 21)
* => 21.000000
* Data: 21.000000
* Type: Integer

> (print (sqrt 8))
* => 2.83
* Data: 2.828427
* Type: Double

> (print (mult 6.5 5))
* => 32.50
* Data: 32.500000
* Type: Double

---
Task 5 Submission





