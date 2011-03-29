/*
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

Int32 st_open(IN const Int8 * pathname, IN Int32 oflag);

Int32 st_write(IN Int32 fd, IN const void *buf, IN Int32 count);

Int32 st_read(IN Int32 fd, IN void *buf, IN Int32 count);

void st_perror(const char *s);

Int32 st_close(IN Int32 fd);
