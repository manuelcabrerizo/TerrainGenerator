#ifndef __cameraH__
#define __cameraH__
#include <d3dx9.h>
class Camera
{
public:
    enum CameraType { LANDOBJECT, AIRCRAFT };
    Camera();
    Camera(CameraType cameraType);
    ~Camera();
    void strafe(float units);
    void fly(float units);
    void walk(float units);
    void pitch(float angle);
    void yaw(float angle);
    void roll(float angle);
    void getViewMatrix(D3DXMATRIX* V);
    void setCameraType(CameraType cameraType);
    void getPosition(D3DXVECTOR3* pos);
    void setPosition(D3DXVECTOR3* pos);
    void getRight(D3DXVECTOR3* right);
    void getUp(D3DXVECTOR3* up);
    void getLook(D3DXVECTOR3* look);
private:
    CameraType cameraType;
    D3DXVECTOR3 right;
    D3DXVECTOR3 up;
    D3DXVECTOR3 look;
    D3DXVECTOR3 pos;
};
#endif
