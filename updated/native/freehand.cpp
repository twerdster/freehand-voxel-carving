// New platform ABI; all numerical reconstruction remains the recovered core.
#include "freehand.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "Sculpture.h"
struct WFCContext {
 std::unique_ptr<Sculpture> core;
 Cube box;uint32_t views=0;std::string error;
 WFCContext(float side,float x,float y,float z):core(new Sculpture()){box.setSize(side);box.setCorner(x,y,z);}
};
extern "C" {
WFCContext *wfc_create(float side,float x,float y,float z){
 if(!std::isfinite(side)||side<=0||!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z))return nullptr;
 try{return new WFCContext(side,x,y,z);}catch(...){return nullptr;}
}
void wfc_destroy(WFCContext *c){delete c;}
int wfc_apply(WFCContext *c,const double *p,const float *rt,const uint8_t *rgba,size_t rgbaBytes,const uint8_t *mask,size_t maskBytes,uint32_t width,uint32_t height){
 if(!c)return -1;c->error.clear();
 if(!p||!rt||!rgba||!mask||!width||!height||width>4096||height>4096||rgbaBytes!=size_t(width)*height*4||maskBytes!=size_t(width)*height){c->error="invalid camera, pixels or exact byte count";return -1;}
 for(unsigned i=0;i<12;++i)if(!std::isfinite(p[i])||!std::isfinite(rt[i])){c->error="nonfinite camera coefficient";return -1;}
 try{
  cv::Mat image(height,width,CV_8UC4),silhouette(height,width,CV_8UC4),projection(3,4,CV_64F),rotation(3,4,CV_32F);
  std::memcpy(image.data,rgba,rgbaBytes);std::memcpy(projection.data,p,sizeof(double)*12);std::memcpy(rotation.data,rt,sizeof(float)*12);
  for(size_t i=0;i<maskBytes;++i){auto &v=silhouette.at<cv::Vec4b>(int(i/width),int(i%width));v=cv::Vec4b(mask[i]?255:0,mask[i]?255:0,mask[i]?255:0,255);}
  c->core->sculpt(ImageData(image,silhouette,projection,rotation),c->box);++c->views;return 0;
 }catch(const std::exception &e){c->error=e.what();return -1;}catch(...){c->error="native carving failed";return -1;}
}
int wfc_make_mesh(WFCContext *c){
 if(!c)return -1;c->error.clear();if(!c->views){c->error="supply at least one view before meshing";return -1;}
 try{return c->core->createMesh();}catch(const std::exception &e){c->error=e.what();return -1;}catch(...){c->error="native mesh generation failed";return -1;}
}
int wfc_get_stats(const WFCContext *c,WFCStats *stats){
 if(!c||!stats)return -1;WFCStats result{};result.views=c->views;result.occupancy_fnv1a64=UINT64_C(14695981039346656037);
 for(int z=0;z<SCULPTURE_SIZE;++z)for(int y=0;y<SCULPTURE_SIZE;++y)for(int x=0;x<SCULPTURE_SIZE;++x){bool on=c->core->getBit(x,y,z);result.occupied_voxels+=on;result.occupancy_fnv1a64^=on?1u:0u;result.occupancy_fnv1a64*=UINT64_C(1099511628211);}
 result.triangles=c->core->getMesh().getNumberOfTriangles();result.vertices=c->core->getMesh().getNumberOfVertices();*stats=result;return 0;
}
size_t wfc_copy_positions(const WFCContext *c,float *output,size_t capacity){if(!c)return 0;const auto &a=c->core->getMesh().getVertices();if(output&&capacity>=a.size())std::memcpy(output,a.data(),a.size()*sizeof(float));return a.size();}
size_t wfc_copy_normals(const WFCContext *c,float *output,size_t capacity){if(!c)return 0;const auto &a=c->core->getMesh().getNormals();if(output&&capacity>=a.size())std::memcpy(output,a.data(),a.size()*sizeof(float));return a.size();}
size_t wfc_copy_colors(const WFCContext *c,uint8_t *output,size_t capacity){if(!c)return 0;const auto &a=c->core->getMesh().getColors();if(output&&capacity>=a.size())std::memcpy(output,a.data(),a.size());return a.size();}
const char *wfc_last_error(const WFCContext *c){return c?c->error.c_str():"null native context";}
int wfc_dinosaur_rt(const double *p,float *output){
 if(!p||!output)return -1;for(unsigned i=0;i<12;++i)if(!std::isfinite(p[i]))return -1;
 try{cv::Mat projection(3,4,CV_64F);std::memcpy(projection.data,p,sizeof(double)*12);
  cv::Mat k=(cv::Mat_<double>(3,3)<<3217.3,-78.6,289.9,0,2292.4,-1070.5,0,0,1),rt64=k.inv()*projection,rt;rt64.convertTo(rt,CV_32F);std::memcpy(output,rt.data,12*sizeof(float));return 0;
 }catch(...){return -1;}
}
}
