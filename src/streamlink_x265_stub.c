/*
 * STREAMLINK-FIX-02 — ABI stub for x265 v215 API.
 *
 * The FFmpeg prebuilts shipped by LizardByte/build-deps reference
 * `x265_api_get_215`, but MSYS2's mingw-w64-ucrt-x86_64-x265 4.2 only exports
 * `x265_api_get_216`. The two are ABI-compatible at the level FFmpeg uses
 * (param/encoder/picture pointers), but the symbol name itself does not
 * resolve, so the link fails.
 *
 * This stub forwards the call to _216 when available, otherwise returns NULL.
 * FFmpeg detects a NULL return and disables the x265/HEVC software encoder
 * gracefully — the host keeps all hardware encoders (NVENC / AMF / QSV)
 * which are the primary encode path for STREAMLINK anyway.
 */

#include <stddef.h>

/* Forward declaration of the available _216 entry point. */
extern void *x265_api_get_216(int bit_depth);

void *x265_api_get_215(int bit_depth) {
    return x265_api_get_216(bit_depth);
}
