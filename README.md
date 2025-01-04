# Gilbert–Johnson–Keerthi
<p>Using the Gilbert–Johnson–Keerthi Algorithm to detect collisions</p>
<p>For debugging purposes, the first step is to detect collisions between objects and the camera</p>
<p>These are the ultimate goals by implementing the GJK algorithm and raycasting.</p>
<ul>
    <li>Polygon-Camera collision ✓</li>
    <li>Collsions with raycasting ✓</li>
    <li>Mouse direction/ray collision ✓</li>
    <li>Polygon-Polygon collision</li>
</ul>

# Theory
## 1. Minkowski Difference between two convex shapes $ A $ and $ B $ is given by:
$$
M(A, B) = A - B = \{ \mathbf{a} - \mathbf{b} \mid \mathbf{a} \in A, \mathbf{b} \in B \}
$$

<p>Where:</p>

$ A $ and $ B $ are convex shapes
<p></p>

$ a $ and $ b $ are points on the boundaries of shapes $ A $ and $ B $

## 2. Support function as defined by $ S(X, d) $ for convex shape $ X $:

$$ S(X, d) = arg max[x \in X]d⋅x $$

<p>Where:</p>

$S(X, d)$ is the support vertex on shape $X$ in the direction $d$
<p></p>

$x$ is a boundary on $X$
<p></p>

$d⋅x$ is the dot product between the direction vector $d$ and point $x$

## 3. Direction updates

<p>The GJK algorithm uses the direction vector which is updated at each iteration. If at any point the algorithm reaches a simplex, the direction is updated towards the closest point/vertex on the convex shape.</p>

## 4. Simplex and the closest points

<p>At each iteration through the Minkowski difference, it derives for simplex formations (line, triangle, or tetrahedron). A simplex is formed by a group of points where the closest point from the origin is computed.</p>
<p></p>

Such as for a 2D triangle formed by points $v_0$, $v_1$, $v_2$, the closest point is computed with dot products and cross products.
