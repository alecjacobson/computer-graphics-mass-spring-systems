# Computer Graphics – Mass-Spring Systems

> **To get started:** Clone this repository using
> 
>     git clone --recursive http://github.com/alecjacobson/computer-graphics-mass-spring-systems.git
>

![](images/flag.gif)

## Background

### Read Chapter 16.5 of _Fundamentals of Computer Graphics (4th Edition)_.

### Read ["Fast Simulation of Mass-Spring Systems" [Tiantian Liu et al. 2013]](http://graphics.berkeley.edu/papers/Liu-FSM-2013-11/Liu-FSM-2013-11.pdf)

### Mass-Spring Systems

In this assignment we'll consider animating a deformable shape.

We _model_ the shape's physical behavior by treating it as a network of [point
masses](https://en.wikipedia.org/wiki/Point_particle) and
[springs](https://en.wikipedia.org/wiki/Effective_mass_(spring–mass_system)). We
can think of our shape as a
[graph](https://en.wikipedia.org/wiki/Graph_(discrete_mathematics)) where each
vertex is a point mass and each edge is a spring.

Given _initial conditions_ (each point's starting position and starting
velocity, if any) we will create an animation following the laws of physics
forward in time. In the real world, physics is deterministic: if we know the
current state, we can be sure of what the next state will be (at least at the
scales we're considering). This will also be true of our physical simulation.

The law that we start with is Newton's second law, which states that the forces
$\f ∈ \R³$ acting on a body must equal its mass $m$ times its acceleration
$\a∈\R³$:


$$
\f = m \a.
$$

Notice that $\f$ and $\a$ are vectors, each having a magnitude and a direction.
We will build our computational simulation by asking for this equation to be
true for each point mass  in our network. The forces $\f_i$ acting on the $i$-th point
mass are simply the sum of forces coming from any incident spring edge $ij$ and
any external force (such as gravity).

[Personifying](https://en.wikipedia.org/wiki/Personification) physical objects, we say
that they are _at rest_ when their potential energy is zero. When the object is
_not at rest_ then it exerts a force pushing it toward its rest state ([elastic
force](https://en.wikipedia.org/wiki/Elasticity_(physics))), decreasing its
potential energy as fast as possible. The force is the negative gradient of the potential
energy.

A simple spring is defined by its stiffness $k>0$ and _rest_ length $r_{ij} ∈ \R$.
Its potential energy measures the squared difference of the current length and
the rest length times the stiffness:

$$
V(\p_i,\p_j) = ½k( ‖\p_i - \p_j‖ - r_{ij} )^2.
$$

![](images/potential-energy.png)

The force exerted by the spring on each mass is the [partial
derivative](https://en.wikipedia.org/wiki/Partial_derivative) of the potential
energy $V$ with respect to the corresponding mass position. For example, for
$\p_i$ we have

$$
\f\_{ij} = -\frac{∂V}{∂\p\_i} ∈ \R\^3.
$$

For now, we can postpone expanding $∂V/∂\p_i$, and just recognize that it is a
3D vector. 

Our problem is to determine _where_ all of the mass will be after a small
duration in time ($∆t$). 

> **Question:** What is a reasonable choice for the value of $∆t$ ?
>
> **Hint:** 🎞️ or 🖥️
>

We'll assume we know the current positions for each
mass $\p^t_i∈\R^3$ at the current time ($t$) and the current velocities
$\dot{\p}^t_i = ∂\p_i(t)/∂t ∈\R^3$. When $t=0$ then we call these the [initial
conditions](https://en.wikipedia.org/wiki/Initial_condition) of the entire
simulation. For $t≥0$, we can still think of these values as the initial
conditions for the remaining time.

In the real world, the trajectory of an object follows a continuous curve as a
function of time. In our simulation, we only need to know the position of each
pass at [discrete moments in
time](https://en.wikipedia.org/wiki/Discrete_time_and_continuous_time). We use
this to build discrete approximation of the time derivatives (velocities and
accelerations) that we encounter. Immediately, we can replace the current
velocties $\dot{\p}^t_i$ with a _backward_ [finite
difference](https://en.wikipedia.org/wiki/Finite_difference) of the positions
over the small time step:

$$
\dot{\p}\^t_i = \frac{\p\^t_i - \p\^{t-∆t}_i}{∆t}
$$
where $\p^{t-∆t}_i ∈ \R^3$ is the position at the _previous_ time.

We can also use a _central_ finite difference to define the acceleration at time
$t$:

$$
\a_i\^t = 
\ddot{\p}\^t\_i = 
\frac{∂²\p\_i(t)}{∂t²} = 
\frac{\dot{\p}\^{t+∆t}\_i - \dot{\p}\^{t}\_i}{∆t} =
\frac{\p\^{t+∆t}\_i - \p\^{t}\_i}{∆t}
-\frac{\p\^t\_i - \p\^{t-∆t}\_i}{∆t}=
\frac{\p\^{t+∆t}\_i - 2 \p\^{t}\_i + \p\^{t-∆t}}{∆t²}.
$$

This expression mentions our _unknown_ variables $\p^{t+∆t}_i$ for the first
time. We'll soon that based on definition of the potential spring energy above
and the acceleration here we can _solve_ for the values of these unknown
variables.

### Time integration as energy optimization

In the equation $\f = m \a$, the acceleration term $\a$ depends _linearly_ on the
unknowns $\p^{t+∆t}$. Unfortunately, even for a simple spring the forces $\f =
∂V/∂\p^{t+∆t}$ depend _non-linearly_ on $\p^{t+∆t}$. This means we have a
_non-linear_ system of equations, which can be tricky to satisfy directly.

<!--
If we expanded this as an expression, we
might write:
$$
\frac{∂ V(\p^{t+∆t})}{∂ \p} = 
\M \left(
\frac{\p\^{t+∆t}\_i - 2 \p\^{t}\_i + \p\^{t-∆t}}{∆t²}.
$$
-->

> **Question:** We've _chosen_ to define $\f$ as the forces that implicitly
> depend on the unknown positions $\p^{t+∆t}$ at the end of the
> time step $t+∆t$. What would happen if we defined the forces to explicitly
> depend on the (known) current positions $\p^t$?

An alternative is to view physics simulation as an optimization problem. We
will define an energy that will be minimized by the value of $\p^{t+∆t}$ that
satisfies $\f = m \a$. The minimizer $\p$ of some function $E(x)$ will satisfy
$∂E/∂\p = 0$. So we construct an energy $E$ such that $∂E/∂\p = \f - m\a$:

$$
\p\^{t+∆t} = \argmin_\p
\underbrace{
\left(\sum\limits_{ij} ½k( ‖\p_i-\p_j‖ - r_{ij})\^2\right)  - 
∆t\^2 \left(\sum\limits_i m_i \left(\frac{\p\_i - 2 \p\^{t}\_i + \p_i\^{t-∆t}}{∆t²}\right)\^2 \right) - 
\left(\sum\limits_i \p_i^\top \f^\text{ext}_i \right)
}_{E(\p)}
$$ 

Keen observers will identify that the first term is potential energy and the
second term resembles [kinetic
energy](https://en.wikipedia.org/wiki/Kinetic_energy). Intuitively, we can see
the first term as trying to return the spring to rest length (elasticity) and
the second term as trying to keep masses [moving in the same
direction](https://en.wikipedia.org/wiki/Newton%27s_laws_of_motion#Newton%27s_first_law). 

Because of the $‖\p_i-\p_j‖ - r_{ij}$ term, minimizing $E$ is a non-linear
optimization problem. The standard approach would be to apply [gradient
descent](https://en.wikipedia.org/wiki/Gradient_descent) (slow), [Gauss-Newton
method](https://en.wikipedia.org/wiki/Gauss–Newton_algorithm), or [Newton's
Method](https://en.wikipedia.org/wiki/Newton%27s_method_in_optimization) (too
complicated for this assignment).

In a relatively recent SIGGRAPH paper ["Fast Simulation of Mass-Spring
Systems"](http://graphics.berkeley.edu/papers/Liu-FSM-2013-11/Liu-FSM-2013-11.pdf),
Tiantian Liu et al. made a neat observation that makes designing an algorithm to
minimize $E$ quite simple and fast. For each spring $ij$, they observe that the
non-linear energy can be written as a small optimization problem:

$$
(‖\p_i - \p_j‖ - r_{ij})\^2  = \min_{\d_{ij}∈\R\^3,‖\d‖ = r_{ij}} ‖(\p_i - \p_j) - \d_{ij}‖\^2.
$$

It may seem like we've just created extra work. We took a closed-form expression 
(left) and replaced it with an optimization problem (right). Yet this
optimization problem is small ($\d_{ij}$ is a single 3D vector) and can be
easily solved _independently_ (and even in parallel) for each spring (i.e.,
$\d_{ij}$ doesn't depend on $\d_{\ell k}$ etc.). Reading the right-hand side in
English it says, find the vector of length $r_{ij}$ that is as close as possible
to the current spring vector $\p_i - \p_j$. 

![](images/dij-rij-closest-vector.png)


Now, suppose we somehow _knew already_ the vector $\d_{ij}$ corresponding to the
_unknown_ optimal solution $\p^{t+∆t}$, then treating $\d_{ij}$ as a _constant_ we could
find the optimal solution by solving the _quadratic_ optimization problem:

$$
\p\^{t+∆t} = \argmin_\p
\underbrace{
\left(\sum\limits_{ij} ½k‖(\p_i-\p_j) - \d_{ij}‖\^2\right)  - 
∆t\^2 \left(\sum\limits_i m_i \left(\frac{\p\_i - 2 \p\^{t}\_i + \p_i\^{t-∆t}}{∆t²}\right)\^2 \right) -
\left(\sum\limits_i \p_i^\top \f^\text{ext}_i \right)
}_{\tilde{E}(\p)}.
$$ 

The modified energy $\tilde{E}(\p)$ is _quadratic_ with respect to the unknowns
$\p$, therefore the solution is found when we set the first derivative equal to
zero: 

$$
\frac{d\tilde{E}}{d\p} = 0.
$$

This leads to a straightforward "local-global" iterative algorithm:

 - Step 1 (local): Given current values of $\p$ determine $\d_{ij}$ for each
   spring.
 - Step 2 (global): Given all $\d_{ij}$ vectors, find positions $\p$ that
   minimize quadratic energy $\tilde{E}$.
 - Step 3: if "not satisfied", go to Step 1.

For the purposes of this assignment we will assume that we're "satisfied" after
a fixed number of iterations (e.g., 50). More advanced _stopping criteria_ could
(should) be employed in general.

### Matrices

The [subtext](https://en.wikipedia.org/wiki/Subtext) of this assignment is
understanding the computational aspects of large matrices. In the algorithm
above, Step 1 is easy and relies on "local" information for each spring.

Step 2 on the otherhand involves all springs simultaneously.
[Matrices](https://en.wikipedia.org/wiki/Matrix_(mathematics)) are our
convenient notation for representing both the [linear
operators](https://en.wikipedia.org/wiki/Linear_operator) (e.g., in the equation
$\frac{d\tilde{E}}{d\p} = 0$) and the [quadratic
forms](https://en.wikipedia.org/wiki/Quadratic_form) (e.g., in the energy
$\tilde{E}$).

Let's begin by being precise about some notation. We will stack up all of the
$n$ unknown mass positions $\p_i ∈ \R^3$ as the rows of a matrix $\p∈\R^{n×3}$.
We can do the same for the _known_ previous time steps' positions
$\p^{t},\p^{t-∆t}∈\R^{n×3}$.

We can then express the inertial term using matrices:
$$
∆t\^2 \left(\sum\limits_i m_i \left(\frac{\p\_i - 2 \p\^{t}\_i - \p_i\^{t-∆t}}{∆t²}\right)\^2 \right) = \\\\
\frac{1}{∆t\^2} \left(\sum\limits_i 
\left(\p\_i - 2 \p\^{t}\_i - \p_i\^{t-∆t}\right)^\top
m_i
\left(\p\_i - 2 \p\^{t}\_i - \p_i\^{t-∆t}\right)
\right) = \\\\
\frac{1}{∆t\^2} 
\tr{
\left(\p - 2\p\^{t} + \p\^{t-∆t}\right)^\top \M \left(\p - 2\p\^{t} + \p\^{t-∆t}\right)
},
$$

where $\tr{\X}$ computes the [trace](https://en.wikipedia.org/wiki/Trace_(linear_algebra)) of $\X$ (sums up the diagonal entries: $\X_{11}+\X_{22}+\dots$).

and the entries of the square matrix $\M∈\R^{n×n}$ are set to 

$$\M_{ij} = \begin{cases} m_{i} & \text{ if $i=j$ } \\\\
0 & \text{ otherwise.} \end{cases}.$$

The potential energy term can be similarly written with matrices. We'll start by
introducing the _signed incidence_ matrix of our mass-psring network of $n$
vertices and $m$ edges $\A∈\R^{m × n}$. The _rows_ of $\A$ correspond to an arbitrary
(but fixed) ordering of the edges in the network. In a mass-spring network, the
edges are un-oriented in the sense that the spring acts symmetrically on its
vertices. For convenience, we'll pick an orientation for edge anyway. For the
$e$-th edge $ij$, we should be sure to use the same orientation when computing
$\d_{ij}$ and for the following entries of $\A$. So, for the $e$-th row of $\A$
corresponding to edge connecting vertices $i$ and $j$ we'll assign values:

$$\A_{ek} = \begin{cases} +1 & \text{ if $k=i$ } \\\\
-1 & \text{ else if $k==j$ } \\\\
0 & \text{ otherwise.} \end{cases}$$

Using this matrix $\A$ as a linear operator we can compute the spring vectors for
each edge:

$$
\v = \A \p  ↔ \v_{ij} = \p_i - \p_j.
$$

We can now write the modified potential energy of $\tilde{E}$ in matrix form:

$$
\left(\sum\limits_{ij} ½k‖(\p_i-\p_j) - \d_{ij}‖\^2\right)  = \\\\
\frac{k}{2} \tr{(\A \p - \d)^\top (\A \p - \d)},
$$
where we stack the vector $\d_{ij}$ for each edge in the corresponding rows of $\d∈\R^{m × 3}$.


Combining our two matrix expressions together we can write $\tilde{E}$ entirely
in matrix form:

$$\tilde{E}(\p) = \\\\
\frac{k}{2} \tr{(\A \p - \d)^\top (\A \p - \d)} + 
\tr{
\left(\p - 2\p\^{t} + \p\^{t-∆t}\right)^\top \M \left(\p - 2\p\^{t} + \p\^{t-∆t}\right)
} 
\tr{\p^\top \f^\text{ext}} = \\\\
\frac{1}{2} \tr{ \p^\top (k \A^\top \A + \frac{1}{∆t²}\M) \p }
- \tr{\p^\top(k \A^\top \d + \frac{1}{∆t²}\M (2\p\^t - \p\^{t-∆t}) + \f^\text{ext})} + \text{ constants }.
$$

> **Question:** Why do we not bother to write out the terms that are constant with
> respect to $\p$?

We can clean this up by introducing a few auxiliary matrices:

$$
\Q := (k \A\^\top \A + \frac{1}{∆t²}\M) ∈ \R\^{n×n} \\\\
\y := \frac{1}{∆t²}\M (2\p\^t - \p\^{t-∆t}) + \f^\text{ext} ∈ \R\^{n×3} \\\\
\b := k \A\^\top \d + \y ∈ \R\^{n×3}.
$$

Now our optimization problem is neatly written as:

$$
\p\^{t+∆t} = \argmin_\p ½ \tr{ \p\^\top \Q \p } - \tr{\p\^\top \b}.
$$

> **Recall:** The trace operator behaves very nicely when differentiating.
>
> $$\frac{∂ \tr{\x^\top \y}}{∂ \x} = \y$$
> and 
>
> $$\frac{∂ ½\tr{\x^\top \Y \x}}{∂ \x} = \Y \x$$
>

Taking a derivative with respect to $\p$ and setting the expression to zero
reveals the minimizer of this quadratic energy:

$$
\Q \p\^{t+∆t} = \b
$$

Since $\Q$ is a square invertible matrix we can _solve_ this system, which we
often write as:

$$
\p\^{t+∆t} = \Q\^{-1} \b.
$$

#### Solving as the _action_ of multiplying by a matrix's inverse

From an algorithmic point of view the notation $\p = \Q^{-1} \b$ is misleading. It
might suggest first constructing `Qinv = inverse(Q)` and then conducting matrix
multiply `p = Qinv * b`. This is almost always a bad idea. Constructing `Qinv` 
be very expensive $O(n³)$ and numerically unstable.

Instead, we should think of the _action_ of multiplying by the inverse of a
matrix as a single "solve" operation: `p = solve(Q,b)`. Some programming
languages (such as MATLAB) indicate using operator overloading "matrix
division": `p = Q \ b`.

All good matrix libraries (including [Eigen](http://eigen.tuxfamily.org)) will
implement this "solve" action. A very common approach is to compute a 
factorization of the matrix into a 
[lower triangular matrix](https://en.wikipedia.org/wiki/Triangular_matrix)
times it's transpose:
$$
\Q = \L \L^\top.
$$
Finding this $\L$ matrix takes $O(n³)$ time in general.

The action of solving against a triangular matrix is simple
[forward-/back-substitution](https://en.wikipedia.org/wiki/Triangular_matrix#Forward_and_back_substitution)
and takes $O(n²)$ time. We can conceptually rewrite our system as 
$\Q \p = \b$ with $\L \L^\top \p = \b$.

A key insight of the Liu et al. paper is that our $\Q$ matrix is always same
(regardless of the iterations in our algorithm above and even regardless of the
time $t$ that we're computing positions for).  We can split our solve routine
into two steps: precomputation done once when the mass-spring system is loaded
in and fast substitution at run-time:

```
// Once Q is known
L = precompute_factorization(Q)
// ... each time step
// ... ... each iteration
p = back_substitution(transpose(L),forward_substitution(L,b))
```

### Sparse Matrices

For small mass spring systems, $O(n³)$ at loading time and $O(n²)$ at runtime
may be acceptable. But for even medium sized systems this will become
intractable $(n=1000 → n³=1,000,000,000.)$

Fortunately, we can avoid this worst-case behavior by observing a special
structure in our matrices. Let's start with the mass matrix $\M ∈ \R^{n×n}$. All
of the values of this matrix are zero except the diagonal. Storing this as a
general matrix we would be storing $n²-n$ zeros. Instead, we can acknowlede that
this matrix is [sparse](https://en.wikipedia.org/wiki/Sparse_matrix) and store
only the non-zeros along the diagonal.

Similarly, the matrix $\A^{m×n}$ has $2m$ non-zeros (a $+1$ and $-1$ per edge)
and the other $mn-2m$ entries are zero. Furthermore, the result of the product $\A^\top\A$ and by
extension $\Q ∈ \R^{n×n}$ will mostly contain zeros. The number of non-zeros is
in fact $O(m + n)$. Large mass-spring systems tend to have $m=O(n)$ edges, so we
can happily think of the number of non-zeros as $O(n)$.

We've reduced the storage required from $O(n²)$ to $O(n)$.  What's the catch?
General (or "dense") matrices can be easily mapped to memory linearly. For a an
arbitrary sparse matrix, we need store additional information to know _where_
each non-zero entry is. The most common general approach is to stored a sorted
list of values in each column (or row) of the matrix. This is a rather awkward
data-structure to manipulate directly. Similar to the pitfalls of [bubble
sort](https://en.wikipedia.org/wiki/Insertion_sort), inserting values one at a
time can be quite slow since we'd have to keep the lists sorted after each
operation. 

Because of this most sparse matrix libraries require (or prefer) to insert all
entries at once and presort non-zeros indices prefer creating the datastructure.
Friendly sparse matrix libraries like Eigen, will let us create a list list of
$(i,j,v)$ triplets for each non-zero and then insert all values. 

So if our dense matrix code looked something like:

```
Afull = zero(m,n)
for each pair i j
  Afull(i,j) += v
end
```

> By convention we use `+=` instead of `=` to allow for repeated $(i,j)$ pairs
> in the list. 

then we can replace this with 

```
triplet_list = []
for each pair i j
  triplet_list.append( i, j, v)
end
Asparse = construct_from_triplets( triplet_list )
```

> **Warning:**
>
> Do not attempt to set values of a sparse matrix directly. That is, **_do
> not_** write:
>
> ```
> A_sparse(i,j) = v
> ```
>

Storing only the non-zero entries means we must rewrite all basic matrix
operations including (matrix-vector product, matrix addition, matrix-matrix
product, transposition, etc.). This is outside the scope of our assignment and
we will use Eigen's `SparseMatrix` class.
 
Most important to our mass spring system is the _solve action_ discussed above.
Similar to the dense case, we can precompute a factorization and use
substitution at runtime. For our sparse matrix, these steps will
be $O(n^{≈1.5})$, with substitution faster and nearly $O(n)$.

### Pinned Vertices

Subject to the external force of gravity in $\f^\text{ext}$ our spring networks
will just accelerate downward off the screen.

We can pin down vertices (e.g., those listed in `b`) at their intial positions,
by requiring that their corresponding positions values $\p_i$ are always forced
to be equal to their initial values $\p^\text{rest}_b$:

$$
\p_i = \p^\text{rest}_i \ ∀ i \text{ in pinned vertices}.
$$

There are various ways we can introduce this simple linear equality constraint
into the energy optimization above. For this assignment, we'll use the
easy-to-implement [penalty
method](https://en.wikipedia.org/wiki/Penalty_method). We will add an additional
quadratic energy term which is minimized when our pinning constraints are
satisfied:

$$
\frac{w}{2} \sum\limits_{i \text{ in pinned vertices}} ‖\p_i - \p\^\text{rest}_i ‖\^2,
$$

where the $w$ should be set to some large value (e.g., `w=1e10`). We can write this in matrix form as:

$$
\frac{w}{2} \tr{(\C \p - \C \p\^\text{rest})^\top(\C \p - \C \p\^\text{rest})}  = \\\\
\frac{1}{2} \tr{\p\^\top (w \C\^\top \C) \p} - \tr{\p^\top w\C^\top \p\^\text{rest}} + \text{constant}
$$

where $\C \in \R^{|\text{pinned}| × n}$ has one row per pinned vertex with a
$+1$ in the corresponding column.

We can add these quadratic and linear coefficients to $\Q$ and $\b$ above correspondingly.

## Tasks

### White List

- `Eigen::Triplet`

### Black List

- `igl::edge_lengths`
- `igl::diag`
- `igl::sparse`
- `igl::massmatrix`
- `.sparseView()` on `Eigen::MatrixXd` types

Write your dense code first. This will be simpler to debug.

### `src/signed_incidence_matrix_dense.cpp`

### `src/fast_mass_springs_precomputation_dense.cpp`

### `src/fast_mass_springs_step_dense.cpp`

At this point you should be able to run on small examples.

For example, running `./masssprings_dense ../data/single-spring-horizontal.json`
should produce a swinging, bouncing spring:

![](images/single-spring-horizontal.gif)

If the single spring example is not working, debug immediately before proceeding
to examples with more than one spring.

Running `./masssprings_dense ../data/horizontal-chain.json`
will produce a hanging [catenary chain](https://en.wikipedia.org/wiki/Catenary):

![](images/horizontal-chain.gif)

Running `./masssprings_dense ../data/net.json`
will produce a hanging [catenary chain](https://en.wikipedia.org/wiki/Catenary):

![](images/net.gif)

If you try to run `./masssprings_dense ../data/flag.json` you'll end up waiting
a while. 

Start your sparse implementations by copying-and-pasting your correct dense
code. Remove any dense operations and construct all matrices using triplet lists.

### `src/signed_incidence_matrix_sparse.cpp`

### `src/fast_mass_springs_precomputation_sparse.cpp`

### `src/fast_mass_springs_step_sparse.cpp`

Now you should be able to see more complex examples, such as running
`./masssprings_sparse ../data/flag.json` or `./masssprings_sparse ../data/skirt.json`:

![](images/skirt.gif)
