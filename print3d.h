#ifndef PRINT3D_H
#define PRINT3D_H

#include <GL/gl.h>

class Print3D
{
public:
    Print3D();
    bool snapshot(int width, int height, GLbyte *buffer, const char *path);
    bool snapshot_bmp(int width, int height, GLbyte *buffer, const char* path);
    bool snapshot_jpg(int width, int height, const char *path, int quality=85);
    bool snapshot_tiff(int width, int height, const char* path);
};

#endif // PRINT3D_H
