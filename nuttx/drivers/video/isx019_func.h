/****************************************************************************
 * drivers/video/isx019_func.h
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

#ifndef __DRIVERS_VIDEO_ISX019_FUNC_H
#define __DRIVERS_VIDEO_ISX019_FUNC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/signal.h>
#include <nuttx/clock.h>
#include <arch/board/board.h>
#include <nuttx/video/isx019.h>
#include <nuttx/video/imgsensor.h>
#include <math.h>
#include <nuttx/mutex.h>

#include <nuttx/video/imgsensor.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Wait time on power on sequence. */

#define TRANSITION_TIME_TO_STARTUP   (130 * USEC_PER_MSEC) /* unit : usec */
#define TRANSITION_TIME_TO_STREAMING (40 * USEC_PER_MSEC)  /* unit : usec */
#define DELAY_TIME_JPEGDQT_SWAP      (35 * USEC_PER_MSEC)  /* unit : usec */

/* For get_supported_value() I/F */

#define SET_RANGE(range, min, max, s, def) \
        do                                 \
          {                                \
            (range).minimum       = (min); \
            (range).maximum       = (max); \
            (range).step          = (s);   \
            (range).default_value = (def); \
          }                                \
        while (0);

#define SET_DISCRETE(disc, nr, val, def)   \
        do                                 \
          {                                \
            (disc).nr_values     = (nr);   \
            (disc).values        = (val);  \
            (disc).default_value = (def);  \
          }                                \
        while (0);

#define SET_ELEMS(elem, nr, min, max, s)   \
        do                                 \
          {                                \
            (elem).nr_elems      = (nr);   \
            (elem).minimum       = (min);  \
            (elem).maximum       = (max);  \
            (elem).step          = (s);    \
          }                                \
        while (0);

#define COMPARE_FRAMESIZE(w, h, sup_w, sup_h)  (((w) == (sup_w)) && \
                                                ((h) == (sup_h)))

#define VALIDATE_FRAMESIZE(w, h) (COMPARE_FRAMESIZE((w), (h), 1280, 960) || \
                                  COMPARE_FRAMESIZE((w), (h), 1280, 720) || \
                                  COMPARE_FRAMESIZE((w), (h),  640, 480) || \
                                  COMPARE_FRAMESIZE((w), (h),  640, 360) || \
                                  COMPARE_FRAMESIZE((w), (h),  480, 360) || \
                                  COMPARE_FRAMESIZE((w), (h),  320, 240) || \
                                  COMPARE_FRAMESIZE((w), (h),  160, 120))

#define VALIDATE_THUMNAIL_SIZE(m, s) (((s) != 0) && \
                                      ((m) % (s) == 0) && \
                                      ((m) / (s) < 5)  && \
                                      ((m) / (s) > 0))

/* For set_value() and get_value() I/F */

#define SET_REGINFO(a, c, o, t, s) do \
                                  {                      \
                                    (a)->category = (c); \
                                    (a)->offset   = (o); \
                                    (a)->type     = (t); \
                                    (a)->size     = (s); \
                                  }                      \
                                while (0);

/* Register type, which represents the number of bits and
 * whether it is signed or unsigned.
 */

#define ISX019_REGTYPE_INT8   (0)
#define ISX019_REGTYPE_UINT8  (1)
#define ISX019_REGTYPE_INT16  (2)

#define SET_REGINFO_INT8(a, c, o) SET_REGINFO(a, c, o, ISX019_REGTYPE_INT8, 1)
#define SET_REGINFO_UINT8(a, c, o) SET_REGINFO(a, c, o, ISX019_REGTYPE_UINT8, 1)
#define SET_REGINFO_INT16(a, c, o) SET_REGINFO(a, c, o, ISX019_REGTYPE_INT16, 2)

#define VALIDATE_RANGE(v, min, max, step) (((v) >= (min)) && \
                                           ((v) <= (max)) && \
                                           (((v) - (min)) % (step) == 0))

/* Offset for IMGSENSOR_ID_3A_PARAMETER control */

#define OFFSET_3APARAMETER_AWB_R      (0)
#define OFFSET_3APARAMETER_AWB_B      (2)
#define OFFSET_3APARAMETER_AE_SHTTIME (4)
#define OFFSET_3APARAMETER_AE_GAIN    (8)

/* Index of array for drive mode setting */

#define INDEX_SENS     (0)
#define INDEX_POST     (1)
#define INDEX_SENSPOST (2)
#define INDEX_IO       (3)

/* Timer value for power on control */

#define ISX019_ACCESSIBLE_WAIT_SEC    (0)
#define ISX019_ACCESSIBLE_WAIT_USEC   (200000)
#define FPGA_ACCESSIBLE_WAIT_SEC      (1)
#define FPGA_ACCESSIBLE_WAIT_USEC     (0)

