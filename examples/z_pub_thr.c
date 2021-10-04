/*
 * Copyright (c) 2017, 2020 ADLINK Technology Inc.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
 * which is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *
 * Contributors:
 *   ADLINK zenoh team, <zenoh@adlink-labs.tech>
 */
#include <stdio.h>
#include <string.h>
#include "zenoh.h"

int main(int argc, char **argv)
{
  z_init_logger();

  if (argc < 2)
  {
    printf("USAGE:\n\tzn_pub_thr <payload-size> [<zenoh-locator>]\n\n");
    exit(-1);
  }
  size_t len = atoi(argv[1]);
  printf("Running throughput test for payload of %zu bytes.\n", len);
  z_owned_config_t config = z_config__default();
  if (argc > 2)
  {
    z_config__insert(config.borrow, ZN_CONFIG_PEER_KEY, z_string__new(argv[2]));
  }

  z_owned_session_t s = z_open(config);
  if (s.borrow == 0)
  {
    printf("Unable to open session!\n");
    exit(-1);
  }

  char *data = (char *)malloc(len);
  memset(data, 1, len);

  z_owned_reskey_t reskey = z_rid(z_register_resource(s.borrow, z_rname("/test/thr")));
  z_owned_publisher_t pub = z_register_publisher(s.borrow, &reskey);
  if (pub.borrow == 0)
  {
    printf("Unable to declare publisher.\n");
    exit(-1);
  }

  while (1)
  {
    z_write(s.borrow, &reskey, (const uint8_t *)data, len);
  }
  z_reskey__free(reskey);
  z_unregister_publisher(pub);
  z_close(s);
}