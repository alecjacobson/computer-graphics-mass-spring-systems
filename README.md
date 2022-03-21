# Computer Graphics – Mass-Spring Systems

> **To get started:** Clone this repository using
> 
>     git clone --recursive http://github.com/alecjacobson/computer-graphics-mass-spring-systems.git
>

![](images/flag.gif)

## Background

### Read Chapter 16.5 of _Fundamentals of Computer Graphics (4th Edition)_.

### Read ["Fast Simulation of Mass-Spring Systems" [Tiantian Liu et al. 2013]](https://www.cs.utah.edu/~ladislav/liu13fast/liu13fast.pdf)

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
<img src="svgs/6337ee87446dee40530f55558466bedc.svg?invert_in_darkmode&sanitize=true" align=middle width=48.390209999999996pt height=26.76201pt/> acting on a body must equal its mass <img src="svgs/0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode&sanitize=true" align=middle width=14.43321pt height=14.15535pt/> times its acceleration
<img src="svgs/29a223b1732e4aa561aec5a87741a53d.svg?invert_in_darkmode&sanitize=true" align=middle width=50.01116999999999pt height=26.76201pt/>:


<p align="center"><img src="svgs/2f4cd85a2f21158bb638f242fd22b4f8.svg?invert_in_darkmode&sanitize=true" align=middle width=57.67476pt height=11.415524999999999pt/></p>

Notice that <img src="svgs/47b0192f8f0819d64bce3612c46d15ea.svg?invert_in_darkmode&sanitize=true" align=middle width=7.568516999999999pt height=22.83138pt/> and <img src="svgs/41f28962986ecdd9c1dc2af8b83fef84.svg?invert_in_darkmode&sanitize=true" align=middle width=9.189509999999999pt height=14.61207pt/> are vectors, each having a magnitude and a direction.
We will build our computational simulation by asking for this equation to be
true for each point mass  in our network. The forces <img src="svgs/a3c2e802fc6ad252375c413a1b6ac2c6.svg?invert_in_darkmode&sanitize=true" align=middle width=10.427174999999998pt height=22.83138pt/> acting on the <img src="svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode&sanitize=true" align=middle width=5.663295pt height=21.68331pt/>-th point
mass are simply the sum of forces coming from any incident spring edge <img src="svgs/e5a8bc7bac1dd7d337c9e609a4ae3f99.svg?invert_in_darkmode&sanitize=true" align=middle width=13.373745pt height=21.68331pt/> and
any external force (such as gravity).

