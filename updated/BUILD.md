# Build, calibration and validation details

## Build and run

Initialize the repository's pinned `source/` submodule first. Requirements are a
C++14 compiler, CMake 3.20+, Python 3, pkg-config, OpenCV core/imgproc development
files and libpng development files. Python is used to prepare build copies; the
reconstruction executes in the recovered C++ implementation.

From the repository root, using an existing compatible dependency installation:

```sh
cmake -S updated -B build/freehand -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/freehand -j4
ctest --test-dir build/freehand --output-on-failure
mkdir -p output/freehand
./build/freehand/freehand_offline --demo source/PointCloudSample/dinosaur output/freehand --historical-matrices
python3 updated/tools/render_demo.py output/freehand output/freehand/viewer.html
```

Open `output/freehand/viewer.html` in a WebGL-capable browser to rotate the
colored mesh. The viewer is generated with embedded mesh data.
`dinosaur.ply` contains colored triangles, and `dinosaur.obj` contains geometry.
`metrics.json` records initial occupancy and occupancy after each view, mesh
counts and silhouette IoU. The first view leaves occupancy unchanged because
removal requires two background votes. Output directories and builds are separate from `source/`.

On the tested Ubuntu 22.04 machine, dependencies were downloaded and extracted
into a task directory without a system installation:

```sh
bash updated/tools/linux_private_deps.sh /absolute/path/freehand-deps
export LD_LIBRARY_PATH=/absolute/path/freehand-deps/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
cmake -S updated -B build/freehand -DFREEHAND_OPENCV_PREFIX=/absolute/path/freehand-deps/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

That helper uses Ubuntu 22.04 package names and needs apt metadata/network access;
it is not a general installer for other distributions. System libpng development
files, compiler, CMake and pkg-config were already available. See
[validation](VALIDATION.json) for the actual toolchains and dependency versions.

## Original operations and changes

`prepare_sources.py` reads the unchanged pinned source and writes build-tree
copies with per-file source/build hashes. It retains `Sculpture::sculpt`,
`Polygonise`, mesh construction and the recovered color accumulation/fallback
logic. No external reconstruction or meshing algorithm replaces those operations.

The build copies make these changes:

- Replace removed `cv::Vector` with `std::vector`, include the needed standard
  headers, and remove unused Apple OpenGLES includes from `Cube` geometry.
- Initialize occupancy/color arrays before setting the interior. The original
  constructor left boundary occupancy dependent on previous heap contents.
- Correct `Zbuffer` dimensions to height-by-width and store scalar depth in its
  single-channel matrix. The original mixed scalar and four-float accesses and
  wrote outside the allocation on a portrait-image regression fixture.

The driver supplies a float32 camera transform to the original color routines,
which access that matrix as float32. PNG decoding/encoding and offline export
orchestration are new. Existing C++ source bytes and the submodule pin are
unchanged; original sample notices are copied beside generated source.

The pictured run uses `--historical-matrices`, selecting the exact rounded
`demoP` table used by the original application. Without that option, the driver
reads the bundled matrix text files at their supplied precision. Those files use
row-major packing for the first nine cameras and column-major packing afterward;
the importer checks both layouts against the original table before selecting one.
It does not optimize camera poses against the reconstruction. The two calibration
variants have separate recorded results.

The original grid is 128³. A voxel needs two background votes before removal
(`STEP=2`), including projections outside a silhouette image. The cube corner is
`(-0.1307, -0.1307, -0.7483)` with side `0.2463`, in the supplied projection
coordinate system. `P` maps world homogeneous coordinates to pixel coordinates
by division by the third component; no image flip is applied. Mesh export centers
grid coordinates and flips z by calling the recovered `objData`/`plyData` routines.

## Checks and measured result

Four registered component checks cover deterministic boundary initialization,
portrait-image depth-buffer insertion/occlusion, a two-vote analytic prism, and
empty/single-corner marching-cubes cells. They passed on macOS arm64 and Linux
x86-64. The Linux tests and full 36-view replay also passed AddressSanitizer and
UndefinedBehaviorSanitizer. OpenCV itself is a prebuilt dependency, not a library
rebuilt with sanitizers.

The regression controls retain the original defects in separate build copies:
the boundary check fails, and AddressSanitizer reports a heap-buffer overflow in
the original depth-buffer constructor. These failures and the passing fixed
checks are retained in [validation records](validation/).

For the Linux sanitizer build, retain the task-scoped dependency loader path
from the setup section and use a separate build directory:

```sh
cmake -S updated -B build/freehand-asan -DFREEHAND_SANITIZE=ON -DCMAKE_BUILD_TYPE=Debug -DFREEHAND_OPENCV_PREFIX=/absolute/path/freehand-deps/usr
cmake --build build/freehand-asan -j4
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir build/freehand-asan --output-on-failure
mkdir -p output/freehand-asan
./build/freehand-asan/freehand_offline --demo source/PointCloudSample/dinosaur output/freehand-asan --historical-matrices
```

The ablation options `FREEHAND_KEEP_BOUNDARY_BUG=ON` and
`FREEHAND_KEEP_ZBUFFER_BUG=ON` prepare separate build copies retaining those
defects. In that build, `--test boundary` must fail its occupancy assertion and
`--test zbuffer` must fail under AddressSanitizer. These are regression controls,
not recommended settings for a reconstruction run.

The historical-matrix replay produced **22,377 occupied voxels and 25,200
triangles**, with 10,654 distinct output RGB colors. A separately written reference applying the same two-vote rule matched occupancy at every voxel of the 128³ grid. All triangle vertices
were finite and within the grid bounds, array lengths matched the triangle count,
and no zero-area triangles were found.

Mac and Linux exports have identical vertex coordinates and face records. Five
of the 75,600 exported vertex colors differ by one intensity level out of 255;
the files are not byte-identical. The comparison is recorded in
[cross-platform results](validation/cross-platform-mesh.json).

Rasterizing the reconstructed triangles into the **same 36 input views** gave
mean silhouette intersection-over-union of **0.9520**, with minimum **0.9380**.
These are input-view consistency measurements, not a held-out reconstruction
benchmark or independent 3D accuracy measurement. The Mac-only text-file calibration variant
produced 22,750 voxels and 25,500 triangles, with mean/minimum overlap 0.9517/0.9393.

## Limitations and attribution

The unchanged iOS project fails on Xcode 16.1 before source compilation because
it targets unsupported armv7/iOS 6. The original live capture, SDK tracking,
segmentation interface and iOS display were not restored. The actual dataset replay
uses supplied masks and camera matrices rather than estimating them.

A visual hull cannot recover concavities that silhouettes do not constrain.
Color visibility and fallback behavior remain the recovered approximation and
can produce patchy coloring. Input-view silhouette overlap does not establish color
accuracy or physical scale. There is no new speed benchmark.

The Mac sanitizer attempt reached passing assertions but failed at shutdown in
the installed Homebrew TBB dependency. It is recorded as failed; sanitizer-clean
claims refer to Linux. The tested Mac OpenCV installation also had broken optional
module dependencies; this build links only core/imgproc and uses libpng for IO.

Original project: Alex Fish, supervised by Aaron Wetzler, Yaron Honen and Ron
Kimmel. The source sample's original license and all student/third-party notices
remain in place; this update assigns no new blanket license. The dinosaur sequence
is attributed by the [VGG multiview dataset page](https://www.robots.ox.ac.uk/~vgg/data/mview/)
to Wolfgang Niem, University of Hannover, and is bundled here through Ben Tordoff's
space-carving example. No new dataset license or permission is implied.
