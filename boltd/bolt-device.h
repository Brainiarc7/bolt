/*
 * Copyright © 2017 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Christian J. Kellner <christian@kellner.me>
 */

#pragma once

#include "bolt-gdbus.h"
#include "bolt-manager.h"

/* forward declaration */
struct udev_device;

G_BEGIN_DECLS

#define BOLT_TYPE_DEVICE bolt_device_get_type ()
G_DECLARE_FINAL_TYPE (BoltDevice, bolt_device, BOLT, DEVICE, BoltDBusDeviceSkeleton);

BoltDevice *      bolt_device_new_for_udev (BoltManager        *manager,
                                            struct udev_device *udev,
                                            GError            **error);


gboolean          bolt_device_export (BoltDevice      *device,
                                      GDBusConnection *connection,
                                      GError         **error);

void              bolt_device_unexport (BoltDevice *device);

const char *      bolt_device_get_object_path (BoltDevice *device);

G_END_DECLS