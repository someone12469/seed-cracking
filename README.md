Targets Minecraft 26.1. JAR available on [GitHub Releases](https://github.com/someone12469/seed-cracking/releases/latest) and temporarily on GitHub Actions.

Mod usage:
- Go to overworld location with untouched bedrock, deepslate-stone transition and ore veins.
- Execute `/bedrockcrack start` to start collecting data.
- You can execute `/bedrockcrack progress` to see how many more chunks you need to scan.
- You can stop collecting data preemptively by doing `/bedrockcrack stop`.

Algorithm for inverting `forkPositional()` (in `xsinv.cpp`) stolen from https://github.com/pseudogravity/xoroshiro/blob/main/from2nextlongs.cu (reimplemented for CPUs with bit packing and vectorization).
