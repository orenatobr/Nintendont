#!/usr/bin/env bash
# Builds loader/loader.dol from the current working tree with the toolchain the README pins:
# devkitARM r53-1, devkitPPC r35-1 and libOGC 1.8.23-1, inside a linux/amd64 Docker container.
#
# Usage: scripts/build_docker.sh [output.dol]
#
# Needs Docker, git, curl and shasum. On Apple silicon the container runs under emulation, so expect 15 to 40
# minutes. The old toolchain packages come from an unofficial mirror; every file is checked against the sha256
# below before it is used. The working tree is copied first, so the build never touches your files.
set -euo pipefail

HERE="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:-$HERE/loader/loader.dol}"
GENERAL_TOOLS_REF=46086605cdc63fb02ba0ed08cdc00801ba00c6f0
GAMECUBE_TOOLS_REF=3b7f914df771d4b779d88607ceed2ec9264ff93a
MIRROR=https://wii.leseratte10.de/devkitPro

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
mkdir -p "$WORK/tc" "$WORK/src" "$WORK/tools"

fetch() { # <url> <sha256>
  local f="$WORK/tc/$(basename "$1")"
  curl -fsSL --retry 3 -o "$f" "$1"
  echo "$2  $f" | shasum -a 256 -c - >/dev/null || { echo "sha256 mismatch for $1" >&2; exit 1; }
}
fetch "$MIRROR/devkitARM/r53%20%282019-06%29/devkitARM-r53-1-linux.pkg.tar.xz" 140d217a747c21a46236a6675b2592b54397e53c56dd8aa91aaf73bd19fa9599
fetch "$MIRROR/devkitPPC/r35/devkitPPC-r35-1-linux.pkg.tar.xz" 2916d431913b00d93dd0570d2d650cb1ec13e75422933e0f5726528d6a181658
fetch "$MIRROR/libogc/libogc_1.8.23%20%282019-10-02%29/libogc-1.8.23-1-any.pkg.tar.xz" e5b61a2e7589ab95e321cae30bbf1511fbe4729f3120e782201eebf8514c6707
fetch "$MIRROR/devkitPPC/devkitppc-rules/devkitppc-rules-1.1.1-1-any.pkg.tar.xz" 0118f06fff938c3d4913fdc004d5edd2f72e3a16c544fb5699b0e97552529d29

# tracked and untracked files, without .git and without old build output
(cd "$HERE" && git ls-files -z --cached --others --exclude-standard | tar -cf - --null -T -) | tar -xf - -C "$WORK/src"

git clone -q https://github.com/devkitPro/general-tools "$WORK/tools/general-tools"
git -C "$WORK/tools/general-tools" checkout -q "$GENERAL_TOOLS_REF"
git clone -q https://github.com/devkitPro/gamecube-tools "$WORK/tools/gamecube-tools"
git -C "$WORK/tools/gamecube-tools" checkout -q "$GAMECUBE_TOOLS_REF"

cat > "$WORK/in-container.sh" <<'EOF'
set -e
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq && apt-get install -y -qq build-essential python3 zip xz-utils libncurses6 >/dev/null 2>&1
for f in /tc/*.pkg.tar.xz; do tar -xJf "$f" -C / 2>/dev/null; done
# bin2s and elf2dol come from devkitPro's general-tools and gamecube-tools (plain C), compiled here.
mkdir -p /opt/devkitpro/tools/bin
for t in bin2s padbin raw2c; do
  gcc -O2 -DPACKAGE_NAME=\"general-tools\" -DPACKAGE_VERSION=\"1.4\" -DPACKAGE_STRING=\"general-tools\ 1.4\" \
      -DPACKAGE_BUGREPORT=\"\" -o /opt/devkitpro/tools/bin/$t /gt/general-tools/$t.c
done
gcc -O2 -DPACKAGE_NAME=\"gamecube-tools\" -DPACKAGE_VERSION=\"1.0\" -DPACKAGE_STRING=\"gamecube-tools\ 1.0\" \
    -DPACKAGE_BUGREPORT=\"\" -o /opt/devkitpro/tools/bin/elf2dol /gt/gamecube-tools/elftool/elf2dol.c
export DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM DEVKITPPC=/opt/devkitpro/devkitPPC
export PATH=$DEVKITPPC/bin:$DEVKITARM/bin:/opt/devkitpro/tools/bin:$PATH
cd /src
make forced > /src/build.log 2>&1 || { tail -30 /src/build.log; exit 1; }
sha256sum loader/loader.dol
EOF

docker run --rm --platform linux/amd64 \
  -v "$WORK/tc":/tc:ro -v "$WORK/tools":/gt:ro -v "$WORK/src":/src -v "$WORK/in-container.sh":/in-container.sh:ro \
  ubuntu:22.04 bash /in-container.sh
cp "$WORK/src/loader/loader.dol" "$OUT"
echo "wrote $OUT"
