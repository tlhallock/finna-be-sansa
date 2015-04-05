#ifndef DISPLAY_H
#define DISPLAY_H

class Image;

namespace gfx
{

void initializeDisplay();
void display(const Image& scene);
void destroyScene();

}

#endif // DISPLAY_H
