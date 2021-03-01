#include <iostream>

#include <Image2d.h>
#include <ImageUtils.h>

int main(int argc, char ** argv)
{
#if defined (_DEBUG) || defined (DEBUG)
	printf("Running DEBUG build => tests speed will be incorrect.\n");
#endif
    if (argc < 4)
    {
        std::cout << "zadej argumenty: out/test1.png res/lenna_rgb.png out/test2.png" << std::endl;
        return EXIT_FAILURE;
    }

	
	{
		Image2d<uint8_t> img(512, 512, ColorSpace::PixelFormat::RGB);

		img.Clear(255);

		uint8_t red[3] = { 255, 0, 0 };
		ImageUtils::DrawLine(img, red, 0, 0, 512, 512);

		uint8_t blue[3] = { 0, 0, 255 };
		ImageUtils::DrawLine(img, blue, 0, 512, 512, 0);

		img.Save(argv[1]);
	}

	{
		Image2d<uint8_t> img(argv[2]);
		
		uint8_t red[3] = { 255, 0, 0 };
		ImageUtils::DrawLine(img, red, 0, 0, 255, 255);

		uint8_t green[3] = { 0, 255, 0 };
		ImageUtils::DrawLine(img, green, 0, 255, 255, 0);

		img.Save(argv[3]);
	}

	return EXIT_SUCCESS;
}
