/***********************************************************************************
 * Copyright (c) 2012, Sepehr Taghdisian
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 ***********************************************************************************/

#include "dhcore-test.h"
#include "dhcore/core.h"
#include "dhcore/hash-table.h"
#include "dhcore/color.h"

void test_hashtable()
{
    dhColor test(1.0f, 2.0f, 3.0f);
    const color* c = test;


    dhHashTableFixed<int, 666> htable;
    htable.create(10);
    htable.add(1, 1);
    int r = htable.value(23);
    htable.destroy();
}