[Personifying](https://en.wikipedia.org/wiki/Personification) physical objects, we say
that they are _at rest_ when their potential energy is zero. When the object is
_not at rest_ then it exerts a force pushing it toward its rest state ([elastic
force](https://en.wikipedia.org/wiki/Elasticity_(physics))), decreasing its
potential energy as fast as possible. The force is the negative gradient of the potential
energy.

A simple spring is defined by its stiffness <img src="svgs/f9bbd08bf846520586581437c960abac.svg?invert_in_darkmode&sanitize=true" align=middle width=39.21225pt height=22.83138pt/> and _rest_ length <img src="svgs/3b2dcd4e3bd8001e045112d599b79c00.svg?invert_in_darkmode&sanitize=true" align=middle width=53.262825pt height=22.557149999999996pt/>.
Its potential energy measures the squared difference of the current length and
the rest length times the stiffness:

<p align="center"><img src="svgs/318e8a5b7d56a623b6be300b71057922.svg?invert_in_darkmode&sanitize=true" align=middle width=243.63899999999998pt height=32.9901pt/></p>

![](images/potential-energy.png)

The force exerted by the spring on each mass is the [partial
derivative](https://en.wikipedia.org/wiki/Partial_derivative) of the potential
energy <img src="svgs/a9a3a4a202d80326bda413b5562d5cd1.svg?invert_in_darkmode&sanitize=true" align=middle width=13.242074999999998pt height=22.46574pt/> with respect to the corresponding mass position. For example, for
<img src="svgs/f13e5bc0860402c82f869bcf883eb8b0.svg?invert_in_darkmode&sanitize=true" align=middle width=15.15327pt height=14.61207pt/> we have

<p align="center"><img src="svgs/419344ec3d3de763aff73f0cd0d1105b.svg?invert_in_darkmode&sanitize=true" align=middle width=127.826985pt height=37.00851pt/></p>

For now, we can postpone expanding <img src="svgs/b2006c0d00d8438cf4513b1570d2dc0b.svg?invert_in_darkmode&sanitize=true" align=middle width=54.068685pt height=24.6576pt/>, and just recognize that it is a
3D vector. 

Our problem is to determine _where_ all of the mass will be after a small
duration in time (<img src="svgs/5a63739e01952f6a63389340c037ae29.svg?invert_in_darkmode&sanitize=true" align=middle width=19.634835pt height=22.46574pt/>). 

> **Question:** What is a reasonable choice for the value of <img src="svgs/5a63739e01952f6a63389340c037ae29.svg?invert_in_darkmode&sanitize=true" align=middle width=19.634835pt height=22.46574pt/> ?
>
> **Hint:** 🎞️ or 🖥️
>

We'll assume we know the current positions for each
mass <img src="svgs/e0b9d9299580e2b6a314b1ce43960e1e.svg?invert_in_darkmode&sanitize=true" align=middle width=57.111615pt height=26.76201pt/> at the current time (<img src="svgs/4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode&sanitize=true" align=middle width=5.9361555pt height=20.22207pt/>) and the current velocities
<img src="svgs/fd37fc619ea79b16b7a3654ca0cd9e5d.svg?invert_in_darkmode&sanitize=true" align=middle width=147.162015pt height=26.76201pt/>. When <img src="svgs/1c899e1c767eb4eac89facb5d1f2cb0d.svg?invert_in_darkmode&sanitize=true" align=middle width=36.07296pt height=21.18732pt/> then we call these the [initial
conditions](https://en.wikipedia.org/wiki/Initial_condition) of the entire
simulation. For <img src="svgs/41163b95295e685e3f25bc73af21a8fb.svg?invert_in_darkmode&sanitize=true" align=middle width=36.07296pt height=21.18732pt/>, we can still think of these values as the initial
conditions for the remaining time.

In the real world, the trajectory of an object follows a continuous curve as a
function of time. In our simulation, we only need to know the position of each
pass at [discrete moments in
time](https://en.wikipedia.org/wiki/Discrete_time_and_continuous_time). We use
this to build discrete approximation of the time derivatives (velocities and
accelerations) that we encounter. Immediately, we can replace the current
velocties <img src="svgs/0b39cd3b1c07dcc4dcbea210f38358ec.svg?invert_in_darkmode&sanitize=true" align=middle width=15.468089999999998pt height=26.086169999999996pt/> with a _backward_ [finite
difference](https://en.wikipedia.org/wiki/Finite_difference) of the positions
over the small time step:

<p align="center"><img src="svgs/245add6aa6b0df39512ea104d7125f08.svg?invert_in_darkmode&sanitize=true" align=middle width=118.89008999999999pt height=37.291484999999994pt/></p>

where <img src="svgs/612ecaae0105899fb8c4b10424449b4a.svg?invert_in_darkmode&sanitize=true" align=middle width=83.150595pt height=29.79009pt/> is the position at the _previous_ time.

We can also use a _central_ finite difference to define the acceleration at time
<img src="svgs/4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode&sanitize=true" align=middle width=5.9361555pt height=20.22207pt/>:

<p align="center"><img src="svgs/9f0117ddac55126872d30e373e4fa435.svg?invert_in_darkmode&sanitize=true" align=middle width=621.8817pt height=37.291484999999994pt/></p>

This expression mentions our _unknown_ variables <img src="svgs/60cd486c9b59db93b3a4fad0605ccebb.svg?invert_in_darkmode&sanitize=true" align=middle width=41.324415pt height=29.79009pt/> for the first
time. We'll soon that based on definition of the potential spring energy above
and the acceleration here we can _solve_ for the values of these unknown
variables.

### Time integration as energy optimization

In the equation <img src="svgs/f428e42f1278299d021e503dfeb2f94f.svg?invert_in_darkmode&sanitize=true" align=middle width=53.108715pt height=22.83138pt/>, the acceleration term <img src="svgs/41f28962986ecdd9c1dc2af8b83fef84.svg?invert_in_darkmode&sanitize=true" align=middle width=9.189509999999999pt height=14.61207pt/> depends _linearly_ on the
unknowns <img src="svgs/61cf41460b5d444b8df90703eb5ef637.svg?invert_in_darkmode&sanitize=true" align=middle width=41.324415pt height=27.65697pt/>. Unfortunately, even for a simple spring the forces <img src="svgs/e0fce5230e658c0ee1a58d7c155de809.svg?invert_in_darkmode&sanitize=true" align=middle width=109.72599000000001pt height=27.65697pt/> depend _non-linearly_ on <img src="svgs/61cf41460b5d444b8df90703eb5ef637.svg?invert_in_darkmode&sanitize=true" align=middle width=41.324415pt height=27.65697pt/>. This means we have a
_non-linear_ system of equations, which can be tricky to satisfy directly.

<!--
If we expanded this as an expression, we
might write:
<p align="center"><img src="svgs/c50231b6047e999b94a458fb5b3282a9.svg?invert_in_darkmode&sanitize=true" align=middle width=293.08125pt height=49.31553pt/></p>
-->

> **Question:** We've _chosen_ to define <img src="svgs/47b0192f8f0819d64bce3612c46d15ea.svg?invert_in_darkmode&sanitize=true" align=middle width=7.568516999999999pt height=22.83138pt/> as the forces that implicitly
> depend on the unknown positions <img src="svgs/61cf41460b5d444b8df90703eb5ef637.svg?invert_in_darkmode&sanitize=true" align=middle width=41.324415pt height=27.65697pt/> at the end of the
> time step <img src="svgs/f49b1cd442802e4abaee5b7044484875.svg?invert_in_darkmode&sanitize=true" align=middle width=45.662099999999995pt height=22.46574pt/>. What would happen if we defined the forces to explicitly
> depend on the (known) current positions <img src="svgs/be91be346dbdf689f26da562e9b8bc99.svg?invert_in_darkmode&sanitize=true" align=middle width=15.468089999999998pt height=26.086169999999996pt/>?

An alternative is to view physics simulation as an optimization problem. We
will define an energy that will be minimized by the value of <img src="svgs/61cf41460b5d444b8df90703eb5ef637.svg?invert_in_darkmode&sanitize=true" align=middle width=41.324415pt height=27.65697pt/> that
satisfies <img src="svgs/f428e42f1278299d021e503dfeb2f94f.svg?invert_in_darkmode&sanitize=true" align=middle width=53.108715pt height=22.83138pt/>. The minimizer <img src="svgs/980fcd4213d7b5d2ffcc82ec78c27ead.svg?invert_in_darkmode&sanitize=true" align=middle width=10.50225pt height=14.61207pt/> of some function <img src="svgs/0dc5590ba457f7e2998c0ed49ab7b31f.svg?invert_in_darkmode&sanitize=true" align=middle width=35.262645pt height=24.6576pt/> will satisfy
<img src="svgs/8146e66aa14c7988c2ba77b40286b1cd.svg?invert_in_darkmode&sanitize=true" align=middle width=81.22125pt height=24.6576pt/>. So we construct an energy <img src="svgs/84df98c65d88c6adf15d4645ffa25e47.svg?invert_in_darkmode&sanitize=true" align=middle width=13.08219pt height=22.46574pt/> such that <img src="svgs/7dc060b03cb3a15a83ee204d181c131e.svg?invert_in_darkmode&sanitize=true" align=middle width=124.28427pt height=24.6576pt/>:

<p align="center"><img src="svgs/76195156f5590387ccb31b9616aff758.svg?invert_in_darkmode&sanitize=true" align=middle width=731.3047499999999pt height=85.26078pt/></p> 

Keen observers will identify that the first term is potential energy and the
second term resembles [kinetic
energy](https://en.wikipedia.org/wiki/Kinetic_energy). Intuitively, we can see
the first term as trying to return the spring to rest length (elasticity) and
the second term as trying to keep masses [moving in the same
direction](https://en.wikipedia.org/wiki/Newton%27s_laws_of_motion#Newton%27s_first_law). 

Because of the <img src="svgs/012ed0635e8ecc5daafe75725d6dea0f.svg?invert_in_darkmode&sanitize=true" align=middle width=108.19627499999999pt height=24.6576pt/> term, minimizing <img src="svgs/84df98c65d88c6adf15d4645ffa25e47.svg?invert_in_darkmode&sanitize=true" align=middle width=13.08219pt height=22.46574pt/> is a non-linear
optimization problem. The standard approach would be to apply [gradient
descent](https://en.wikipedia.org/wiki/Gradient_descent) (slow), [Gauss-Newton
method](https://en.wikipedia.org/wiki/Gauss–Newton_algorithm), or [Newton's
Method](https://en.wikipedia.org/wiki/Newton%27s_method_in_optimization) (too
complicated for this assignment).

In a relatively recent SIGGRAPH paper ["Fast Simulation of Mass-Spring
Systems"](http://graphics.berkeley.edu/papers/Liu-FSM-2013-11/Liu-FSM-2013-11.pdf),
Tiantian Liu et al. made a neat observation that makes designing an algorithm to
minimize <img src="svgs/84df98c65d88c6adf15d4645ffa25e47.svg?invert_in_darkmode&sanitize=true" align=middle width=13.08219pt height=22.46574pt/> quite simple and fast. For each spring <img src="svgs/e5a8bc7bac1dd7d337c9e609a4ae3f99.svg?invert_in_darkmode&sanitize=true" align=middle width=13.373745pt height=21.68331pt/>, they observe that the
non-linear energy can be written as a small optimization problem:

<p align="center"><img src="svgs/b048d7d1d98be7eb5965b404323e0808.svg?invert_in_darkmode&sanitize=true" align=middle width=388.13775pt height=28.81461pt/></p>

It may seem like we've just created extra work. We took a closed-form expression 
(left) and replaced it with an optimization problem (right). Yet this
optimization problem is small (<img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> is a single 3D vector) and can be
easily solved _independently_ (and even in parallel) for each spring (i.e.,
<img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> doesn't depend on <img src="svgs/7ccb533cd16fff60e80c336cf8aeef37.svg?invert_in_darkmode&sanitize=true" align=middle width=23.27061pt height=22.83138pt/> etc.). Reading the right-hand side in
English it says, find the vector of length <img src="svgs/92e0822b1528090efc2435d2ae60c9ee.svg?invert_in_darkmode&sanitize=true" align=middle width=18.17178pt height=14.15535pt/> that is as close as possible
to the current spring vector <img src="svgs/64aaef2d00caf95ec5f213a01b58f662.svg?invert_in_darkmode&sanitize=true" align=middle width=52.67294999999999pt height=19.17828pt/>. 

![](images/dij-rij-closest-vector.png)


Now, suppose we somehow _knew already_ the vector <img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> corresponding to the
_unknown_ optimal solution <img src="svgs/61cf41460b5d444b8df90703eb5ef637.svg?invert_in_darkmode&sanitize=true" align=middle width=41.324415pt height=27.65697pt/>, then treating <img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> as a _constant_ we could
find the optimal solution by solving the _quadratic_ optimization problem:

<p align="center"><img src="svgs/d3dc7494d872489937b2ef87313ca469.svg?invert_in_darkmode&sanitize=true" align=middle width=741.6964499999999pt height=87.27906pt/></p> 

The modified energy <img src="svgs/5fc3d1ec40b9a3acc07496c9f0e7f577.svg?invert_in_darkmode&sanitize=true" align=middle width=36.36996pt height=30.2676pt/> is _quadratic_ with respect to the unknowns
<img src="svgs/980fcd4213d7b5d2ffcc82ec78c27ead.svg?invert_in_darkmode&sanitize=true" align=middle width=10.50225pt height=14.61207pt/>, therefore the solution is found when we set the first derivative equal to
zero: 

<p align="center"><img src="svgs/639a82aadbc9a66011d72b222f97bfec.svg?invert_in_darkmode&sanitize=true" align=middle width=58.313805pt height=40.72662pt/></p>

This leads to a straightforward "local-global" iterative algorithm:

 - Step 1 (local): Given current values of <img src="svgs/980fcd4213d7b5d2ffcc82ec78c27ead.svg?invert_in_darkmode&sanitize=true" align=middle width=10.50225pt height=14.61207pt/> determine <img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> for each
   spring.
 - Step 2 (global): Given all <img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> vectors, find positions <img src="svgs/980fcd4213d7b5d2ffcc82ec78c27ead.svg?invert_in_darkmode&sanitize=true" align=middle width=10.50225pt height=14.61207pt/> that
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
<img src="svgs/4d5f39204df8cdb5837a5bd197c9a57c.svg?invert_in_darkmode&sanitize=true" align=middle width=49.23567pt height=34.241460000000004pt/>) and the [quadratic
forms](https://en.wikipedia.org/wiki/Quadratic_form) (e.g., in the energy
<img src="svgs/9674605b99e37d0dbca0fc51ec6b1bc7.svg?invert_in_darkmode&sanitize=true" align=middle width=13.08219pt height=30.2676pt/>).

Let's begin by being precise about some notation. We will stack up all of the
<img src="svgs/55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode&sanitize=true" align=middle width=9.867pt height=14.15535pt/> unknown mass positions <img src="svgs/4b1d092e4a9a112ea2e69c8b7c2c61c2.svg?invert_in_darkmode&sanitize=true" align=middle width=56.796794999999996pt height=26.76201pt/> as the rows of a matrix <img src="svgs/eedb351c2dc2a62725645049d96e217d.svg?invert_in_darkmode&sanitize=true" align=middle width=69.72404999999999pt height=26.76201pt/>.
We can do the same for the _known_ previous time steps' positions
<img src="svgs/36b373ed95b016fb214ef52b36dc5010.svg?invert_in_darkmode&sanitize=true" align=middle width=125.146395pt height=27.65697pt/>.

We can then express the inertial term using matrices:
<p align="center"><img src="svgs/edef44c91a1772228d8d6de8ddeeba32.svg?invert_in_darkmode&sanitize=true" align=middle width=688.7198999999999pt height=98.744085pt/></p>

where <img src="svgs/797d20fec02db91e546ce25d31c18bf5.svg?invert_in_darkmode&sanitize=true" align=middle width=42.648375pt height=24.6576pt/> computes the [trace](https://en.wikipedia.org/wiki/Trace_(linear_algebra)) of <img src="svgs/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode&sanitize=true" align=middle width=14.292300000000001pt height=22.557149999999996pt/> (sums up the diagonal entries: <img src="svgs/3d30c855ff8804d9162b1ba648834929.svg?invert_in_darkmode&sanitize=true" align=middle width=115.79881499999999pt height=22.557149999999996pt/>).

and the entries of the square matrix <img src="svgs/35eaa614ad74ef19a4a94c079c27b637.svg?invert_in_darkmode&sanitize=true" align=middle width=78.74047499999999pt height=26.17758pt/> are set to 

<p align="center"><img src="svgs/0c054b82efaa4e2ed9d99fd3ee6221f7.svg?invert_in_darkmode&sanitize=true" align=middle width=189.43815pt height=69.041775pt/></p>

The potential energy term can be similarly written with matrices. We'll start by
introducing the _signed incidence_ matrix of our mass-psring network of <img src="svgs/55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode&sanitize=true" align=middle width=9.867pt height=14.15535pt/>
vertices and <img src="svgs/0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode&sanitize=true" align=middle width=14.43321pt height=14.15535pt/> edges <img src="svgs/35d615023140b8914fbc142ac3bb1336.svg?invert_in_darkmode&sanitize=true" align=middle width=78.62629499999998pt height=26.17758pt/>. The _rows_ of <img src="svgs/96458543dc5abd380904d95cae6aa2bc.svg?invert_in_darkmode&sanitize=true" align=middle width=14.292300000000001pt height=22.557149999999996pt/> correspond to an arbitrary
(but fixed) ordering of the edges in the network. In a mass-spring network, the
edges are un-oriented in the sense that the spring acts symmetrically on its
vertices. For convenience, we'll pick an orientation for edge anyway. For the
<img src="svgs/8cd34385ed61aca950a6b06d09fb50ac.svg?invert_in_darkmode&sanitize=true" align=middle width=7.6542015pt height=14.15535pt/>-th edge <img src="svgs/e5a8bc7bac1dd7d337c9e609a4ae3f99.svg?invert_in_darkmode&sanitize=true" align=middle width=13.373745pt height=21.68331pt/>, we should be sure to use the same orientation when computing
<img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> and for the following entries of <img src="svgs/96458543dc5abd380904d95cae6aa2bc.svg?invert_in_darkmode&sanitize=true" align=middle width=14.292300000000001pt height=22.557149999999996pt/>. So, for the <img src="svgs/8cd34385ed61aca950a6b06d09fb50ac.svg?invert_in_darkmode&sanitize=true" align=middle width=7.6542015pt height=14.15535pt/>-th row of <img src="svgs/96458543dc5abd380904d95cae6aa2bc.svg?invert_in_darkmode&sanitize=true" align=middle width=14.292300000000001pt height=22.557149999999996pt/>
corresponding to edge connecting vertices <img src="svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode&sanitize=true" align=middle width=5.663295pt height=21.68331pt/> and <img src="svgs/36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode&sanitize=true" align=middle width=7.710483pt height=21.68331pt/> we'll assign values:

<p align="center"><img src="svgs/7a805f65b1821701404dc10387e0ad77.svg?invert_in_darkmode&sanitize=true" align=middle width=205.76819999999998pt height=118.357305pt/></p>

Using this matrix <img src="svgs/96458543dc5abd380904d95cae6aa2bc.svg?invert_in_darkmode&sanitize=true" align=middle width=14.292300000000001pt height=22.557149999999996pt/> as a linear operator we can compute the spring vectors for
each edge:

<p align="center"><img src="svgs/08920ab7a3528fb8d252f81d62c41091.svg?invert_in_darkmode&sanitize=true" align=middle width=184.05585pt height=15.9817185pt/></p>

We can now write the modified potential energy of <img src="svgs/9674605b99e37d0dbca0fc51ec6b1bc7.svg?invert_in_darkmode&sanitize=true" align=middle width=13.08219pt height=30.2676pt/> in matrix form:

<p align="center"><img src="svgs/d9867749b79e88915bc85196bccc511e.svg?invert_in_darkmode&sanitize=true" align=middle width=429.08084999999994pt height=59.178735pt/></p>

where we stack the vector <img src="svgs/27ad4bcb9a515705743055d231c7d7c5.svg?invert_in_darkmode&sanitize=true" align=middle width=21.257775pt height=22.83138pt/> for each edge in the corresponding rows of <img src="svgs/39b5c8193ccaa0606e2ad1b2af6c2c65.svg?invert_in_darkmode&sanitize=true" align=middle width=73.26280499999999pt height=26.76201pt/>.


Combining our two matrix expressions together we can write <img src="svgs/9674605b99e37d0dbca0fc51ec6b1bc7.svg?invert_in_darkmode&sanitize=true" align=middle width=13.08219pt height=30.2676pt/> entirely
in matrix form:

<p align="center"><img src="svgs/20b527fb5c733058667e0de48fecf678.svg?invert_in_darkmode&sanitize=true" align=middle width=732.8046pt height=79.83986999999999pt/></p>

> **Question:** Why do we not bother to write out the terms that are constant with
> respect to <img src="svgs/980fcd4213d7b5d2ffcc82ec78c27ead.svg?invert_in_darkmode&sanitize=true" align=middle width=10.50225pt height=14.61207pt/>?

We can clean this up by introducing a few auxiliary matrices:

<p align="center"><img src="svgs/bd7e1e8f880815c8a5bee3f04c7cb457.svg?invert_in_darkmode&sanitize=true" align=middle width=294.88965pt height=97.10547pt/></p>

Now our optimization problem is neatly written as:

<p align="center"><img src="svgs/a66d510d67403c77f7e051f211662a7b.svg?invert_in_darkmode&sanitize=true" align=middle width=296.31854999999996pt height=37.01115pt/></p>

> **Recall:** The trace operator behaves very nicely when differentiating.
>
> <p align="center"><img src="svgs/650532e6d415fe94decf282253b94033.svg?invert_in_darkmode&sanitize=true" align=middle width=108.461925pt height=37.412924999999994pt/></p>
> and 
>
> <p align="center"><img src="svgs/f073e73746c3abad28587eb4a4f5e47a.svg?invert_in_darkmode&sanitize=true" align=middle width=147.01896pt height=37.412924999999994pt/></p>
>

Taking a derivative with respect to <img src="svgs/980fcd4213d7b5d2ffcc82ec78c27ead.svg?invert_in_darkmode&sanitize=true" align=middle width=10.50225pt height=14.61207pt/> and setting the expression to zero
reveals the minimizer of this quadratic energy:

<p align="center"><img src="svgs/bbb02a616d809294bc90179f34caeabd.svg?invert_in_darkmode&sanitize=true" align=middle width=61.689209999999996pt height=14.611871999999998pt/></p>

Since <img src="svgs/61ccc6d099c3b104d8de703a10b20230.svg?invert_in_darkmode&sanitize=true" align=middle width=14.20089pt height=22.557149999999996pt/> is a square invertible matrix we can _solve_ this system, which we
often write as:

<p align="center"><img src="svgs/eee7831efc046d1f55b39a687aa08b25.svg?invert_in_darkmode&sanitize=true" align=middle width=79.33761pt height=17.399085pt/></p>

#### Solving as the _action_ of multiplying by a matrix's inverse

From an algorithmic point of view the notation <img src="svgs/50d36af977c46318008201eb650030fd.svg?invert_in_darkmode&sanitize=true" align=middle width=74.7714pt height=26.76201pt/> is misleading. It
might suggest first constructing `Qinv = inverse(Q)` and then conducting matrix
multiply `p = Qinv * b`. This is almost always a bad idea. Constructing `Qinv` 
be very expensive <img src="svgs/b85a093314c0f482cded300a33f790b9.svg?invert_in_darkmode&sanitize=true" align=middle width=43.022265pt height=26.76201pt/> and numerically unstable.

Instead, we should think of the _action_ of multiplying by the inverse of a
matrix as a single "solve" operation: `p = solve(Q,b)`. Some programming
languages (such as MATLAB) indicate using operator overloading "matrix
division": `p = Q \ b`.

All good matrix libraries (including [Eigen](http://eigen.tuxfamily.org)) will
implement this "solve" action. A very common approach is to compute a 
factorization of the matrix into a 
[lower triangular matrix](https://en.wikipedia.org/wiki/Triangular_matrix)
times it's transpose:
<p align="center"><img src="svgs/6da94c7036f7473a9c1bb41e4a88a1fd.svg?invert_in_darkmode&sanitize=true" align=middle width=74.52027pt height=17.974439999999998pt/></p>

Finding this <img src="svgs/80637df1ca7533740cc7b3fdd1ab540b.svg?invert_in_darkmode&sanitize=true" align=middle width=11.369819999999999pt height=22.557149999999996pt/> matrix takes <img src="svgs/b85a093314c0f482cded300a33f790b9.svg?invert_in_darkmode&sanitize=true" align=middle width=43.022265pt height=26.76201pt/> time in general.

The action of solving against a triangular matrix is simple
[forward-/back-substitution](https://en.wikipedia.org/wiki/Triangular_matrix#Forward_and_back_substitution)
and takes <img src="svgs/e103cb4afcb639eecf8fda6ff0e12731.svg?invert_in_darkmode&sanitize=true" align=middle width=43.022265pt height=26.76201pt/> time. We can conceptually rewrite our system as 
<img src="svgs/dbe06987bb362e43e1f7bc44d3f4300e.svg?invert_in_darkmode&sanitize=true" align=middle width=57.12299999999999pt height=22.83138pt/> with <img src="svgs/2b85eecc6fff1c1afe338e337cdafad3.svg?invert_in_darkmode&sanitize=true" align=middle width=76.75766999999999pt height=27.912719999999997pt/>.

A key insight of the Liu et al. paper is that our <img src="svgs/61ccc6d099c3b104d8de703a10b20230.svg?invert_in_darkmode&sanitize=true" align=middle width=14.20089pt height=22.557149999999996pt/> matrix is always same
(regardless of the iterations in our algorithm above and even regardless of the
time <img src="svgs/4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode&sanitize=true" align=middle width=5.9361555pt height=20.22207pt/> that we're computing positions for).  We can split our solve routine
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

For small mass spring systems, <img src="svgs/b85a093314c0f482cded300a33f790b9.svg?invert_in_darkmode&sanitize=true" align=middle width=43.022265pt height=26.76201pt/> at loading time and <img src="svgs/e103cb4afcb639eecf8fda6ff0e12731.svg?invert_in_darkmode&sanitize=true" align=middle width=43.022265pt height=26.76201pt/> at runtime
may be acceptable. But for even medium sized systems this will become
intractable <img src="svgs/c44aa3f9209ffa667e04d8f87e0438a4.svg?invert_in_darkmode&sanitize=true" align=middle width=250.852305pt height=26.76201000000001pt/>

Fortunately, we can avoid this worst-case behavior by observing a special
structure in our matrices. Let's start with the mass matrix <img src="svgs/46cca5aa23b7d4f0ba986d15ca38e312.svg?invert_in_darkmode&sanitize=true" align=middle width=78.74047499999999pt height=26.17758pt/>. All
of the values of this matrix are zero except the diagonal. Storing this as a
general matrix we would be storing <img src="svgs/8ac8fa012ae9908a95982a20da5b3ffc.svg?invert_in_darkmode&sanitize=true" align=middle width=47.199405pt height=26.76201pt/> zeros. Instead, we can acknowlede that
this matrix is [sparse](https://en.wikipedia.org/wiki/Sparse_matrix) and store
only the non-zeros along the diagonal.

Similarly, the matrix <img src="svgs/3a87475c442cbc1195190e8687bc1d80.svg?invert_in_darkmode&sanitize=true" align=middle width=44.35711499999999pt height=26.17758pt/> has <img src="svgs/64560e95e7c73072649d794700d5fcfc.svg?invert_in_darkmode&sanitize=true" align=middle width=22.652355pt height=21.18732pt/> non-zeros (a <img src="svgs/c11fe0cea175e1b787b3403c763dc9b0.svg?invert_in_darkmode&sanitize=true" align=middle width=21.004665pt height=21.18732pt/> and <img src="svgs/e11a8cfcf953c683196d7a48677b2277.svg?invert_in_darkmode&sanitize=true" align=middle width=21.004665pt height=21.18732pt/> per edge)
and the other <img src="svgs/6a6a244d45a7c52adacfb0b3cfcafb16.svg?invert_in_darkmode&sanitize=true" align=middle width=67.04346pt height=21.18732pt/> entries are zero. Furthermore, the result of the product <img src="svgs/274cbe00364791ee073c3054b169ec9d.svg?invert_in_darkmode&sanitize=true" align=middle width=39.680355pt height=27.912719999999997pt/> and by
extension <img src="svgs/f27a7bed3895c3f995921598e6b3d130.svg?invert_in_darkmode&sanitize=true" align=middle width=74.99613pt height=26.17758pt/> will mostly contain zeros. The number of non-zeros is
in fact <img src="svgs/4ec673c850255d1482f1ae9f8d97c166.svg?invert_in_darkmode&sanitize=true" align=middle width=70.172025pt height=24.6576pt/>. Large mass-spring systems tend to have <img src="svgs/c3d4945b05e5708192987aee3c345f9d.svg?invert_in_darkmode&sanitize=true" align=middle width=71.998575pt height=24.6576pt/> edges, so we
can happily think of the number of non-zeros as <img src="svgs/1f08ccc9cd7309ba1e756c3d9345ad9f.svg?invert_in_darkmode&sanitize=true" align=middle width=35.647755pt height=24.6576pt/>.

We've reduced the storage required from <img src="svgs/e103cb4afcb639eecf8fda6ff0e12731.svg?invert_in_darkmode&sanitize=true" align=middle width=43.022265pt height=26.76201pt/> to <img src="svgs/1f08ccc9cd7309ba1e756c3d9345ad9f.svg?invert_in_darkmode&sanitize=true" align=middle width=35.647755pt height=24.6576pt/>.  What's the catch?
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
<img src="svgs/6df5ae9af75cbdd161c06e308727fe6c.svg?invert_in_darkmode&sanitize=true" align=middle width=48.415455pt height=24.6576pt/> triplets for each non-zero and then insert all values. 

So if our dense matrix code looked something like:

```
Afull = zero(m,n)
for each pair i j
  Afull(i,j) += v
end
```

> By convention we use `+=` instead of `=` to allow for repeated <img src="svgs/aa20264597f5a63b51587e0581c48f2c.svg?invert_in_darkmode&sanitize=true" align=middle width=33.46497pt height=24.6576pt/> pairs
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
be <img src="svgs/e592823f1705f2f2adbee5645981be2e.svg?invert_in_darkmode&sanitize=true" align=middle width=63.752865pt height=26.76201pt/>, with substitution faster and nearly <img src="svgs/1f08ccc9cd7309ba1e756c3d9345ad9f.svg?invert_in_darkmode&sanitize=true" align=middle width=35.647755pt height=24.6576pt/>.

### Pinned Vertices

Subject to the external force of gravity in <img src="svgs/84278aca4a52c2b38bc81753120cdfd4.svg?invert_in_darkmode&sanitize=true" align=middle width=25.456695pt height=26.086169999999996pt/> our spring networks
will just accelerate downward off the screen.

We can pin down vertices (e.g., those listed in `b`) at their intial positions,
by requiring that their corresponding positions values <img src="svgs/f13e5bc0860402c82f869bcf883eb8b0.svg?invert_in_darkmode&sanitize=true" align=middle width=15.15327pt height=14.61207pt/> are always forced
to be equal to their initial values <img src="svgs/2859ff4a8b95aa36e0a448368fdaf224.svg?invert_in_darkmode&sanitize=true" align=middle width=31.828829999999996pt height=26.086169999999996pt/>:

<p align="center"><img src="svgs/9df2286fdacf6e27b7482b5ab860fdc8.svg?invert_in_darkmode&sanitize=true" align=middle width=227.94254999999998pt height=17.928735pt/></p>

There are various ways we can introduce this simple linear equality constraint
into the energy optimization above. For this assignment, we'll use the
easy-to-implement [penalty
method](https://en.wikipedia.org/wiki/Penalty_method). We will add an additional
quadratic energy term which is minimized when our pinning constraints are
satisfied:

<p align="center"><img src="svgs/23e0fc780c411db5a4bac693ed770ed0.svg?invert_in_darkmode&sanitize=true" align=middle width=227.57625000000002pt height=40.20753pt/></p>

where the <img src="svgs/31fae8b8b78ebe01cbfbe2fe53832624.svg?invert_in_darkmode&sanitize=true" align=middle width=12.210989999999999pt height=14.15535pt/> should be set to some large value (e.g., `w=1e10`). We can write this in matrix form as:

<p align="center"><img src="svgs/a8e5540721f67b7ce7add387fd2d4fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=664.79325pt height=32.9901pt/></p>

where <img src="svgs/499f8b11566a62ae9b0a2f4b918744c0.svg?invert_in_darkmode&sanitize=true" align=middle width=112.73789999999998pt height=29.19114pt/> has one row per pinned vertex with a
<img src="svgs/c11fe0cea175e1b787b3403c763dc9b0.svg?invert_in_darkmode&sanitize=true" align=middle width=21.004665pt height=21.18732pt/> in the corresponding column.

We can add these quadratic and linear coefficients to <img src="svgs/61ccc6d099c3b104d8de703a10b20230.svg?invert_in_darkmode&sanitize=true" align=middle width=14.20089pt height=22.557149999999996pt/> and <img src="svgs/a10ec92d13e76a02b538967f6b90b345.svg?invert_in_darkmode&sanitize=true" align=middle width=10.50225pt height=22.83138pt/> above correspondingly.

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


> ## Notes for TAs editing the README
>
> This README file is too complex for [texify](https://github.com/apps/texify) to render. Use [readme2tex](https://github.com/leegao/readme2tex) locally to render the TeX to SVGs.
>
> `python -m readme2tex --output README.md README.tex.md --nocdn`
> 
> `sed -i 's/invert_in_darkmode\"/invert_in_darkmode\&sanitize=true\"/g' README.md`

