#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager        shaderManager;            // 着色器管理器
GLMatrixStack        modelViewMatrix;        // 模型视图矩阵
GLMatrixStack        projectionMatrix;        // 投影矩阵
GLFrustum            viewFrustum;            // 视景体
GLGeometryTransform    transformPipeline;        // 几何图形变换管道

GLTriangleBatch        torusBatch;             // 花托批处理
GLBatch                floorBatch;             // 地板批处理

//**2、定义公转球的批处理（公转自转）**
GLTriangleBatch     sphereBatch;            //球批处理

//**3、角色帧 照相机角色帧（全局照相机实例）
GLFrame             cameraFrame;

//**4、添加附加随机球
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

//**5、添加纹理
//纹理标记数组
GLuint uiTextures[3];

void ShutdownRC(void) {
    
}

void SpeacialKeys(int key,int x,int y) {
    
}

void drawSomething(GLfloat yRot) {
    
}

bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
    
    GLbyte *pBytes;
    GLint newWidth,newHeight,newComponents;
    GLenum eFormate;
    
    pBytes = gltReadTGABits(szFileName, &newWidth, &newHeight, &newComponents, &eFormate);
    if (pBytes == NULL) {
        return false;
    }
    
    //设置纹理参数
    
    //环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    //过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    glTexImage2D(GL_TEXTURE_2D, 0, newComponents, newWidth, newHeight, 0, eFormate, GL_UNSIGNED_BYTE, pBytes);
    
    free(pBytes);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
    //4.加载Mip,纹理生成所有的Mip层
    //参数：GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return true;
}

void RenderScene(void) {
    
}

void ChangeSize(int nWidth, int nHeight) {
    
    glViewport(0, 0, nWidth, nHeight);
    
    viewFrustum.SetPerspective(65.0f, float(nWidth)/float(nHeight), 1.0f, 500.0f);
    modelViewMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void SetupRC() {
    
    //设置清屏颜色到颜色缓存区
    glClearColor(0, 0, 0, 1.0f);
    
    //初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
//    //开启深度测试
//    glEnable(GL_DEPTH_TEST);
//    //开启背面剔除
//    glEnable(GL_CULL_FACE);
    
    // 设置大球
    gltMakeSphere(torusBatch, 0.4f, 40, 80);
    
    // 设置小球(公转自转)
    gltMakeSphere(sphereBatch, 0.1f, 13, 26);
    
    // 设置地板顶点数据&地板纹理
    GLfloat texSize = 10.0f;
    floorBatch.Begin(GL_TRIANGLE_FAN, 4, 1);
    
    floorBatch.MultiTexCoord2f(0, 0, 0);
    floorBatch.Vertex3f(-20.0f, -0.41f, 20.0f);
    
    floorBatch.MultiTexCoord2f(0, texSize, 0.0f);
    floorBatch.Vertex3f(20.0f, -0.41f, 20.0f);
    
    floorBatch.MultiTexCoord2f(0, texSize, texSize);
    floorBatch.Vertex3f(20.0f, -0.41f, -20.0f);
    
    floorBatch.MultiTexCoord2f(0, 0, texSize);
    floorBatch.Vertex3f(-20.0f, -0.41f, -20.0f);
    
    floorBatch.End();
    
    for (int i = 0; i < NUM_SPHERES; i++) {
        GLfloat x = (GLfloat)(rand() % 400 - 200) * 0.1f;
        GLfloat z = (GLfloat)(rand() % 400 - 200) * 0.1f;
        
        spheres[i].SetOrigin(x, 0, z);
    }
    
    //生成纹理对象
    glGenTextures(3, uiTextures);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    LoadTGATexture("marble.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    LoadTGATexture("marslike.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    LoadTGATexture("moonlike.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
}

int main(int argc, char* argv[]) {
    
    gltSetWorkingDirectory(argv[0]);

    // 标准初始化
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("SphereWorld");
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpeacialKeys);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    glutMainLoop();
    ShutdownRC();
    
    return 0;
}
