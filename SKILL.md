# Maintaining Freehand Voxel Carving

Read SUMMARY.md first, then this file, updated/README.md, updated/BUILD.md, validation records and
the relevant original source. Preserve the source submodule pin and all original
bytes. Keep the root README as the historical reference; link new usage/results
only from updated/README.md. Preserve existing terms; do not assign a blanket license.

Build with CMake from updated/; the preparation script creates build-tree copies
and a SHA-256 manifest. Actual carving/meshing/texturing remain recovered C++.
Use the four CTest checks for affected component changes. Run the 36-view dinosaur
replay for reconstruction changes, and verify its separately implemented full-volume vote
reference, mesh validity and all input-view silhouette metrics. Keep historical
demoP and text-file-calibration results separate. Never turn component counts or
input-view consistency into general 3D accuracy, SDK/iOS compatibility or benchmark claims.

For memory/index changes, test the original-defect ablations and a Linux ASan/UBSan
build. Mac Homebrew TBB sanitizer shutdown currently fails externally; preserve
that evidence and do not suppress it to claim a clean run. Private extracted Linux
OpenCV core/imgproc/TBB dependencies are supported by FREEHAND_OPENCV_PREFIX and a
task-scoped loader path. Do not repair/install global dependencies without coordination.

Generate visuals only from actual exported mesh data using tools/render_demo.py.
Retain dataset/student attribution and label updated replay images. Keep GIFs below
2,000,000 bytes and MP4s below 10,000,000 bytes. Inspect representative frames, browser
errors and file sizes. Record tests, commands, dependencies, original/build hashes,
failed attempts and exact numerical scope in updated/VALIDATION.json and validation/.

Before every commit, reread and synchronize SUMMARY.md and SKILL.md, verify root
README/source unchanged, check doc links, image/video sizes and diff scope. Obtain
a real Claude editorial review when requested; save its actual response and verify
factual suggestions against code/results. Never invent review or runtime coverage.
