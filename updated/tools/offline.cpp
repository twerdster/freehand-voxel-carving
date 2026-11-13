// Offline driver for Alex Fish's recovered carving/meshing implementation.
#include <vector>
#include <sstream>
#include <limits>
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <memory>
#include <new>
#include <set>
#include <stdexcept>
#include <sys/stat.h>
#include <png.h>
#include "Sculpture.h"
#include "matricesData.h"

void require(bool yes,const std::string& message){if(!yes)throw std::runtime_error(message);}
size_t count(const Sculpture& s){size_t n=0;for(int z=0;z<SCULPTURE_SIZE;++z)for(int y=0;y<SCULPTURE_SIZE;++y)for(int x=0;x<SCULPTURE_SIZE;++x)n+=s.getBit(x,y,z);return n;}
void test(const std::string& which){
    if(which=="boundary"){
        void* memory=::operator new(sizeof(Sculpture));std::memset(memory,0xa5,sizeof(Sculpture));
        Sculpture* s=new(memory)Sculpture();size_t mismatches=0;
        for(int z=0;z<SCULPTURE_SIZE;++z)for(int y=0;y<SCULPTURE_SIZE;++y)for(int x=0;x<SCULPTURE_SIZE;++x)
            mismatches+=s->getBit(x,y,z)!=(x>0&&y>0&&z>0&&x<SCULPTURE_SIZE-1&&y<SCULPTURE_SIZE-1&&z<SCULPTURE_SIZE-1);
        s->~Sculpture();::operator delete(memory);require(mismatches==0,"constructor did not initialize occupancy boundary/interior deterministically");
    }else if(which=="zbuffer"){
        Zbuffer buffer(3,7);cv::Mat rt=cv::Mat::eye(3,4,CV_32F);
        buffer.addVertex(2,2,cv::Vec3f(1,1,-4),rt);require(buffer.getVertex(2,2)==-4,"Zbuffer failed scalar depth insertion");
        buffer.addVertex(2,2,cv::Vec3f(2,3,-8),rt);require(buffer.getVertex(2,2)==-4,"farther depth replaced nearer depth");
        buffer.addVertex(2,2,cv::Vec3f(1,1,-2),rt);require(buffer.getVertex(2,2)==-2,"nearer depth did not replace farther depth");
        buffer.addVertex(2,6,cv::Vec3f(1,1,-3),rt);require(buffer.getVertex(2,6)==-3,"portrait image row was incorrectly clipped");
    }else if(which=="carving"){
        auto s=std::unique_ptr<Sculpture>(new Sculpture());Cube b;b.setSize(SCULPTURE_SIZE);b.setCorner(0,0,0);
        cv::Mat mask(SCULPTURE_SIZE,SCULPTURE_SIZE,CV_8UC4,cv::Scalar(0,0,0,255));
        mask(cv::Rect(SCULPTURE_SIZE/4,0,SCULPTURE_SIZE/2,SCULPTURE_SIZE)).setTo(cv::Scalar(255,255,255,255));
        cv::Mat projection=(cv::Mat_<double>(3,4)<<1,0,0,0,0,1,0,0,0,0,0,1),rt=cv::Mat::eye(3,4,CV_32F);
        size_t initial=count(*s);ImageData view(mask,mask,projection,rt);s->sculpt(view,b);
        require(count(*s)==initial,"first silhouette vote should retain STEP=2 voxels");s->sculpt(view,b);
        for(int z=0;z<SCULPTURE_SIZE;++z)for(int y=0;y<SCULPTURE_SIZE;++y)for(int x=0;x<SCULPTURE_SIZE;++x){bool expected=x>=SCULPTURE_SIZE/4&&x<3*SCULPTURE_SIZE/4&&y>0&&y<SCULPTURE_SIZE-1&&z>0&&z<SCULPTURE_SIZE-1;require(s->getBit(x,y,z)==expected,"carved volume differs from analytic rectangular prism");}
    }else if(which=="marching-cubes"){
        const int corners[8][3]={{0,0,0},{0,0,1},{1,0,1},{1,0,0},{0,1,0},{0,1,1},{1,1,1},{1,1,0}};
        GRIDCELL cell;for(int i=0;i<8;++i){cell.p[i]=Vector3DEx(corners[i][0],corners[i][1],corners[i][2]);cell.val[i]=0;}
        std::vector<Triangle> triangles;Polygonise(cell,.5,triangles);require(triangles.empty(),"empty cell generated triangles");
        cell.val[0]=1;Polygonise(cell,.5,triangles);require(triangles.size()==1,"one occupied corner should produce one triangle");
        std::set<std::array<int,3>> actual;
        for(int j=0;j<3;++j){std::array<int,3> point;for(int k=0;k<3;++k){double value=triangles[0].p[j][k];require(std::isfinite(value)&&std::abs(value*2-std::round(value*2))<1e-9,"marching-cubes interpolation differs from analytic half-edge");point[k]=int(std::round(value*2));}actual.insert(point);}
        require(actual==std::set<std::array<int,3>>{{1,0,0},{0,1,0},{0,0,1}},"one-corner triangle does not intersect the three expected half-edges");
    }else throw std::runtime_error("unknown test");
    std::cout<<"PASS "<<which<<'\n';
}
cv::Mat loadRGBA(const std::string& filename){png_image input{};input.version=PNG_IMAGE_VERSION;require(png_image_begin_read_from_file(&input,filename.c_str()),"cannot read "+filename);input.format=PNG_FORMAT_RGBA;cv::Mat rgba(input.height,input.width,CV_8UC4);bool ok=png_image_finish_read(&input,nullptr,rgba.data,0,nullptr);png_image_free(&input);require(ok,"cannot decode "+filename);return rgba;}
void savePNG(const std::string& filename,const cv::Mat& matrix){png_image output{};output.version=PNG_IMAGE_VERSION;output.width=matrix.cols;output.height=matrix.rows;output.format=matrix.channels()==1?PNG_FORMAT_GRAY:PNG_FORMAT_RGBA;require(png_image_write_to_file(&output,filename.c_str(),0,matrix.data,0,nullptr),"cannot write PNG "+filename);}
cv::Mat loadProjection(const std::string& data,int i,bool historical){
    if(historical)return cv::Mat(3,4,CV_64F,demoP[i]).clone();
    std::ifstream f(data+"/matrices/"+std::to_string(i)+".txt");require(bool(f),"missing projection matrix");
    std::string text((std::istreambuf_iterator<char>(f)),{});std::replace(text.begin(),text.end(),';',' ');std::istringstream stream(text);double values[12];for(double& value:values)require(bool(stream>>value),"incomplete projection matrix");
    // The first nine text files are row-major; subsequent files are column-
    // major. Identify the packing against the unchanged upstream demo table,
    // rather than optimizing a calibration against the reconstructed result.
    cv::Mat row(3,4,CV_64F),column(3,4,CV_64F);double rowError=0,columnError=0;
    for(int r=0;r<3;++r)for(int c=0;c<4;++c){row.at<double>(r,c)=values[r*4+c];column.at<double>(r,c)=values[c*3+r];rowError=std::max(rowError,std::abs(values[r*4+c]-demoP[i][r*4+c]));columnError=std::max(columnError,std::abs(values[c*3+r]-demoP[i][r*4+c]));}
    require(std::min(rowError,columnError)<.0001,"projection text does not agree with the upstream calibration table in either packing");return rowError<columnError?row:column;
}
bool off(const ImageData& view,double x,double y,double z){
    const cv::Mat& p=view.P;double u=p.at<double>(0,0)*x+p.at<double>(0,1)*y+p.at<double>(0,2)*z+p.at<double>(0,3);
    double v=p.at<double>(1,0)*x+p.at<double>(1,1)*y+p.at<double>(1,2)*z+p.at<double>(1,3);
    double w=p.at<double>(2,0)*x+p.at<double>(2,1)*y+p.at<double>(2,2)*z+p.at<double>(2,3);
    u=std::round(u/w);v=std::round(v/w);return !std::isfinite(u)||!std::isfinite(v)||u<0||v<0||u>=view.silhouette.cols||v>=view.silhouette.rows||view.silhouette.at<cv::Vec4b>(int(v),int(u))[1]==0;
}
void demo(const std::string& data,const std::string& output,bool historical){
    mkdir(output.c_str(),0755);auto s=std::unique_ptr<Sculpture>(new Sculpture());Cube box;box.setSize(DEMO_MESH_SIZE);box.setCorner(DEMO_X_POSITION,DEMO_Y_POSITION,DEMO_Z_POSITION);
    std::vector<ImageData> views;std::vector<size_t> counts{count(*s)};
    cv::Mat k=(cv::Mat_<double>(3,3)<<3217.3,-78.6,289.9,0,2292.4,-1070.5,0,0,1);
    for(int i=0;i<36;++i){
        cv::Mat image=loadRGBA(data+"/pictures/viff."+std::to_string(i+1)+".png"),mask=loadRGBA(data+"/silhouettes/sil_"+std::to_string(i+1)+".png"),p=loadProjection(data,i,historical),rt64=k.inv()*p,rt;rt64.convertTo(rt,CV_32F);
        views.emplace_back(image,mask,p,rt);s->sculpt(views.back(),box);counts.push_back(count(*s));require(counts.back()<=counts[counts.size()-2],"carving increased occupied volume");std::cout<<"view "<<i+1<<": "<<counts.back()<<" occupied voxels\n";
    }
    require(counts.back()>1000&&counts.back()<counts.front()/2,"dinosaur reconstruction unexpectedly empty or nearly uncarved");
    size_t mismatches=0;
    for(int z=0;z<SCULPTURE_SIZE;++z)for(int y=0;y<SCULPTURE_SIZE;++y)for(int x=0;x<SCULPTURE_SIZE;++x){
        bool expected=false;if(x>0&&y>0&&z>0&&x<SCULPTURE_SIZE-1&&y<SCULPTURE_SIZE-1&&z<SCULPTURE_SIZE-1){int votes=0;double wx=DEMO_X_POSITION+double(x)/SCULPTURE_SIZE*DEMO_MESH_SIZE,wy=DEMO_Y_POSITION+double(y)/SCULPTURE_SIZE*DEMO_MESH_SIZE,wz=DEMO_Z_POSITION+double(z)/SCULPTURE_SIZE*DEMO_MESH_SIZE;for(const auto& view:views){votes+=off(view,wx,wy,wz);if(votes>=STEP)break;}expected=votes<STEP;}mismatches+=expected!=s->getBit(x,y,z);
    }
    require(mismatches==0,"independent full-volume vote reference differs from original carving");
    int faces=s->createMesh();const Mesh& mesh=s->getMesh();const auto& vertices=mesh.getVertices();const auto& colors=mesh.getColors();
    require(faces>100&&vertices.size()==size_t(faces)*9&&colors.size()==size_t(faces)*12,"mesh arrays do not match triangle count");
    size_t degenerate=0;std::set<unsigned> uniqueColors;
    for(size_t i=0;i<vertices.size();++i)require(std::isfinite(vertices[i])&&std::abs(vertices[i])<=SCULPTURE_SIZE/2,"invalid mesh vertex");
    for(size_t i=0;i<vertices.size();i+=9){cv::Vec3f a(vertices[i+3]-vertices[i],vertices[i+4]-vertices[i+1],vertices[i+5]-vertices[i+2]),b(vertices[i+6]-vertices[i],vertices[i+7]-vertices[i+1],vertices[i+8]-vertices[i+2]);degenerate+=cv::norm(a.cross(b))<1e-8;}
    require(degenerate==0,"mesh contains zero-area triangles");for(size_t i=0;i<colors.size();i+=4)uniqueColors.insert(unsigned(colors[i])|(unsigned(colors[i+1])<<8)|(unsigned(colors[i+2])<<16));require(uniqueColors.size()>100,"texturing did not produce varied source-image colors");
    std::vector<double> ious;double sum=0,minimum=1;
    for(size_t n=0;n<views.size();++n){const auto& view=views[n];cv::Mat raster(view.silhouette.rows,view.silhouette.cols,CV_8U,cv::Scalar(0));
        for(size_t i=0;i<vertices.size();i+=9){cv::Point points[3];for(int j=0;j<3;++j){double x=DEMO_X_POSITION+(vertices[i+j*3]+SCULPTURE_SIZE/2)*DEMO_MESH_SIZE/SCULPTURE_SIZE,y=DEMO_Y_POSITION+(vertices[i+j*3+1]+SCULPTURE_SIZE/2)*DEMO_MESH_SIZE/SCULPTURE_SIZE,z=DEMO_Z_POSITION+(SCULPTURE_SIZE/2-vertices[i+j*3+2])*DEMO_MESH_SIZE/SCULPTURE_SIZE;cv::Mat v=(cv::Mat_<double>(4,1)<<x,y,z,1),q=view.P*v;points[j]=cv::Point(cvRound(q.at<double>(0)/q.at<double>(2)),cvRound(q.at<double>(1)/q.at<double>(2)));}cv::fillConvexPoly(raster,points,3,cv::Scalar(255));}
        cv::Mat gt,intersection,united;cv::extractChannel(view.silhouette,gt,1);cv::threshold(gt,gt,0,255,cv::THRESH_BINARY);cv::bitwise_and(raster,gt,intersection);cv::bitwise_or(raster,gt,united);double iou=double(cv::countNonZero(intersection))/cv::countNonZero(united);ious.push_back(iou);sum+=iou;minimum=std::min(minimum,iou);
        if(n==0){savePNG(output+"/input.png",view.image);savePNG(output+"/silhouette.png",gt);savePNG(output+"/reprojected-silhouette.png",raster);}
    }
    require(sum/views.size()>.75&&minimum>.60,"input-view silhouette overlap is unexpectedly poor");
    std::ofstream ply(output+"/dinosaur.ply");ply<<s->plyData();require(bool(ply),"cannot write PLY");std::ofstream obj(output+"/dinosaur.obj");obj<<s->objData();
    std::ofstream metrics(output+"/metrics.json");metrics<<std::setprecision(12)<<"{\n  \"input\": \"bundled dinosaur photographs, silhouettes and calibration; Ben Tordoff space-carving example\",\n  \"views\": 36,\n  \"matrix_source\": \""<<(historical?"rounded upstream demoP header":"full-precision bundled matrix text files")<<"\",\n  \"grid_size\": "<<SCULPTURE_SIZE<<",\n  \"rejection_votes\": "<<STEP<<",\n  \"occupied_voxels\": "<<counts.back()<<",\n  \"independent_occupancy_reference_mismatches\": "<<mismatches<<",\n  \"triangles\": "<<faces<<",\n  \"vertices\": "<<vertices.size()/3<<",\n  \"degenerate_triangles\": "<<degenerate<<",\n  \"unique_rgb_colors\": "<<uniqueColors.size()<<",\n  \"input_view_silhouette_mean_iou\": "<<sum/views.size()<<",\n  \"input_view_silhouette_min_iou\": "<<minimum<<",\n  \"input_view_silhouette_iou\": [";for(size_t i=0;i<ious.size();++i){if(i)metrics<<", ";metrics<<ious[i];}metrics<<"],\n  \"occupancy_after_each_view\": [";for(size_t i=0;i<counts.size();++i){if(i)metrics<<", ";metrics<<counts[i];}metrics<<"],\n  \"status\": \"passed\"\n}\n";
    require(bool(obj)&&bool(metrics),"cannot write reconstruction output");
    std::cout<<"PASS dinosaur: "<<faces<<" triangles, "<<uniqueColors.size()<<" colors; silhouette IoU mean "<<sum/views.size()<<", minimum "<<minimum<<"; full-volume reference mismatches "<<mismatches<<'\n';
}
int main(int argc,char** argv){try{if(argc==3&&std::string(argv[1])=="--test"){test(argv[2]);return 0;}if(argc>=4&&std::string(argv[1])=="--demo"){demo(argv[2],argv[3],argc==5&&std::string(argv[4])=="--historical-matrices");return 0;}std::cerr<<"Usage: freehand_offline --test {boundary|zbuffer|carving|marching-cubes}\n       freehand_offline --demo DINOSAUR_DIRECTORY OUTPUT_DIRECTORY [--historical-matrices]\n";return 2;}catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
