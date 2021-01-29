#include "camera.h"

Camera::Camera()
{
    this->pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    this->right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    this->up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    this->look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}
Camera::Camera(CameraType cameraType)
{
    this->cameraType = cameraType;
    this->pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    this->right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    this->up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    this->look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}
Camera::~Camera()
{

}
void Camera::strafe(float units)
{
    if(this->cameraType == LANDOBJECT)
    {
        this->pos += D3DXVECTOR3(this->right.x, 0.0f, this->right.z) * units;
    }
    if(this->cameraType == AIRCRAFT)
    {
        this->pos += this->right * units;
    }
}
void Camera::fly(float units)
{

    this->pos += this->up * units;
    
}
void Camera::walk(float units)
{
    if(this->cameraType == LANDOBJECT)
    {
        this->pos += D3DXVECTOR3(this->look.x, 0.0f, this->look.z) * units;
    }
    if(this->cameraType == AIRCRAFT)
    {
        this->pos += this->look * units;
    }
}
void Camera::pitch(float angle)
{
    D3DXMATRIX T;
    D3DXMatrixRotationAxis(&T, &this->right, angle);
    D3DXVec3TransformCoord(&this->up, &this->up, &T);
    D3DXVec3TransformCoord(&this->look, &this->look, &T);
}
void Camera::yaw(float angle)
{
    D3DXMATRIX T;
    if(this->cameraType == LANDOBJECT)
    {
        D3DXMatrixRotationY(&T, angle); 
    }
    if(this->cameraType == AIRCRAFT)
    {
        D3DXMatrixRotationAxis(&T, &this->up, angle);
    }
    D3DXVec3TransformCoord(&this->right, &this->right, &T);
    D3DXVec3TransformCoord(&this->look, &this->look, &T);
}
void Camera::roll(float angle)
{
    if(this->cameraType == AIRCRAFT)
    {
        D3DXMATRIX T;
        D3DXMatrixRotationAxis(&T, &this->look, angle);
        D3DXVec3TransformCoord(&this->right, &this->right, &T);
        D3DXVec3TransformCoord(&this->up, &this->up, &T); 
    }
}
void Camera::getViewMatrix(D3DXMATRIX* V)
{
    // Keep cameras axes orthogonal to each other:
    D3DXVec3Normalize(&this->look, &this->look);
    // get the up vec3
    D3DXVec3Cross(&this->up, &this->look, &this->right);
    D3DXVec3Normalize(&this->up, &this->up);
    // get the right vec3
    D3DXVec3Cross(&this->right, &this->up, &this->look);
    D3DXVec3Normalize(&this->right, &this->right);
    // Build the VIEW MATRIX:
    float x = -D3DXVec3Dot(&this->right, &this->pos); 
    float y = -D3DXVec3Dot(&this->up, &this->pos); 
    float z = -D3DXVec3Dot(&this->look, &this->pos);
    // Fill matrix
    (*V)(0, 0) = this->right.x;
    (*V)(0, 1) = this->up.x;
    (*V)(0, 2) = this->look.x;
    (*V)(0, 3) = 0.0f;

    (*V)(1, 0) = this->right.y;
    (*V)(1, 1) = this->up.y;
    (*V)(1, 2) = this->look.y;
    (*V)(1, 3) = 0.0f;

    (*V)(2, 0) = this->right.z;
    (*V)(2, 1) = this->up.z;
    (*V)(2, 2) = this->look.z;
    (*V)(2, 3) = 0.0f;

    (*V)(3, 0) = x;
    (*V)(3, 1) = y;
    (*V)(3, 2) = z;
    (*V)(3, 3) = 1.0f;
}
void Camera::setCameraType(CameraType cameraType)
{
    this->cameraType = cameraType;
}
void Camera::getPosition(D3DXVECTOR3* pos)
{
    *pos = this->pos;
}
void Camera::setPosition(D3DXVECTOR3* pos)
{
    this->pos = *pos;
}
void Camera::getRight(D3DXVECTOR3* right)
{
    *right = this->right;
}
void Camera::getUp(D3DXVECTOR3* up)
{
    *up = this->up;
}
void Camera::getLook(D3DXVECTOR3* look)
{
    *look = this->look;
}
