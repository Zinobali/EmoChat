#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include "bubbleframe.h"

class PictureBubble: public BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(const QPixmap &picture, ChatRole role, QWidget* parent = nullptr);
    ~PictureBubble();
};

#endif // PICTUREBUBBLE_H
