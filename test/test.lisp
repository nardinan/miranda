(define test 10)
(define gianni 0)
(cond 
	((and (> test 5) (<= test 10)) (begin
                (print "output 10")
                (print "output 50")
                (set test 5)
                (set gianni 10)
                (if (> test 5)
                    (print "not visible")
                    (print "not visible as well"))))
	((< test 10) (print "output 30"))
)
(print "another test")
(print test)

(print "and gianni at the end goes to")
(print gianni)

