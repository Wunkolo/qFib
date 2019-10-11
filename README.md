# qFib [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)]()

---

This is a little proof of concept of a very curious matrix that I have derived
that allows for multiple fibonacci terms to be calculated in parallel using
only a subset of the previous terms:

```
Important relations
F( n - 0 )	= F( n - 1 ) + F( n - 2 )
F( n - 1 )	= F( n - 2 ) + F( n - 3 )
F( n - 2 )	= F( n - 3 ) + F( n - 4 )
F( n - 3 )	= F( n - 4 ) + F( n - 5 )
F( n - 4 )	= F( n - 5 ) + F( n - 6 )

Guided substitutions, favoring power-of-two coefficients(marked with *).
Also limiting the dependencies to only the previous four terms.

F( n )		= F( n - 1 ) + F( n - 2 )
			= ( F( n - 2 ) + F( n - 3 ) ) + F( n - 2 )
	*		= 2 * F( n - 2 ) + F( n - 3 )
			= 2 * (F( n - 3 ) + F( n - 4 )) + F( n - 3  )
			= 3 * F( n - 3 ) + 2 * F( n - 4 )

F( n + 1 )  = F( n ) + F( n - 1 )
			= ( F( n - 1 ) + F( n - 2 ) ) + F( n - 1 )
	*		= 2 * F( n - 1 ) + F( n - 2 )
			= 2 * (F( n - 2 ) + F( n - 3 )) + F( n - 2  )
			= 3 * F( n - 2 ) + 2 * F( n - 3 )

F( n + 2 )  = F( n + 1 ) + F( n )
			= (F( n ) + F( n - 1 )) + F( n )
			= 2 * F( n ) + F( n - 1 )
			= 2 * (F( n - 1 ) + F( n - 2 )) + F( n - 1 )
			= 2 * F( n - 1 ) + 2 * F( n - 2 ) + F( n - 1 )
			= 3 * F( n - 1 ) + 2 * F( n - 2 )

			= 2 * F( n ) + F( n - 1 )
			= 2 * ( 2 * F( n - 2 ) + F( n - 3 ) ) + F( n - 1 )
	*		= 4 * F( n - 2 ) + 2 * F( n - 3 ) + F( n - 1 )


F( n + 3 )  = F( n + 2 ) + F( n + 1 )
			= 3 * F( n - 1 ) + 2 * F( n - 2 ) + 3 * F( n - 2 ) + 2 * F( n - 3 )
			= 3 * F( n - 1 ) + 5 * F( n - 2 ) + 2 * F( n - 3 )
			= 3 * F( n - 1 ) + 2 * F( n - 2 ) + F( n ) + F( n - 1 )
			= 4 * F( n - 1 ) + 2 * F( n - 2 ) + F( n )
	*		= 4 * F( n - 1 ) + 4 * F( n - 2 ) + F( n - 3 )
```

