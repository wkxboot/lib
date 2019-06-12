#ifndef _FYMODEM_H_
#define _FYMODEM_H_

/**
 * Free YModem implementation.
 *
 * Fredrik Hederstierna 2014
 *
 * This file is in the public domain.
 * You can do whatever you want with it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "serial.h"

/* max length of filename */
#define FYMODEM_FILE_NAME_MAX_LENGTH  (64)

/* receive file over ymodem */
int32_t fymodem_receive(serial_handle_t *handle,
                        uint32_t flash_addr,
                        uint32_t flash_limit,
                        char *filename,
                        uint32_t timeout);

/* send file over ymodem */
int32_t fymodem_send(serial_handle_t *handle,
                     uint8_t *txdata,
                     size_t txsize,
                     const char *filename);

#endif
