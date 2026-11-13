# Freehand Voxel Carving maintenance context

Read this file before SKILL.md. Updated 2026-11-14.

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
updated/BUILD.md, as requested. Preserve main README as the archival reference.
The dinosaur images/calibration are the VGG/Wolfgang Niem sequence bundled through
Ben Tordoff's example. Preserve data attribution and existing source/SDK terms.

Native builds and complete failed-attempt logs are retained in the private task
handoff. Bulk outputs use a separate output volume; repositories and
builds stay on native filesystems. Linux dependencies were extracted into the
task directory without sudo or global changes. Public failure summaries and measured records are in updated/VALIDATION.json
and validation/; full private logs remain in the task handoff.
