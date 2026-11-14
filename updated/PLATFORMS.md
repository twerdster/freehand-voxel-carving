# Native replay interfaces

These are new interfaces around the recovered, adapted C++ carving, marching-cubes
and vertex-color operators. They reconstruct the bundled dinosaur sequence at run
time. They do not restore the original PointCloud SDK, live camera capture or pose
tracking. The original iOS 6/armv7 project and source submodule remain unchanged.

## Tested execution and recordings

The desktop Compute button launches a local C++ executable. The iOS and Android
buttons call the same C ABI in the app process. All receive 36 actual photographs,
masks and the original application's rounded `demoP` matrices; no saved mesh is
bundled as a substitute for computation. The result is 22,377 occupied voxels,
25,200 triangles and 75,600 triangle vertices. The FNV-1a64 of one 0/1 byte per voxel, traversed Z/Y/X, is
`803e245304c9b374` for this fixed fixture. This fingerprint binds one result; it is
not a collision-resistant proof or a measure of geometric accuracy.

[Runtime records and capture provenance](validation/native-platforms/) distinguish
build checks, actual executions and screen captures. Both new apps completed
all 36 views on physical iPhone 12/iOS 18.0.1 and Pixel 2/Android 10, with the counts
and FNV above. Artifact records bind the tested binaries. This does not validate
other devices, live capture or the historical app. Phone recordings:
[iOS GIF](assets/ios-native.gif) / [MP4](assets/ios-native.mp4),
[Android GIF](assets/android-native.gif) / [MP4](assets/android-native.mp4).
Desktop recordings:
[macOS GIF](assets/macos-native.gif) / [MP4](assets/macos-native.mp4),
[Linux GIF](assets/linux-native.gif) / [MP4](assets/linux-native.mp4).
The macOS recording samples the headed Chrome viewport; Linux captures the actual
isolated X11 desktop. Linux uses software WebGL for display, while reconstruction
runs in native C++. Sampling and encoding details are retained with each clip;
these recordings are not throughput benchmarks.

## macOS and Linux

Requirements: C++14, CMake 3.20+, Python 3, OpenCV core library plus core/imgproc headers, libpng development files,
and a WebGL browser. The separate [offline guide](BUILD.md) describes extracted
Ubuntu dependencies and the earlier full reference/sanitizer checks. Initialize
the source submodule first. From the repository root:

```sh
git submodule update --init source
cmake -S updated/native -B build/native -DWFC_BUILD_REPLAY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/native -j4
python3 updated/desktop/server.py --project freehand \
  --binary build/native/freehand_native_replay \
  --dataset source/PointCloudSample/dinosaur \
  --calibration /path/to/dinosaur-calibration.json \
  --work output/desktop --port 8784
```

Open `http://127.0.0.1:8784`, then choose **Compute 36 views**. Drag the computed
mesh or enable display rotation. Use [dinosaur-calibration.json from the Volume archive](https://github.com/twerdster/iphone-volume-carving/blob/main/updated/assets/dinosaur-calibration.json);
its `projection_matrices` array copies the original Freehand `demoP` table.
It is not the separately tested
text-file calibration variant. Set `OpenCV_DIR` if CMake cannot find your OpenCV
installation; `FREEHAND_OPENCV_PREFIX` supports the extracted Linux layout.

The loopback-only server starts one run at a time, clears prior results before a
new run and rejects foreign Host/Origin values. A repeated-run regression held a
previous geometry response across a new Compute request and verified that it could
not replace the new run's output. This interface is intended for a local workstation,
not deployment as an Internet service. The verified workflow uses one browser tab;
concurrent independent tabs are outside that test.

## iOS

Use Xcode 16.1 and the official OpenCV 4.12 iOS `opencv2.framework`. Generate the
project outside the repository:

```sh
python3 updated/ios/generate_project.py --output /native/path/freehand-ios \
  --dataset source/PointCloudSample/dinosaur \
  --calibration /path/to/dinosaur-calibration.json \
  --opencv-framework /path/to/opencv2.framework
```

Open the generated `FreehandUpdated.xcodeproj`, select your signing team and build
for your device. The tested target is Release/iphoneos, minimum declared iOS 16;
simulator and other devices require separate verification. The new UIKit interface
shows input/progress and a SceneKit rendering of the computed colored triangles.
Launch arguments `--autorun --autorun-delay 2 --orbit` start real computation after
two seconds, then rotate only the computed display. There is no artificial delay
between views. Documents contains `positions.f32`, `colors.rgba` and
`run-result.json`; the completion record is written last after checked writes,
finite/count/fixed-fixture occupancy checks and SHA-256 of saved artifacts and
calibration. A partial progress display is not a completion record.

SceneKit receives a separate float4 RGBA buffer normalized to [0,1]; this fixes
the earlier white-color display while preserving saved byte colors. The focused
[display conversion test](validation/native-platforms/ios-display-colors-test.json)
checks all 256 byte values per channel and source preservation. To rerun it on a host:

```sh
c++ -std=c++14 -I updated/ios/App updated/ios/tests/display_colors.cpp -o /native/path/display-colors-test
/native/path/display-colors-test
```

## Android and output formats

See [Android build/run instructions](android/README.md) and
[build/package checks](android/VALIDATION.json). The app uses Java/JNI and GLES20;
its APK contains source inputs and notices, not saved geometry. Native worker
ownership serializes computation across Activity recreation. Physical-device results and their separate capture provenance are in
`validation/native-platforms/`. Compile-only ABI/minimum-API support is not
runtime coverage.

The native C ABI is declared in [freehand.h](native/freehand.h). It copies input
pixels, preserves recovered matrices and returns actual triangle arrays.
`wfc_make_mesh` returns the triangle count, or -1 on failure. Only the bundled
camera fixture is validated; arbitrary finite camera coefficients are not a
supported safety/accuracy claim.

Android saves raw recovered positions (grid-centered, Z already negated,
not divided by 64) and recovered normals. iOS and the host replay save
positions divided by 64; both save per-vertex RGBA. A further bounding-box center,
scale and axis permutation makes the dinosaur upright in each viewer without
changing saved arrays. Android derives face normals for display lighting while
retaining recovered exported normals. Compare coordinate-normalized data rather
than assuming raw files match across frontends.

The visual-hull and color limitations in the [README](README.md) still apply.
Inputs are provided, not captured or calibrated by these apps. Preserve Alex Fish's
attribution, supervisor credits and the Wolfgang Niem / Hannover / Oxford VGG /
Ben Tordoff dataset provenance. Existing notices and terms remain in force.

Android's completion marker confirms native processing and saved arrays/counts/hash
records; its fixed-fixture baseline is checked externally by the device runner.
It does not self-gate that baseline as the UIKit and host replay targets do.
The [actual Claude code review record](validation/native-platforms/claude-review.json)
is a static review of the supplied source snapshots and adds no runtime coverage.

In the shared result schema, Freehand `observed_voxels` equals `raw_voxels`;
no Volume-style in-image output filter applies.
