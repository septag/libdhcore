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
#include "dhcore/std-math.h"
#include "dhcore/timer.h"
#include "dhcore/numeric.h"

using namespace dh;

void test_hashtable()
{
    const int item_cnt = 100000;
    ProfileTimer tm;
    HashtableFixed<int, -1> htable;

    int *keys = (int*)ALLOC(sizeof(int)*item_cnt, 0);
    ASSERT(keys);
    for (int i = 0; i < item_cnt; i++)
        keys[i] = rand_geti(0, 1000000);

    htable.create(item_cnt);

    printf("adding %d items to fixed hashtable ...\n", item_cnt);
    tm.begin();
    for (int i = 0; i < item_cnt; i++)
        htable.add(keys[i], i);
    printf("time: %f\n", tm.end());

    printf("searching %d items ...\n", item_cnt);
    tm.begin();
    for (int i = 0; i < item_cnt; i++)
        htable.value(keys[i]);
    printf("time: %f\n", tm.end());

    htable.destroy();
    FREE(keys);
}
