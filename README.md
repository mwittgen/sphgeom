sphgeom: spherical geometry primitives {#mainpage}
======================================

Overview
--------

This low-level C++ library provides primitives for representing points and
regions on the unit sphere, as well as support for partitioning the sphere.
It can be used to answer the following sorts of questions:

  - *Is point X inside region Y?*
  - *Do two regions A and B intersect?*
  - *Which pieces of the sphere does region C overlap?*

Regions can be serialized to binary strings, so that they may be stored
efficiently in files or VARBINARY database columns. They can also be
approximated with simpler regions - for example, one can ask for the
bounding circle of a convex polygon.

Python bindings that expose most of the C++ API are also provided via SWIG.

Points
------

There are 3 different classes for points -
[LonLat](\ref lsst::sphgeom::LonLat) for spherical coordinates,
[Vector3d](\ref lsst::sphgeom::Vector3d) for Cartesian vectors in ℝ³
(not constrained to lie on the unit sphere), and
[UnitVector3d](\ref lsst::sphgeom::UnitVector3d) for vectors in ℝ³ with
unit ℓ² norm.

Regions
-------

Four basic spherical [Region](\ref lsst::sphgeom::Region) types are
provided:

  - [Box](\ref lsst::sphgeom::Box), a longitude/latitude angle box
  - [Circle](\ref lsst::sphgeom::Circle), a small circle defined
    by a center and opening angle/chord length
  - [Ellipse](\ref lsst::sphgeom::Ellipse), the intersection of an
    elliptical cone with the unit sphere
  - [ConvexPolygon](\ref lsst::sphgeom::ConvexPolygon), a convex
    spherical polygon with unit vector vertices and great circle edges

In addition to the spherical regions, there is a type for 3-D axis aligned
boxes, [Box3d](\ref lsst::sphgeom::Box3d). All spherical regions know how
to compute their 3-D bounding boxes, which makes it possible to insert them
into a 3-D [R-tree](https://en.wikipedia.org/wiki/R-tree). This is used by the
exposure indexing task in the [daf_ingest](https://github.com/lsst/daf_ingest)
package to spatially index exposure bounding polygons using the
[SQLite](https://sqlite.org) 3
[R*tree module](https://www.sqlite.org/rtree.html).

A region can also determine its spatial
[relationship](\ref lsst::sphgeom::Relationship) to another region, and
test whether or not it contains a given unit vector.

Pixelizations
-------------

This library also provides support for assigning points to pixels (a.k.a.
cells or partitions) in a [Pixelization](\ref lsst::sphgeom::Pixelization)
(a.k.a. partitioning) of the sphere, and for determining which pixels
intersect a region.

Currently, the [Chunker](\ref lsst::sphgeom::Chunker) class implements
the partitioning scheme employed by [Qserv](https://github.com/lsst/qserv).
The [HtmPixelization](\ref lsst::sphgeom::HtmPixelization) class implements
the HTM (Hierarchical Triangular Mesh) pixelization. The
[Q3cPixelization](\ref lsst::sphgeom::Q3cPixelization) and
[Mq3cPixelization](\ref lsst::sphgeom::Mq3cPixelization) classes implement
the original Quad Tree Cube indexing scheme and a modified version with
reduced pixel area variation.

See Also
--------

#### Contributing

For instructions on how to contribute, see http://dm.lsst.org/#contributing
(or just send us a pull request).

#### Support

For help, see http://dm.lsst.org/#support.
