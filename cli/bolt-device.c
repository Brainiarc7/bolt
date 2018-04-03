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

#include "config.h"

#include "bolt-device.h"

#include "bolt-enums.h"
#include "bolt-error.h"
#include "bolt-names.h"

#include <gio/gio.h>

struct _BoltDevice
{
  BoltProxy parent;
};

enum {
  PROP_0,

  /* D-Bus Props */
  PROP_UID,
  PROP_NAME,
  PROP_VENDOR,
  PROP_TYPE,
  PROP_STATUS,
  PROP_PARENT,
  PROP_SYSPATH,
  PROP_CONNTIME,
  PROP_AUTHTIME,

  PROP_STORED,
  PROP_POLICY,
  PROP_KEY,
  PROP_STORETIME,
  PROP_LABEL,

  PROP_LAST
};

static GParamSpec *props[PROP_LAST] = {NULL, };

G_DEFINE_TYPE (BoltDevice,
               bolt_device,
               BOLT_TYPE_PROXY);

static void
bolt_device_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  if (bolt_proxy_get_dbus_property (object, pspec, value))
    return;
}



static void
bolt_device_class_init (BoltDeviceClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = bolt_device_get_property;

  props[PROP_UID] =
    g_param_spec_string ("uid",
                         "Uid", NULL,
                         "unknown",
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_NICK);

  props[PROP_NAME] =
    g_param_spec_string ("name",
                         "Name", NULL,
                         "unknown",
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_NICK);

  props[PROP_VENDOR] =
    g_param_spec_string ("vendor",
                         "Vendor", NULL,
                         "unknown",
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_NICK);

  props[PROP_TYPE] =
    g_param_spec_enum ("type",
                       "Type", NULL,
                       BOLT_TYPE_DEVICE_TYPE,
                       BOLT_DEVICE_PERIPHERAL,
                       G_PARAM_READABLE |
                       G_PARAM_STATIC_NICK);

  props[PROP_STATUS] =
    g_param_spec_enum ("status",
                       "Status", NULL,
                       BOLT_TYPE_STATUS,
                       BOLT_STATUS_DISCONNECTED,
                       G_PARAM_READABLE |
                       G_PARAM_STATIC_NICK);

  props[PROP_PARENT] =
    g_param_spec_string ("parent",
                         "Parent", NULL,
                         "unknown",
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_NICK);

  props[PROP_SYSPATH] =
    g_param_spec_string ("syspath",
                         "SysfsPath", NULL,
                         "unknown",
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_NICK);

  props[PROP_CONNTIME] =
    g_param_spec_uint64 ("conntime",
                         "ConnectTime", NULL,
                         0, G_MAXUINT64, 0,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_STRINGS);

  props[PROP_AUTHTIME] =
    g_param_spec_uint64 ("authtime",
                         "AuthorizeTime", NULL,
                         0, G_MAXUINT64, 0,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_STRINGS);

  props[PROP_STORED] =
    g_param_spec_boolean ("stored",
                          "Stored", NULL,
                          FALSE,
                          G_PARAM_READABLE |
                          G_PARAM_STATIC_NICK);

  props[PROP_POLICY] =
    g_param_spec_enum ("policy",
                       "Policy", NULL,
                       BOLT_TYPE_POLICY,
                       BOLT_POLICY_DEFAULT,
                       G_PARAM_READABLE |
                       G_PARAM_STATIC_NICK);

  props[PROP_KEY] =
    g_param_spec_enum ("key",
                       "Key", NULL,
                       BOLT_TYPE_KEY_STATE,
                       BOLT_KEY_MISSING,
                       G_PARAM_READABLE |
                       G_PARAM_STATIC_NICK);

  props[PROP_STORETIME] =
    g_param_spec_uint64 ("storetime",
                         "StoreTime", NULL,
                         0, G_MAXUINT64, 0,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_STRINGS);

  props[PROP_LABEL] =
    g_param_spec_string ("label",
                         "Label", NULL,
                         NULL,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class,
                                     PROP_LAST,
                                     props);

}

static void
bolt_device_init (BoltDevice *mgr)
{
}

/* public methods */

BoltDevice *
bolt_device_new_for_object_path (GDBusConnection *bus,
                                 const char      *path,
                                 GCancellable    *cancel,
                                 GError         **error)
{
  BoltDevice *dev;

  dev = g_initable_new (BOLT_TYPE_DEVICE,
                        cancel, error,
                        "g-flags", G_DBUS_PROXY_FLAGS_NONE,
                        "g-connection", bus,
                        "g-name", BOLT_DBUS_NAME,
                        "g-object-path", path,
                        "g-interface-name", BOLT_DBUS_DEVICE_INTERFACE,
                        NULL);

  return dev;
}

