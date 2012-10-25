#ifndef VIDEO_H_
#define VIDEO_H_

typedef void* (*LineDataFunc)(int line);

void video_init(int rows, int cols);
/* Called before the start of each line */
extern LineDataFunc video_line_data_func;

#endif /* VIDEO_H_ */
