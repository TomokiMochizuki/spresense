/****************************************************************************
 * drivers/video/video.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __DRIVERS_VIDEO_VIDEO_H
#define __DRIVERS_VIDEO_VIDEO_H


#include "video_framebuff.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MAX_VIDEO_FILE_PATH     (32)
#define MAX_VIDEO_FMT           (2)
#define VIDEO_FMT_MAIN          (0)
#define VIDEO_FMT_SUB           (1)

#define VIDEO_REMAINING_CAPNUM_INFINITY (-1)

#define VIDEO_ID(x, y) (y)

/****************************************************************************
 * Private Types
 ****************************************************************************/

enum video_state_e
{
  VIDEO_STATE_STREAMOFF = 0, /* Capture trigger event is not received */
  VIDEO_STATE_STREAMON  = 1, /* Capture trigger event is received,
                              * but capture is not operated.
                              */
  VIDEO_STATE_CAPTURE   = 2, /* On capture */
};

enum video_state_transition_cause
{
  CAUSE_VIDEO_STOP  = 0,     /* Stop  capture event for video stream */
  CAUSE_VIDEO_START = 1,     /* Start capture event for video stream */
  CAUSE_VIDEO_DQBUF = 2,     /* DQBUF timing        for video stream */
  CAUSE_STILL_STOP  = 3,     /* Stop  capture event for still stream */
  CAUSE_STILL_START = 4,     /* Start capture event for still stream */
};

enum video_waitend_cause_e
{
  VIDEO_WAITEND_CAUSE_CAPTUREDONE = 0,
  VIDEO_WAITEND_CAUSE_DQCANCEL  = 1,
  VIDEO_WAITEND_CAUSE_STILLSTOP = 2,
};

struct video_wait_capture_s
{
  sem_t                dqbuf_wait_flg;

  /* Save container which capture is done */

  FAR vbuf_container_t *done_container;
  enum video_waitend_cause_e waitend_cause;
};

typedef struct video_wait_capture_s video_wait_capture_t;

struct video_format_s
{
  uint16_t width;
  uint16_t height;
  uint32_t pixelformat;
};

typedef struct video_format_s video_format_t;

struct video_type_inf_s
{
  mutex_t              lock_state;
  enum video_state_e   state;
  int32_t              remaining_capnum;
  video_wait_capture_t wait_capture;
  uint8_t              nr_fmt;
  video_format_t       fmt[MAX_VIDEO_FMT];
  struct v4l2_rect     clip;
  struct v4l2_fract    frame_interval;
  video_framebuff_t    bufinf;
  FAR uint8_t          *bufheap;   /* for V4L2_MEMORY_MMAP buffers */
  FAR struct pollfd    *fds;
  uint32_t             seqnum;
};

typedef struct video_type_inf_s video_type_inf_t;

struct video_scene_params_s
{
  uint8_t mode;   /* enum v4l2_scene_mode */

  int32_t brightness;
  int32_t contrast;
  int32_t saturation;
  int32_t hue;
  bool    awb;
  int32_t red;
  int32_t blue;
  int32_t gamma;
  uint32_t gamma_curve_sz;
  uint8_t *gamma_curve;
  int32_t ev;
  bool    hflip_video;
  bool    vflip_video;
  bool    hflip_still;
  bool    vflip_still;
  int32_t sharpness;
  enum v4l2_colorfx colorfx;
  bool    auto_brightness;
  int32_t rotate;
  enum  v4l2_exposure_auto_type ae;
  int32_t exposure_time;
  int32_t focus;
  bool    af;
  int32_t zoom;
  int32_t iris;
  enum v4l2_auto_n_preset_white_balance wb;
  int32_t wdr;
  bool    stabilization;
  enum v4l2_iso_sensitivity_auto_type iso_auto;
  int32_t iso;
  enum v4l2_exposure_metering meter;
  int32_t spot_pos;
  int32_t threea_lock;
  enum v4l2_flash_led_mode led;
  int32_t jpeg_quality;
};

typedef struct video_scene_params_s video_scene_params_t;

struct video_parameter_name_s
{
  uint32_t id;
  const char *name;
};

typedef struct video_parameter_name_s video_parameter_name_t;

struct video_mng_s
{
  /* Parameter of video_initialize() */

  FAR char                 *devpath;
  mutex_t                  lock_open_num;
  uint8_t                  open_num;
  video_type_inf_t         video_inf;
  video_type_inf_t         still_inf;
  FAR struct imgdata_s     *imgdata;
  FAR struct imgsensor_s   *imgsensor;
  enum v4l2_scene_mode     video_scene_mode;
  uint8_t                  video_scence_num;
  FAR video_scene_params_t *video_scene_param[V4L2_SCENE_MODE_MAX];
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
  bool                     unlinked;
#endif
};

typedef struct video_mng_s video_mng_t;

extern const struct file_operations g_video_fops;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

 /* Get the connected imgsensor ops from video driver.
  *
  *  param [in] sensors: provide imgsensor ops array
  *  param [in] sensor_num: the number of imgsensor ops array
  *
  *  Return on success, imgsensor ops is returned.
  *  On failure, NULL is returned.
  */
FAR struct imgsensor_s *
get_connected_imgsensor(FAR struct imgsensor_s **sensors,
                        size_t sensor_num);

// The following APIs are used to get imgdata and imgsensor ops

/* Get imgdata ops from video driver.
 *
 *  Return on success, imgdata ops is returned.
 *  On failure, NULL is returned.
 */
FAR struct imgdata_s *video_get_imgdata(void);

/* Get imgsensor ops from video driver.
 *
 *  Return on success, imgsensor ops is returned.
 *  On failure, NULL is returned.
 */
FAR struct imgsensor_s **video_get_imgsensors(void);

/* Get the number of imgsensor ops array from video driver.
 *
 *  Return on success, the number of imgsensor ops array is returned.
 *  On failure, 0 is returned.
 */
size_t video_get_sensor_num(void);

#endif /* __DRIVERS_VIDEO_VIDEO_H */
