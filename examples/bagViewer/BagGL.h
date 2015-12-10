#ifndef BAGGL_H
#define BAGGL_H

#include "GLWindow.h"
#include "BagIO.h"
#include <QVector3D>
#include <QTime>
#include <memory>
#include <QOpenGLTexture>
#include <QOpenGLDebugLogger>
#include <QMatrix4x4>
#include <QVector2D>

struct Tile;

struct TileGL
{
    QOpenGLTexture elevations;
    QOpenGLTexture normals;
    QOpenGLTexture uncertainties;
    TileGL(): elevations(QOpenGLTexture::Target2D), normals(QOpenGLTexture::Target2D), uncertainties(QOpenGLTexture::Target2D) {}
    ~TileGL(){}
};

class BagGL: public GLWindow
{
    Q_OBJECT
    
public:
    BagGL();
    ~BagGL();
    
    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;
    
    bool openBag(QString const &bagFileName);
    void closeBag();
    void setColormap(std::string const &cm);
    void setDrawStyle(std::string const &ds);
    
    static const GLuint primitiveReset;
    
public slots:
    void resetView();
    void messageLogged(const QOpenGLDebugMessage & debugMessage);
    
protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    
private:
    QMatrix4x4 genMatrix();
    
    GLuint matrixUniform;
    GLuint normMatrixUniform;
    GLuint lightDirectionUniform;
    GLuint minElevationUniform;
    GLuint maxElevationUniform;
    GLuint elevationMapUniform;
    GLuint spacingUniform;
    GLuint lowerLeftUniform;
    GLuint tileSizeUniform;
    GLuint colorMapUniform;
    GLuint normalMapUniform;
    
    GLuint tileVAO;
    GLuint tileBuffers[2];
    //GLsizei tileIndeciesCount;
    std::vector<GLsizei> lodIndecies;
    
    BagIO bag;

    QOpenGLShaderProgram *program;
    
#ifndef NDEBUG
    QOpenGLDebugLogger gldebug;
#endif
    
    typedef std::shared_ptr<QOpenGLTexture> QOpenGLTexturePtr;
    typedef std::map<std::string,QOpenGLTexturePtr> ColormapMap;
    ColormapMap colormaps;
    std::string currentColormap;
    
    std::string drawStyle;
    
    float nearPlane, farPlane;
    
    float zoom;
    float yaw;
    float pitch;
    bool rotating;
    QPoint lastPosition;
    
    QVector3D translatePosition;
    
    bool translating;
    QVector3D translateStartPosition, translateEndPosition;
    QTime translateStartTime;
    
    float heightExaggeration;
    bool adjustingHeightExaggeration;
    
    struct Frustum
    {
        struct Plane
        {
            QVector3D ll,lr,ul,ur;
            Plane(float depth):
                ll(-1.0,-1.0,depth),
                lr(1.0,-1.0,depth),
                ul(-1.0,1.0,depth),
                ur(1.0,1.0,depth)
                {}
        };
        
        struct PlaneEq
        {
            QVector3D v1,v2,v3;

            void set(QVector3D v1, QVector3D v2, QVector3D v3)
            {
                this->v1 = v1;
                this->v2 = v2;
                this->v3 = v3;
            }

            float whichSide(QVector3D p) const
            {
                return p.distanceToPlane(v1,v2,v3);
            }
        };
        
        QMatrix4x4 matrix,imatrix;
        Plane nearPlane;
        Plane farPlane;
        PlaneEq l,r,t,b,n,f;
        QVector2D viewportSize;
        
        Frustum(QMatrix4x4 m):matrix(m),imatrix(m.inverted()), nearPlane(-1.0),farPlane(1.0)
        {
            nearPlane.ll = imatrix*nearPlane.ll;
            nearPlane.lr = imatrix*nearPlane.lr;
            nearPlane.ul = imatrix*nearPlane.ul;
            nearPlane.ur = imatrix*nearPlane.ur;

            farPlane.ll = imatrix*farPlane.ll;
            farPlane.lr = imatrix*farPlane.lr;
            farPlane.ul = imatrix*farPlane.ul;
            farPlane.ur = imatrix*farPlane.ur;
            
            n.set(nearPlane.ll, nearPlane.ul, nearPlane.lr);
            f.set( farPlane.ur,  farPlane.ul,  farPlane.lr);
            l.set( farPlane.ll,  farPlane.ul, nearPlane.ll);
            r.set(nearPlane.lr, nearPlane.ur,  farPlane.lr);
            t.set(nearPlane.ul,  farPlane.ul, nearPlane.ur);
            b.set( farPlane.lr,  farPlane.ll, nearPlane.lr);
        }
        
        QVector2D drawSize(QVector3D const &p0, QVector3D const &p1)
        {
            QVector3D p0p = matrix*p0;
            QVector3D p1p = matrix*p1;
            return QVector2D( viewportSize.x()*std::abs(p0p.x()-p1p.x())/2.0 , viewportSize.y()*std::abs(p0p.y()-p1p.y())/2.0);
        }
    };
    
    bool isCulled(const BagGL::Frustum& f, const Tile& t, const BagIO::MetaData& meta) const;
                  
};

#endif