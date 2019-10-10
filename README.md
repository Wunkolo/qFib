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
reduces the matrix multiplication into very fast and simple bit shifts and
horizontal adds.