/* Array size of DQT setting for JPEG quality */

#define JPEG_DQT_ARRAY_SIZE (64)

/* ISX019 standard master clock */

#define ISX019_STANDARD_MASTER_CLOCK (27000000)

/* Vivid colors setting */

#define VIVID_COLORS_SATURATION (0xf0)
#define VIVID_COLORS_SHARPNESS  (0x20)

/* Black white colors setting */

#define BW_COLORS_SATURATION (0x00)

/* Definition for calculation of extended frame number */

#define VTIME_PER_FRAME    (30518)
#define INTERVAL_PER_FRAME (33333)

/* ISX019 image sensor output frame size. */

#define ISX019_WIDTH  (1280)
#define ISX019_HEIGHT (960)

/* The number of whole image splits for spot position decision. */

#define ISX019_SPOT_POSITION_SPLIT_NUM_X (9)
#define ISX019_SPOT_POSITION_SPLIT_NUM_Y (7)

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct isx019_default_value_s
{
  int32_t brightness;
  int32_t contrast;
  int32_t saturation;
  int32_t hue;
  int32_t awb;
  int32_t gamma;
  int32_t ev;
  int32_t hflip_video;
  int32_t vflip_video;
  int32_t hflip_still;
  int32_t vflip_still;
  int32_t sharpness;
  int32_t ae;
  int32_t wbmode;
  int32_t hdr;
  int32_t iso_auto;
  int32_t meter;
  int32_t spot_pos;
  int32_t threealock;
  int32_t jpgquality;
};

typedef struct isx019_default_value_s isx019_default_value_t;

struct isx019_rect_s
{
  int32_t left;
  int32_t top;
  uint32_t width;
  uint32_t height;
};

typedef struct isx019_rect_s isx019_rect_t;

struct isx019_dev_s
{
  struct imgsensor_s sensor;
  mutex_t fpga_lock;
  mutex_t i2c_lock;
  FAR struct i2c_master_s *i2c;
  struct i2c_config_s i2c_cfg;
  float clock_ratio;
  isx019_default_value_t  default_value;
  imgsensor_stream_type_t stream;
  imgsensor_white_balance_t wb_mode;
  uint8_t flip_video;
  uint8_t flip_still;
  isx019_rect_t clip_video;
  isx019_rect_t clip_still;
  double  gamma;
  int32_t jpg_quality;
  int32_t hue;
  imgsensor_colorfx_t colorfx;
};

typedef struct isx019_dev_s isx019_dev_t;

typedef CODE int32_t (*convert_t)(int32_t value32);

typedef CODE int (*setvalue_t)(FAR isx019_dev_t *priv,
                               imgsensor_value_t value);
typedef CODE int (*getvalue_t)(FAR isx019_dev_t *priv,
                               FAR imgsensor_value_t *value);

struct isx019_reginfo_s
{
  uint16_t category;
  uint16_t offset;
  uint8_t  type;
  uint8_t  size;
};

typedef struct isx019_reginfo_s isx019_reginfo_t;

struct isx019_fpga_jpg_quality_s
{
  /* JPEG quality */

  int quality;

  /* DQT header setting for y component */

  uint8_t y_head[JPEG_DQT_ARRAY_SIZE];

  /* DQT calculation data for y component */

  uint8_t y_calc[JPEG_DQT_ARRAY_SIZE];

  /* DQT header setting for c component */

  uint8_t c_head[JPEG_DQT_ARRAY_SIZE];

  /* DQT calculation data for c component */

  uint8_t c_calc[JPEG_DQT_ARRAY_SIZE];
};

typedef struct isx019_fpga_jpg_quality_s isx019_fpga_jpg_quality_t;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/
/* Retry mechanism for I2C communication */
int isx019_i2c_read(FAR isx019_dev_t *priv,
                    uint8_t cat,
                    uint16_t addr,
                    FAR void *data,
                    uint8_t size);
int fpga_i2c_read(FAR isx019_dev_t *priv, uint8_t addr,
                  FAR void *data, uint8_t size);
int isx019_i2c_write(FAR isx019_dev_t *priv,
                      uint8_t cat,
                      uint16_t addr,
                      FAR const void *data,
                      uint8_t size);
int fpga_i2c_write(FAR isx019_dev_t *priv, uint8_t addr,
                   FAR const void *data, uint8_t size);

/* Split initialization functions for isx019_init() */
void isx019_fpga_init(FAR isx019_dev_t *priv);
void isx019_initialize_wbmode(FAR isx019_dev_t *priv);
int isx019_initialize_jpg_quality(FAR isx019_dev_t *priv);
void isx019_store_default_value(FAR isx019_dev_t *priv);

#endif /* __DRIVERS_VIDEO_ISX019_FUNC_H */
