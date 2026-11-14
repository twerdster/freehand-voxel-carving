# Freehand Voxel Carving maintenance context

Read this file before SKILL.md. Updated 2026-11-14.

## Updated-work navigation (2026-11-14)

Aaron authorized a top-of-page `New!` badge and “Updated and ported for modern hardware” link in the root README. Both lead to the corresponding heading in `updated/README.md`, which now opens with a brief description of the actual port, repairs or reimplementation and its limits. An italic line marks the following content as the historical reference. The historical root reference body remains unchanged; current implementation details stay under `updated/`.

This documentation change does not alter source, submodule pins, media or recorded validation. Earlier root-README byte-identity statements and hashes apply to their recorded baseline; current preservation checks remove only the authorized navigation block before comparing the historical body.

The archive pins ice00cold/iPhone-3D-scanner at
881f5a8f02d9282d502d4547da0ecb34d5c0f00c. Original source bytes, notices and the
main README remain unchanged. The original iOS 6/armv7 project fails before source
compilation on Xcode 16.1. SDK camera capture/tracking remain unvalidated.

`updated/` now builds the recovered C++ carving, marching cubes and color
accumulation offline on macOS/Linux. Python generates build-tree source copies with
limited compatibility edits and fixes: deterministic occupancy borders and correct
scalar Zbuffer dimensions/access. Original-defect controls reproduce assertion
failure and an ASan heap-buffer overflow. The recovered C++ operators perform reconstruction.

Four component checks pass on Mac/OpenCV 4.12 and Linux/OpenCV 4.5.4. Linux ASan/UBSan
checks and the 36-view dinosaur replay pass. Mac sanitizer shutdown fails in
the installed Homebrew TBB dependency; do not call that run sanitizer-clean.

Historical demo matrices: 22,377 occupied voxels, 25,200 triangles, 10,654 distinct
RGBs; separate 128³ two-vote reference: 0 mismatches, finite bounded mesh/no degenerate
triangles; mean/min input-view silhouette IoU 0.9520/0.9380. Input-view overlap is not
independent 3D accuracy. The Mac-only text-file matrix variant produces 22,750 voxels/25,500 triangles;
packing changes after camera 9 and is checked against the unchanged demoP table.

New usage and results are linked from updated/README.md, with details in
updated/BUILD.md. Preserve main README as the archival reference.
The dinosaur images/calibration are the VGG/Wolfgang Niem sequence bundled through
Ben Tordoff's example. Preserve data attribution and existing source/SDK terms.

Native builds and complete failed-attempt logs are retained in the private task
handoff. Bulk outputs use a separate output volume; repositories and
builds stay on native filesystems. Linux dependencies were extracted into the
task directory without sudo or global changes. Public failure summaries and measured records are in updated/VALIDATION.json
and validation/; full private logs remain in the task handoff.

## New native replay frontends (2026-11-14)

`updated/native/` exposes the recovered C++ through a C ABI; `updated/ios/` generates
an external UIKit/SceneKit Xcode project, `updated/android/` provides Java/JNI/GLES20,
and `updated/desktop/` starts an actual native replay from a loopback browser UI.
All reconstruct from 36 bundled real inputs and supplied rounded demoP cameras;
no saved geometry substitutes for computation. These are new interfaces, not a
restoration of the historical mobile app or live tracking/capture.

See `updated/PLATFORMS.md` for build/run commands and coordinate formats, and
`updated/validation/native-platforms/` for separate build/source, native result,
repeated-run, cross-platform and capture records. Final host builds and all 36-view
C-ABI runs passed on Mac/Linux; signed iphoneos builds passed. The new apps also completed all 36 views on a physical iPhone 12/iOS 18.0.1 and
Pixel 2/Android 10, with matching recorded fixture counts/FNV. The iOS marker
self-checks its baseline; Android baseline comparison was performed externally.
Physical results/artifact hashes/captures are retained separately in
`ios-result.json`/`android-result.json` and corresponding provenance. Android
ABI compilation and minimum OS settings are not extra runtime coverage. Do not
overwrite the device runner's mobile evidence.

The desktop renderer uses actual computed arrays, supports drag/orbit, rejects
stale results across reruns and retains all per-view native counts. Mac capture is
a headed Chrome viewport; Linux capture is X11 :98 with software WebGL. Display
axis transformations leave saved arrays unchanged. Android stores raw native
coordinates; host/UIKit outputs are normalized as documented. New C ABI/frontend
sanitizer coverage is not inferred from the earlier offline-core sanitizer runs.

Native replay final result: 22,377 occupied voxels, 25,200 triangles, 75,600
vertices; occupancy FNV 803e245304c9b374. Mac/Linux normalized geometry bytes match;
10 RGBA channel bytes differ by one out of 255, so do not claim exact color parity.
The C ABI copies input buffers and preserves adapted recovered operators. Only
the fixed camera fixture is validated, not arbitrary finite projection matrices.

Actual tools-disabled Claude code reviews and a focused 59-file follow-up completed
for both native frontend implementations. Useful fixes were verified against code;
the follow-up found no High issues or material regressions. Android completion
baseline semantics are explicitly qualified. An actual 32-record documentation
review also completed and its factual qualifications were applied. Review adds
no runtime coverage; see updated/validation/native-platforms/claude-review.json.


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
