/****************************************************************************
 * fs/fs_initialize.c
 *
 * SPDX-License-Identifier: Apache-2.0
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/reboot_notifier.h>
#include <nuttx/trace.h>

#include "rpmsgfs/rpmsgfs.h"
#include "inode/inode.h"
#include "aio/aio.h"
#include "vfs/vfs.h"
#include "fs_heap.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: sync_reboot_handler
 ****************************************************************************/

static int sync_reboot_handler(FAR struct notifier_block *nb,
                               unsigned long action, FAR void *data)
{
  if (action == SYS_POWER_OFF || action == SYS_RESTART)
    {
      sync();
    }

  return 0;
}

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Name: fs_sync_reboot_nb
 ****************************************************************************/

static struct notifier_block g_sync_nb =
{
  sync_reboot_handler
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: fs_initialize
 *
 * Description:
 *   This is called from the OS initialization logic to configure the file
 *   system.
 *
 ****************************************************************************/

void fs_initialize(void)
{
  fs_trace_begin();

  fs_heap_initialize();

  /* Initial inode, file, and VFS data structures */

  inode_initialize();

  file_initlk();

#ifdef CONFIG_FS_AIO
  /* Initialize for asynchronous I/O */

  aio_initialize();

#endif

#ifdef CONFIG_FS_RPMSGFS_SERVER
  rpmsgfs_server_init();
#endif

#ifdef CONFIG_FS_NOTIFY
  notify_initialize();
#endif

  register_reboot_notifier(&g_sync_nb);
  fs_trace_end();
}
