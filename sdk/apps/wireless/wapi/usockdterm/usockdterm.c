/****************************************************************************
 * apps/wireless/wapi/usockdterm/usockdterm.c

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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>

#include "wireless/wapi.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define USOCK_IFNAME "wlan0"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int fd;
  int ret;

  fd = wapi_make_socket();
  if (fd < 0)
    {
      printf("wapi socket creation failed...\n");
      return 1;
    }

  ret = wapi_term_usock_daemon(fd, USOCK_IFNAME);
  if (ret < 0)
    {
      printf("wapi terminate usock daemon faield...\n");
      close(fd);
      return 1;
    }

  close(fd);
  return 0;
}
