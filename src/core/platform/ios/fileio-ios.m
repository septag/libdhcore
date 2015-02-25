/***********************************************************************************
 * Copyright (c) 2015, Sepehr Taghdisian
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

#import <Foundation/Foundation.h>
#include "dhcore/types.h"
#include "dhcore/path.h"
#include "dhcore/str.h"

NSMutableArray *g_bundles = nil;

int fio_ios_add_bundle(const char *bundle_name)
{
    NSBundle *bundle = [NSBundle bundleWithPath:[[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:bundle_name] ofType:@"bundle"]];
    if (!bundle)
        return 0;
    
    if (!g_bundles) {
        g_bundles = [NSMutableArray arrayWithObject:bundle];
    }   else    {
        [g_bundles addObject:bundle];
    }
    
    return (int)g_bundles.count;
}

const char* fio_ios_resolve_path(char *outstr, int outstr_sz, int bundle_id, const char *filepath)
{
    if (g_bundles)  {
        char filename[DH_PATH_MAX];
        char fileext[DH_PATH_MAX];
        
        path_getfilename(filename, filepath);
        path_getfileext(fileext, filepath);
        
        NSUInteger index = (NSUInteger)(bundle_id-1);
        NSBundle *bundle = [g_bundles objectAtIndex:index];
        
        NSString *path = [bundle pathForResource:[NSString stringWithUTF8String:filename] ofType:[NSString stringWithUTF8String:fileext]];
        if (path)
            str_safecpy(outstr, outstr_sz, [path UTF8String]);
        else
            outstr[0] = 0;
        return outstr;
    }   else    {
        return nil;
    }
}

