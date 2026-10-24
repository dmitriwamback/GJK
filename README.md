# Gilbert–Johnson–Keerthi
<p>The Gilbert–Johnson–Keerthi Algorithm is a collision detection algorithm paired with EPA (Expanding Polytope Algoirthm) to detect collisions between 2 convex objects.</p>

<p>The following are ultimately the goals by implementing the GJK algorithm, EPA and raycasting:</p>
<ul>
    <li>Polygon-Camera collision ✓</li>
    <li>Collsions with raycasting ✓</li>
    <li>Mouse direction/ray collision ✓</li>
    <li>Polygon-Polygon collision ✓</li>
    <li>EPA ✓</li>
</ul>

---

### Technologies:

<ul>
    <li>GLFW</li>
    <li>glew</li>
    <li>glm</li>
    <li>cmake</li>
</ul>

### How to run:

```
cmake CMakeLists.txt
cmake --build .
./GJK
```

### How to modify the scene:



# Theory on GJK and EPA
## 1. Minkowski Difference between two convex shapes $A$ and $B$ is given by:
$$
M(A, B) = A - B = \{ \vec{a} - \vec{b} \mid \vec{a} \in A, \vec{b} \in B \}
$$

<p>Where:</p>

$A$ and $B$ are convex shapes

<p></p>

$\vec{a}$ and $\vec{b}$ are points on the boundaries of shapes $A$ and $B$

## 2. Support function as defined by $S(X, \vec{d})$ for convex shape $X$:

$$ S(X, \vec{d}) = \operatorname*{arg\,max}_{\vec{x} \in X} \vec{d} \cdot \vec{x} $$

<p>Where:</p>

1. $S(X, \vec{d})$ is the support vertex on shape $X$ in the direction $\vec{d}$.

<p></p>

2. $\vec{x}$ is a boundary on $X$.

<p></p>

3. $\vec{d} \cdot \vec{x}$ is the dot product between the direction vector $\vec{d}$ and point $\vec{x}$.

<p></p>

The dot product (in 3D) calculated for vectors $\vec{a}$ and $\vec{b}$ is:

$$ \vec{a} \cdot \vec{b} = \sum_{i = 0}^{2} \vec{a}_i \vec{b}_i = \vec{a}_0 \vec{b}_0 + \vec{a}_1 \vec{b}_1 + \vec{a}_2 \vec{b}_2 $$

## 3. Direction updates

<p>The GJK algorithm uses the direction vector which is updated at each iteration. If at any point the algorithm reaches a simplex, the direction is updated towards the closest point/vertex on the convex shape.</p>

## 4. Simplex and the closest points

<p>At each iteration through the Minkowski difference, it derives for simplex formations (line, triangle, or tetrahedron). A simplex is formed by a group of points where the closest point from the origin is computed.</p>
<p></p>

Such as for a 2D triangle formed by points $\vec{v}_0$, $\vec{v}_1$, $\vec{v}_2$, the closest point is computed with dot products and cross products.

## 5. Degenerate Triangles

Given a triangle with vertices $\vec{v}_0$, $\vec{v}_1$, $\vec{v}_2$, it is important to determine whether the triangle is degenerate (meaning zero area). For the EPA algorithm, in order to calculate the normal vector, the triangle should have a defined surface.

<p></p>

The area can be computed by constructing 2 adjacent boundaries of the triangle as such:

$$ \vec{b}_1 = \vec{v}_0 - \vec{v}_1 $$
$$ \vec{b}_2 = \vec{v}_0 - \vec{v}_2 $$

The area, denoted as $A$, of the triangle is calculated using the cross product:

$$ A = |\frac{1}{2}(\lVert \vec{b}_1 \times \vec{b}_2 \lVert )| $$

Where if $\vec{b}_1 \times \vec{b}_2$ is the zero vector $\vec{0}$, then the area is zero. Additionally the magnitude of the cross product can be used too:

$$ A = \frac{1}{2} b_1 b_2 \sin(\theta)$$

If the two adjacent edges are parallel, then $\theta = 0\degree$ or $\theta = 180\degree$ ($\pi$ rad) depending if they are pointing in the same direction or opposing directions. This makes $\sin(0) = \sin(\pi) = 0$, meaning the triangle has zero area and we can disgard it for EPA when calculating the collision surface normal. 

## Demo via Youtube from 2025
[![GJK Algorithm Test](https://img.youtube.com/vi/et7qVVCosnA/0.jpg)](https://www.youtube.com/watch?v=et7qVVCosnA)

# Expanding Polytope Algorithm (EPA)

(To be elaborated on)

# Workaround for Concave Objects using V-HACD