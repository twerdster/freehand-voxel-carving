# Maintaining Freehand Voxel Carving

Read SUMMARY.md first, then this file, updated/README.md, updated/BUILD.md, validation records and
the relevant original source. Preserve the source submodule pin and all original
bytes. Keep the root README as the historical reference; link new usage/results
only from updated/README.md. Preserve existing terms; do not assign a blanket license.

## Updated-work navigation

Aaron explicitly authorized the root README’s local `New!` badge and exact “Updated and ported for modern hardware” link on 2026-11-14. Keep both linked to `updated/README.md#updated-and-ported-for-modern-hardware`; retain the italic historical-reference qualifier and preserve the rest of the historical root reference. The matching updated README heading must introduce a short, factual account of the actual changes and limitations before the existing visuals, setup and evidence. Keep implementation descriptions and detailed results under `updated/`.

This navigation addition is the authorized exception to older root-README preservation instructions below. Verify the historical body after removing only that block, preserve source hashes/pins and leave historical validation hashes attached to their original freezes. Documentation-only navigation edits do not add runtime coverage or require a package rebuild.

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


## Native fixture applications

Use updated/PLATFORMS.md for the new C ABI, UIKit/SceneKit, Android/JNI/GLES20 and
localhost desktop interfaces. Preserve original source/root README and build
outside the repository. Android build metadata is in updated/android/VALIDATION.json;
host/iOS source/build records and distinct device runtime/capture evidence are in
updated/validation/native-platforms/. Confirm completion JSON and per-view counts,
not merely a screen image. Only the 36-view supplied-camera fixture is validated;
no live capture/tracking or restored historical app is implied.

For C ABI changes rerun the four CTest component checks and the 36-view host replay.
Freehand has no separate C-ABI prism regression comparable to Volume's test.
For frontend changes check artifact writes/counts/finiteness, stale completion
handling, repeated runs and actual geometry interaction. Serialize Android work
across Activity recreation. Keep completion markers last and bind saved artifacts
by hashes. Android completion does not gate the fixed baseline: compare its
recorded counts/FNV externally rather than calling it baseline-verified. Normalize
the documented saved coordinate formats before comparing
platforms. Distinguish native CPU reconstruction from GPU display; optional orbit
is display-only. Do not infer sanitizer coverage for new wrappers/frontends from
prior offline core tests. Tie Claude claims to actual reviewed source snapshots;
check its factual suggestions rather than applying them automatically.


The UIKit display adapter converts a copy of byte RGBA to float4 in [0,1] for
SceneKit; saved byte arrays/native operators are unchanged. The earlier physical
white-color display failure is retained privately. A focused host test checks all
256 byte values in each channel, finite range/error, source preservation and a
known teal/alpha case; see updated/validation/native-platforms/ios-display-colors-test.json. Rebuild and recapture the
iOS app for display changes; desktop/Android builds are unaffected.

The final normalized-color iOS builds completed the same 36-view fixture on the
physical iPhone with unchanged saved positions/colors and occupancy hashes. Actual
recordings visibly show colored Freehand geometry/teal Volume points. The bounded
8-file color-delta Claude review completed without material findings; it is a
separate actual static invocation after the earlier 59-file review. See
updated/validation/native-platforms/platform-status.json for current artifact,
runtime and capture links. Preserve the existing SDK/live-capture limitations.
