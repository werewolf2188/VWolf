//
//  Core.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/30/24.
//

#pragma once

bool CheckMAErrors(const char* file, int code, int line);

#ifndef MAThrowIfFailed
#define MAThrowIfFailed
#define CHECKMAERROR(x) \
CheckMAErrors(__FILE__, x, __LINE__);
#endif
