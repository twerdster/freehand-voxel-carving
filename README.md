# Freehand Voxel Carving Scanning on a Mobile Device

Freehand 3D scanning on a mobile device using voxel carving.

Original project credits: Alex Fish, Aaron Wetzler, Y. Honen, Ron Kimmel.

## Status

Alex Fish's original [iPhone-3D-scanner source](https://github.com/ice00cold/iPhone-3D-scanner) is included as the `source/` Git submodule, pinned to `881f5a8f02d9282d502d4547da0ecb34d5c0f00c`. This is an archival snapshot, not a verified current iOS release. It is distinct from the earlier Kucher/Povichuk volume-carving project.

## Running

```sh
git clone --recurse-submodules https://github.com/twerdster/freehand-voxel-carving.git
cd freehand-voxel-carving
open source/PointCloudSample/PointCloudSample.xcodeproj
```

The original author's README explicitly requires a physical iOS device, not the simulator. The Xcode project targets iOS 6.0 and links `libpointcloud.a`. Open the historical Xcode project, supply your own valid signing and compatible dependencies, and inspect the app target before building. No modern Xcode/device compatibility has been tested.

The original workflow initializes SLAM from the camera, positions a voxel cube, captures object views, then uses the Mesh screen to inspect segmentation and sculpt individual or all views. A demo mode uses predefined images/silhouettes; the upstream README credits those sample images and camera matrices to Ben Tordoff's space-carving example. Keep those third-party notices intact.

The upstream README documents `SCULPTURE_SIZE` in `PointCloudSample/Sculptor/Sculpture.h` as the historical voxel-resolution setting. See the original README and tutorial for the original controls. No source code or scientific results have been modified, compiled or run during recovery.

## Links

- [Project entry on Aaron Wetzler's website](https://twerdster.github.io/#freehand-voxel-carving-scanning-on-a-mobile-device)
- [Original lab project page](https://gip.cs.technion.ac.il/last-projects-2015-7.html)
- [Original student source](https://github.com/ice00cold/iPhone-3D-scanner)
- [Original tutorial](https://www.youtube.com/watch?v=zy40dpsmprg)

## Attribution and Updates

Retain the upstream PointCloud SDK license files, sample license, branding requirements and third-party attribution. This archive assigns no additional code or media license. Original authors can open an issue to contribute updates or request corrections or removal of their material.