gboolean
bolt_device_authorize (BoltDevice   *dev,
                       BoltAuthFlags flags,
                       GCancellable *cancel,
                       GError      **error)
{
  g_autoptr(GError) err = NULL;
  g_autofree char *fstr = NULL;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), FALSE);

  fstr = bolt_flags_to_string (BOLT_TYPE_AUTH_FLAGS, flags, error);
  if (fstr == NULL)
    return FALSE;

  g_dbus_proxy_call_sync (G_DBUS_PROXY (dev),
                          "Authorize",
                          g_variant_new ("(s)", fstr),
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          cancel,
                          &err);

  if (err != NULL)
    {
      if (g_dbus_error_is_remote_error (err))
        g_dbus_error_strip_remote_error (err);

      g_propagate_error (error, g_steal_pointer (&err));
      return FALSE;
    }

  return TRUE;
}

const char *
bolt_device_get_uid (BoltDevice *dev)
{
  const char *key;
  const char *str;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), NULL);

  key = g_param_spec_get_name (props[PROP_UID]);
  str = bolt_proxy_get_property_string (BOLT_PROXY (dev), key);

  return str;
}

const char *
bolt_device_get_name (BoltDevice *dev)
{
  const char *key;
  const char *str;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), NULL);

  key = g_param_spec_get_name (props[PROP_NAME]);
  str = bolt_proxy_get_property_string (BOLT_PROXY (dev), key);

  return str;
}

const char *
bolt_device_get_vendor (BoltDevice *dev)
{
  const char *key;
  const char *str;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), NULL);

  key = g_param_spec_get_name (props[PROP_VENDOR]);
  str = bolt_proxy_get_property_string (BOLT_PROXY (dev), key);

  return str;
}

BoltDeviceType
bolt_device_get_device_type (BoltDevice *dev)
{
  const char *key;
  gboolean ok;
  gint val = BOLT_DEVICE_PERIPHERAL;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_TYPE]);
  ok = bolt_proxy_get_property_enum (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

BoltStatus
bolt_device_get_status (BoltDevice *dev)
{
  const char *key;
  gboolean ok;
  gint val = BOLT_STATUS_UNKNOWN;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_STATUS]);
  ok = bolt_proxy_get_property_enum (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

const char *
bolt_device_get_parent (BoltDevice *dev)
{
  const char *key;
  const char *str;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), NULL);

  key = g_param_spec_get_name (props[PROP_PARENT]);
  str = bolt_proxy_get_property_string (BOLT_PROXY (dev), key);

  return str;
}

const char *
bolt_device_get_syspath (BoltDevice *dev)
{
  const char *key;
  const char *str;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), NULL);

  key = g_param_spec_get_name (props[PROP_SYSPATH]);
  str = bolt_proxy_get_property_string (BOLT_PROXY (dev), key);

  return str;
}

guint64
bolt_device_get_conntime (BoltDevice *dev)
{
  const char *key;
  guint64 val = 0;
  gboolean ok;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_CONNTIME]);
  ok = bolt_proxy_get_property_uint64 (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

guint64
bolt_device_get_authtime (BoltDevice *dev)
{
  const char *key;
  guint64 val = 0;
  gboolean ok;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_AUTHTIME]);
  ok = bolt_proxy_get_property_uint64 (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

gboolean
bolt_device_is_stored (BoltDevice *dev)
{
  const char *key;
  gboolean val = FALSE;
  gboolean ok;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_STORED]);
  ok = bolt_proxy_get_property_bool (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

BoltPolicy
bolt_device_get_policy (BoltDevice *dev)
{
  const char *key;
  gboolean ok;
  gint val = BOLT_POLICY_DEFAULT;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_POLICY]);
  ok = bolt_proxy_get_property_enum (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

BoltKeyState
bolt_device_get_keystate (BoltDevice *dev)
{
  const char *key;
  gboolean ok;
  gint val = BOLT_KEY_MISSING;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_KEY]);
  ok = bolt_proxy_get_property_enum (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

guint64
bolt_device_get_storetime (BoltDevice *dev)
{
  const char *key;
  guint64 val = 0;
  gboolean ok;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), val);

  key = g_param_spec_get_name (props[PROP_STORETIME]);
  ok = bolt_proxy_get_property_uint64 (BOLT_PROXY (dev), key, &val);

  if (!ok)
    g_warning ("failed to get enum property '%s'", key);

  return val;
}

const char *
bolt_device_get_label (BoltDevice *dev)
{
  const char *key;
  const char *str;

  g_return_val_if_fail (BOLT_IS_DEVICE (dev), NULL);

  key = g_param_spec_get_name (props[PROP_LABEL]);
  str = bolt_proxy_get_property_string (BOLT_PROXY (dev), key);

  return str;
}
