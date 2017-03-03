#ifndef __IPREVIEW_CALLBACK_H__
#define __IPREVIEW_CALLBACK_H__
#include <cstdint>
#include <memory>

struct IPreviewCallback
{
	virtual void onPreviewImage(std::shared_ptr<uint8_t> image, int imageSize, int width, int height) = 0;
};

#endif //!__IPREVIEW_CALLBACK_H__