Here, this matrix allows the next four fibonacci terms to be calculated using
only the previous four terms.
The coefficients curiously follow a pattern similar to [Pascal's Triangle](https://en.wikipedia.org/wiki/Pascal%27s_triangle).

```
F( n + 4 * k + 3 ) = [ 4 4 1 0 ]^k  ( F( n - 1 ) )
F( n + 4 * k + 2 ) = [ 1 4 2 0 ]    ( F( n - 2 ) )
F( n + 4 * k + 1 ) = [ 2 1 0 0 ]    ( F( n - 3 ) )
F( n + 4 * k + 0 ) = [ 1 1 0 0 ]    ( F( n - 4 ) )
```

Exponentiation allows for a much larger 4*k stride of fibonacci values

```
exp 1, Skip 4
F( n + 11 ) = [ 4 4 1 0 ]^1  ( F( n - 1 ) )
F( n + 10 ) = [ 1 4 2 0 ]    ( F( n - 2 ) )
F( n +  9 ) = [ 2 1 0 0 ]    ( F( n - 3 ) )
F( n +  8 ) = [ 1 1 0 0 ]    ( F( n - 4 ) )

exp 2, Skip 8
F( n + 11 ) = [ 4 4 1 0 ]^2  ( F( n - 1 ) )
F( n + 10 ) = [ 1 4 2 0 ]    ( F( n - 2 ) )
F( n +  9 ) = [ 2 1 0 0 ]    ( F( n - 3 ) )
F( n +  8 ) = [ 1 1 0 0 ]    ( F( n - 4 ) )

F( n + 11 ) = [ 22 33 12 0 ]  ( F( n - 1 ) )
F( n + 10 ) = [ 12 22  9 0 ]  ( F( n - 2 ) )
F( n +  9 ) = [  9 12  4 0 ]  ( F( n - 3 ) )
F( n    8 ) = [  5  8  3 0 ]  ( F( n - 4 ) )

exp 3, skip 12
F( n + 15 ) = [ 4 4 1 0 ]^3  ( F( n - 1 ) )
F( n + 14 ) = [ 1 4 2 0 ]    ( F( n - 2 ) )
F( n + 13 ) = [ 2 1 0 0 ]    ( F( n - 3 ) )
F( n   12 ) = [ 1 1 0 0 ]    ( F( n - 4 ) )

F( n + 15 ) = [ 145 232 88 0 ]  ( F( n - 1 ) )
F( n + 14 ) = [  88 145 56 0 ]  ( F( n - 2 ) )
F( n + 13 ) = [  56  88 33 0 ]  ( F( n - 3 ) )
F( n + 12 ) = [  34  55 21 0 ]  ( F( n - 4 ) )
```

The coefficients of this matrix all happen to be powers of two, or zero, which
reduces the matrix multiplication into very fast and simple bit shifts to the
left(`_mm_sllv_epi32`) and horizontal adds(transpose the matrix, and use
`_mm_add_epi32`).

On an [Intel(R) Core(TM) i3-6100 CPU @ 3.70GHz](https://en.wikichip.org/wiki/intel/core_i3/i3-6100),
batches of **four** fibonacci terms(mod 2^32) can be calculated, on average, in
parallel in only **21ns** using SSE instructions.

```

       0:                               0
       1:                               1
       2:                               1
       3:                               2
24ns | 
       0:                               3
       1:                               5
       2:                               8
       3:                              13
27ns | 
       4:                              21
       5:                              34
       6:                              55
       7:                              89
20ns | 
       8:                             144
       9:                             233
      10:                             377
      11:                             610
20ns | 
      12:                             987
      13:                            1597
      14:                            2584
      15:                            4181
19ns | 
      16:                            6765
      17:                           10946
      18:                           17711
      19:                           28657
19ns | 
      20:                           46368
      21:                           75025
      22:                          121393
      23:                          196418
19ns | 
      24:                          317811
      25:                          514229
      26:                          832040
      27:                         1346269
19ns | 
      28:                         2178309
      29:                         3524578
      30:                         5702887
      31:                         9227465
...
```

While other methods, on average, would take hundreds of nanoseconds just to
serially calculate one fibonacci term.

```
n  |  Recursive    2-Stack 2-Stack-Register Matrix-Exponent Chun-Min Chang
0  |       219|       103|              95|            100|           285|
1  |        67|        67|              50|             54|           101|
2  |        84|        53|              52|             54|            95|
3  |       121|       118|             117|            139|           128|
4  |       142|       117|             133|            113|           108|
5  |       156|       108|             130|            421|           111|
6  |       266|       445|             105|            138|           114|
7  |       379|       145|             131|            106|           131|
8  |       402|       132|             129|            140|           134|
9  |       535|       161|              98|            135|           110|
10 |       728|       137|              98|             94|           124|
11 |      1403|       449|             127|            112|           149|
12 |      1855|       130|             137|            134|           149|
13 |      2168|       152|             137|            122|           118|
14 |      3064|       111|              96|             89|           102|
15 |      3266|        83|             105|             74|            93|
16 |      5846|       106|             136|             93|           123|
17 |     11604|       141|             153|            131|           138|
18 |     26330|       128|             138|            137|           163|
19 |     30329|       107|             117|            119|            98|
20 |     46648|       136|             123|             99|            92|
21 |     67866|       112|             108|             86|           124|
22 |    102843|       121|             115|            102|           102|
```
