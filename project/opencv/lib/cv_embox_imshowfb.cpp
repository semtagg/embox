/**
 * @file
 *
 * @date   12.04.2021
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <drivers/video/fb.h>
#include <util/log.h>
#include <algorithm>
#include <cv_embox_imshowfb.hpp>

void imshowfb(cv::Mat& img, int fbx) {
    struct fb_info *fbi;
    int w, h;

    fbi = fb_lookup(fbx);
    if (!fbi) {
		fprintf(stderr, "%s: fb%d not found\n", __func__, fbx);

        return;
    }

	log_debug("\nimage width:    %d\n"
	          "image height:   %d\n"
	          "image size:     %dx%d\n"
	          "image depth:    %d\n"
	          "image channels: %d\n"
	          "image type:     %d",
	          img.cols, img.rows, img.size().width, img.size().height,
	          img.depth(), img.channels(), img.type());

	if (img.channels() != 1 && img.channels() != 3) {
		fprintf(stderr, "Unsupported channels count: %d\n", img.channels());

		return;
	}

    h = std::min((int) fbi->var.yres, img.rows);
    w = std::min((int) (fbi->var.bits_per_pixel * fbi->var.xres) / 8, img.channels() * img.cols);

    for (int y = 0; y < h; y++) {
        const uchar *row = &img.at<uchar>(y, 0);
        for (int x = 0; x < w; x += img.channels()) {
            unsigned rgb888;

			switch (img.channels()) {
			case 1:
			{
				unsigned val = unsigned(row[x]);

            	rgb888 = 0xFF000000 | val | (val << 8) | (val << 16);

				break;
			}
			case 3:
            	rgb888 =
            	    0xFF000000 |
            	    unsigned(row[x + 2]) |
            	    (unsigned(row[x + 1]) << 8) |
            	    (unsigned(row[x]) << 16);

				break;
			default:
				break;
			}

            ((uint32_t *) fbi->screen_base)[fbi->var.xres * y + x / img.channels()] = rgb888;
        }
    }
}
