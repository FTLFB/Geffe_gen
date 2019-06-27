# Geffe_gen
correlation attack on Geffe generator

Linear-feedback shift register equations : 

(others are also apropriate, if they work properly ofc)

L1 = x^30 + x^6 + x^4 + x + 1

L2 = x^31 + x^3 + 1

L3 = x^32 + x^7 + x^5 + x^3 + x^2 + x + 1





 (runtime for variant 7 is ~11 min 30 sec) 
 
 the worst case runtime is 15 min or 30 min or whatever
 
 
 
 
 TO COMPILE : g++-8 Acr4.cpp -o Acr4.cpp -O3
 
 very important to enable flag -O3 (-Ofast) because of the amount of computations
 
 (L1 : 1 073 741 823(shifts) + L2 : 2 147 483 647(shifts) + (L3 : 4 294 967 295(shifts) * L1_pretenders_amount * L2_pretenders_amount))
