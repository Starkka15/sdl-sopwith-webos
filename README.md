# sdl-sopwith-webos

⚠️ **Note: This repo name is misleading — it actually contains Bos Wars 2.8, not SDL Sopwith.**

## Bos Wars for webOS TouchPad

Bos Wars 2.8 — a futuristic real-time strategy game — ported to HP webOS TouchPad with OpenGL ES rendering.

### Port changes

- Converted OpenGL immediate mode to GLES 1.x vertex arrays
- Added PDL initialization for webOS
- Modified video, SDL, GL graphics, and minimap rendering
- Wrapped texture compression code (unavailable in GLES)
- Includes cross-compile build script (build-webos.sh) and packaging script (package-webos.sh)

### Building

Requires webOS PDK and ARM cross-compiler toolchain.

\`\`\`bash
./build-webos.sh
\`\`\`

### License

GPL-2.0
