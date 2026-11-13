#!/usr/bin/env python3
"""Export the unchanged upstream demoP table for independent replay tools."""
import argparse
import hashlib
import json
import re
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("output", type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = root/"source/PointCloudSample/dinosaur/matrices/matricesData.h"
text = re.sub(r"//[^\n]*", "", source.read_text())
table = text[text.index("static double demoP"):].split("=", 1)[1].split("};", 1)[0]
numbers = [float(v) for v in re.findall(r"[-+]?(?:\d*\.\d+|\d+)(?:[eE][-+]?\d+)?", table)]
assert len(numbers) == 36*12
payload = {"source": str(source.relative_to(root)), "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
           "source_pin": "881f5a8f02d9282d502d4547da0ecb34d5c0f00c", "matrix_source": "unchanged rounded upstream demoP table",
           "convention": "row-major 3x4 P, homogeneous world point to pixel; divide first two components by third; no image flip",
           "image_width": 720, "image_height": 576, "foreground": "nonzero green channel; white masks on black background",
           "cube_corner": [-0.1307, -0.1307, -0.7483], "cube_side": 0.2463,
           "projection_matrices": [numbers[i:i+12] for i in range(0,len(numbers),12)]}
args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_text(json.dumps(payload, indent=2)+"\n")
print(args.output)
