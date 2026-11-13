#!/usr/bin/env python3
"""Prepare a private build copy of the pinned carving core; never edit source/."""
import argparse
import hashlib
import json
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("output", type=Path)
parser.add_argument("--keep-boundary-bug", action="store_true")
parser.add_argument("--keep-zbuffer-bug", action="store_true")
args = parser.parse_args()
original = ROOT / "source/PointCloudSample/PointCloudSample"
args.output.mkdir(parents=True, exist_ok=True)
records = []
files = [original/name for name in ("Cube.cpp", "Cube.h", "Mesh.cpp", "Mesh.h", "ImageData.cpp", "ImageData.h", "Zbuffer.cpp", "Zbuffer.h")]
files += sorted((original/"Sculptor").glob("*.cpp")) + sorted((original/"Sculptor").glob("*.h"))
for path in files:
    content = path.read_bytes()
    text = content.decode("utf-8")
    changes = []
    if "#include <opencv2/opencv.hpp>" in text:
        text = text.replace("#include <opencv2/opencv.hpp>", "#include <opencv2/core.hpp>\n#include <opencv2/imgproc.hpp>")
        changes.append("include only the OpenCV core/imgproc modules actually used")
    if path.name == "Cube.h":
        text = text.replace("#include <OpenGLES/ES1/gl.h>", "// Offline Cube geometry does not use OpenGL types.")
        text = text.replace("#include <OpenGLES/ES1/glext.h>", "")
        changes.append("remove unused Apple OpenGLES includes")
    if path.name == "Sculpture.h":
        assert "Vector<ImageData> cameras;" in text
        text = text.replace("Vector<ImageData> cameras;", "std::vector<ImageData> cameras;")
        text = text.replace('"../Cube.h"', '"Cube.h"')
        changes.append("replace removed cv::Vector with std::vector; flatten build-copy include")
    if path.name == "Sculpture.cpp" and not args.keep_boundary_bug:
        assert "Sculpture::Sculpture() {" in text
        text = text.replace("Sculpture::Sculpture() {", "Sculpture::Sculpture() {\n    std::memset(sculpture, 0, sizeof(sculpture));\n    std::memset(colors, 0, sizeof(colors));")
        changes.append("initialize all occupancy and color bytes before setting the interior")
    if path.name == "Zbuffer.cpp" and not args.keep_zbuffer_bug:
        start = text.index("Zbuffer::Zbuffer(")
        end = text.index("Zbuffer::~Zbuffer", start)
        text = text[:start] + """Zbuffer::Zbuffer(int width, int height)
    : buffer(height, width, CV_32FC1, cv::Scalar(-std::numeric_limits<float>::max())) {}

""" + text[end:]
        start = text.index("    Vec4f currentBuff")
        end = text.index("\n}", start)
        text = text[:start] + """    float& currentBuff = buffer.at<float>(y,x);
    if (currentBuff < currentZ) currentBuff = currentZ;""" + text[end:]
        changes.append("correct height/width and store one scalar depth per single-channel pixel")
    destination = args.output/path.name
    destination.write_text(text)
    records.append({"source": str(path.relative_to(ROOT)), "build_copy": path.name,
                    "source_sha256": hashlib.sha256(content).hexdigest(),
                    "build_copy_sha256": hashlib.sha256(text.encode()).hexdigest(), "changes": changes})
(args.output/"source-manifest.json").write_text(json.dumps({"pin": "881f5a8f02d9282d502d4547da0ecb34d5c0f00c", "files": records}, indent=2)+"\n")
shutil.copyfile(ROOT/"source/PointCloudSample/LICENSE", args.output/"LICENSE.original-sample")
