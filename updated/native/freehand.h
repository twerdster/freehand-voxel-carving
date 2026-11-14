/* New 2026 C ABI for the recovered Freehand carving/meshing/color code.
 * Source pin/notices remain in ../../source. No live capture or pose tracking.
 */
#ifndef WETZLER_FREEHAND_H
#define WETZLER_FREEHAND_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct WFCContext WFCContext;
typedef struct { uint32_t views, occupied_voxels, triangles, vertices; uint64_t occupancy_fnv1a64; } WFCStats;
WFCContext *wfc_create(float side,float corner_x,float corner_y,float corner_z);
void wfc_destroy(WFCContext *context);
/* Input P: row-major double3x4; rt: row-major float3x4 used by original colors.
 * RGBA has width*height*4 bytes; mask has width*height bytes (0=background).
 * Validation currently covers the bundled dinosaur fixture, not arbitrary cameras.
 * Host buffers are copied; originals' per-view images remain owned by core.
 */
int wfc_apply(WFCContext *context,const double *projection12,const float *rt12,
    const uint8_t *rgba,size_t rgba_bytes,const uint8_t *mask,size_t mask_bytes,
    uint32_t width,uint32_t height);
/* Returns the generated triangle count, or -1 on failure. */
int wfc_make_mesh(WFCContext *context);
int wfc_get_stats(const WFCContext *context,WFCStats *stats);
size_t wfc_copy_positions(const WFCContext *context,float *output,size_t float_capacity);
size_t wfc_copy_normals(const WFCContext *context,float *output,size_t float_capacity);
size_t wfc_copy_colors(const WFCContext *context,uint8_t *output,size_t byte_capacity);
const char *wfc_last_error(const WFCContext *context);
/* Fixture helper: same dinosaur intrinsic matrix/inverse as offline.cpp. */
int wfc_dinosaur_rt(const double *projection12,float *output12);
#ifdef __cplusplus
}
#endif
#endif
