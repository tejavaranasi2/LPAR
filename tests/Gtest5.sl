(set-logic NIA)
(declare-var x0 Int)
(assume ( or ( and ( and (<= 28 x0) (>= 29 x0) )  )( and ( and (<= 29 x0) (>= 29 x0) )  )( and ( and (<= 29 x0) (>= 29 x0) )  )( and ( and (<= 27 x0) (>= 27 x0) )  )( and ( and (<= 26 x0) (>= 26 x0) )  )( and ( and (<= 26 x0) (>= 26 x0) )  )( and ( and (<= 27 x0) (>= 27 x0) )  )( and ( and (<= 27 x0) (>= 27 x0) )  )( and ( and (<= 28 x0) (>= 29 x0) )  )( and ( and (<= 29 x0) (>= 29 x0) )  )( and ( and (<= 29 x0) (>= 29 x0) )  )( and ( and (<= 29 x0) (>= 29 x0) )  )( and ( and (<= 28 x0) (>= 28 x0) )  ) ) )
(declare-var x1 Int)
(assume (and (<= 0 x1) (<= x1 80) ) )
(synth-fun f1 ((x0 Int) ) Int)
(constraint (<= 0 (+ (* 1 (+ (* 1 ( - 1900.000000)) (* ( f1 x0 ) 160.000000) (* ( * ( f1 x0 ) ( f1 x0 ) ) ( - 4.000000)) )) (* x0 40.000000) (* ( * x0 x0 ) ( - 1.000000)) ) ) )
(constraint (<= 0 (+ (* 1 (+ (* 1 ( - 2700.000000)) (* ( f1 x0 ) 120.000000) (* ( * ( f1 x0 ) ( f1 x0 ) ) ( - 2.000000)) )) (* x0 120.000000) (* ( * x0 x0 ) ( - 2.000000)) ) ) )
(constraint (<= 0 (+ (* 1 (+ (* 1 ( - 5500.000000)) (* ( f1 x0 ) 80.000000) (* ( * ( f1 x0 ) ( f1 x0 ) ) ( - 1.000000)) )) (* x0 240.000000) (* ( * x0 x0 ) ( - 3.000000)) ) ) )
(constraint (and (<= 0 ( f1 x0 ) ) (<= ( f1 x0 ) 80) ) ) 
(constraint (=> (and (<= 0 (+ (* 1 (+ (* 1 ( - 1900.000000)) (* x1 160.000000) (* ( * x1 x1 ) ( - 4.000000)) )) (* x0 40.000000) (* ( * x0 x0 ) ( - 1.000000)) ) ) (<= 0 (+ (* 1 (+ (* 1 ( - 2700.000000)) (* x1 120.000000) (* ( * x1 x1 ) ( - 2.000000)) )) (* x0 120.000000) (* ( * x0 x0 ) ( - 2.000000)) ) ) (<= 0 (+ (* 1 (+ (* 1 ( - 5500.000000)) (* x1 80.000000) (* ( * x1 x1 ) ( - 1.000000)) )) (* x0 240.000000) (* ( * x0 x0 ) ( - 3.000000)) ) ) ) (or (<= 1 0) (and (<= x1 ( f1 x0 ) ) )
) ) )
(check-synth)
