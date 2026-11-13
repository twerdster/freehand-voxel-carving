#!/usr/bin/env python3
"""Make an offline WebGL turntable of the actual exported colored triangle mesh."""
import argparse
import base64
import json
import math
import struct
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("reconstruction", type=Path)
parser.add_argument("output", type=Path)
args = parser.parse_args()
lines=(args.reconstruction/"dinosaur.ply").read_text().splitlines()
end=lines.index("end_header")
count=int(next(line.split()[-1] for line in lines[:end] if line.startswith("element vertex ")))
points=[]
for line in lines[end+1:end+1+count]:
    values=line.split();points.append(([float(values[0]),float(values[1]),-float(values[2])],[int(v)/255 for v in values[3:6]]))
low=[min(p[0][i] for p in points) for i in range(3)]
high=[max(p[0][i] for p in points) for i in range(3)]
center=[(a+b)/2 for a,b in zip(low,high)];scale=1.7/max(b-a for a,b in zip(low,high))
packed=[]
for i in range(0,len(points),3):
    a,b,c=[points[i+j][0] for j in range(3)]
    u=[b[k]-a[k] for k in range(3)];v=[c[k]-a[k] for k in range(3)]
    normal=[u[1]*v[2]-u[2]*v[1],u[2]*v[0]-u[0]*v[2],u[0]*v[1]-u[1]*v[0]]
    length=math.sqrt(sum(t*t for t in normal));normal=[t/(length or 1) for t in normal]
    for j in range(3):packed.extend([(points[i+j][0][k]-center[k])*scale for k in range(3)]+normal+points[i+j][1])
payload=base64.b64encode(struct.pack("<"+"f"*len(packed),*packed)).decode()
metrics=json.loads((args.reconstruction/"metrics.json").read_text())
photo=base64.b64encode((args.reconstruction/"input.png").read_bytes()).decode()
html=r'''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Freehand carving: reconstructed dinosaur</title>
<style>*{box-sizing:border-box}body{margin:0;background:#101a27;color:#e9eff7;font:15px system-ui}main{max-width:1000px;margin:auto;padding:24px}h1{font-size:25px;font-weight:600;margin:0 0 7px}p{color:#bac7d9;margin:0 0 18px}.stage{position:relative;border:1px solid #324359;border-radius:15px;overflow:hidden;background:#14243a}canvas{display:block;width:100%;height:auto}.inset{position:absolute;left:18px;top:18px;width:150px;padding:7px;border:1px solid #627086;background:#182536e8;border-radius:8px}.inset img{display:block;width:100%;border-radius:3px}.label{font-size:11px;color:#d8e1ef;padding-top:5px}.controls{display:flex;align-items:center;gap:15px;margin:16px 0}input{flex:1}button{color:white;background:#294661;border:1px solid #5c7896;border-radius:7px;padding:8px 18px}footer{font-size:12px;color:#a8bad0;line-height:1.5}.metric{position:absolute;bottom:15px;left:19px;color:#c7d6e8;font-size:13px}</style>
<main><h1>Freehand voxel carving</h1><p>Actual reconstruction from the bundled dinosaur photographs, masks and camera matrices.</p><div class="stage" id="stage"><canvas id="view" width="900" height="540"></canvas><div class="inset"><img src="data:image/png;base64,PHOTO"><div class="label">One of the 36 source photographs</div></div><div class="metric">36 views · 128³ voxel grid · FACES triangles · original C++ carving and marching cubes</div></div><div class="controls"><button id="play">Pause</button><input id="angle" aria-label="Turntable angle" type="range" min="0" max="360" value="0"></div><footer>Updated offline replay, 2026. Original project: Alex Fish, supervised by Aaron Wetzler, Yaron Honen and Ron Kimmel.<br>Dinosaur sequence: VGG / Wolfgang Niem (Hannover), bundled through Ben Tordoff's space-carving example. This is a visual hull; input-view overlap is not independent 3D ground truth.</footer></main>
<script>
const canvas=document.getElementById('view'),gl=canvas.getContext('webgl',{antialias:true,preserveDrawingBuffer:true});if(!gl)throw Error('WebGL unavailable');
const vertex=`attribute vec3 position;attribute vec3 normal;attribute vec3 rgb;uniform float angle;varying vec3 color;void main(){float c=cos(angle),s=sin(angle);vec3 p=vec3(c*position.x-s*position.y,s*position.x+c*position.y,position.z);vec3 n=vec3(c*normal.x-s*normal.y,s*normal.x+c*normal.y,normal.z);float elevation=.23;vec3 camera=vec3(p.x,p.z*cos(elevation)-p.y*sin(elevation),p.y*cos(elevation)+p.z*sin(elevation));gl_Position=vec4(camera.x*.61,camera.y*.94,camera.z*.3,1.);float light=.72+.28*abs(dot(normalize(n),normalize(vec3(.3,-.7,.8))));color=rgb*light;}`;
const fragment=`precision mediump float;varying vec3 color;void main(){gl_FragColor=vec4(color,1.);}`;
function shader(type,source){let s=gl.createShader(type);gl.shaderSource(s,source);gl.compileShader(s);if(!gl.getShaderParameter(s,gl.COMPILE_STATUS))throw Error(gl.getShaderInfoLog(s));return s}
const program=gl.createProgram();gl.attachShader(program,shader(gl.VERTEX_SHADER,vertex));gl.attachShader(program,shader(gl.FRAGMENT_SHADER,fragment));gl.linkProgram(program);if(!gl.getProgramParameter(program,gl.LINK_STATUS))throw Error(gl.getProgramInfoLog(program));gl.useProgram(program);
const raw=atob('PAYLOAD'),bytes=new Uint8Array(raw.length);for(let i=0;i<raw.length;i++)bytes[i]=raw.charCodeAt(i);const data=new Float32Array(bytes.buffer),buffer=gl.createBuffer();gl.bindBuffer(gl.ARRAY_BUFFER,buffer);gl.bufferData(gl.ARRAY_BUFFER,data,gl.STATIC_DRAW);['position','normal','rgb'].forEach((name,index)=>{let location=gl.getAttribLocation(program,name);gl.enableVertexAttribArray(location);gl.vertexAttribPointer(location,3,gl.FLOAT,false,36,index*12)});const uniform=gl.getUniformLocation(program,'angle');gl.enable(gl.DEPTH_TEST);gl.clearColor(.075,.12,.19,1);let playing=true,rotation=0,last=0;
window.renderFrame=function(degrees){rotation=degrees;document.getElementById('angle').value=degrees;gl.viewport(0,0,canvas.width,canvas.height);gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);gl.uniform1f(uniform,degrees*Math.PI/180);gl.drawArrays(gl.TRIANGLES,0,data.length/9);gl.finish();return true};
document.getElementById('play').onclick=()=>{playing=!playing;document.getElementById('play').textContent=playing?'Pause':'Play'};document.getElementById('angle').oninput=e=>{playing=false;document.getElementById('play').textContent='Play';renderFrame(+e.target.value)};window.pause=()=>{playing=false;document.getElementById('play').textContent='Play'};
function tick(time){if(playing)renderFrame((rotation+(time-last)*.012)%360);last=time;requestAnimationFrame(tick)}renderFrame(270);window.ready=true;requestAnimationFrame(tick);
</script></html>'''
args.output.parent.mkdir(parents=True,exist_ok=True)
args.output.write_text(html.replace('PHOTO',photo).replace('FACES',str(metrics['triangles'])).replace('PAYLOAD',payload))
print(args.output)
