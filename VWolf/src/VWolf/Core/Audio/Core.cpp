//
//  Core.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/30/24.
//

#include "vwpch.h"
#include "miniaudio.h"
#include "Core.h"

bool CheckMAErrors(const char* file, int code, int line) {
    ma_result result = (ma_result)code;
    if (result != MA_SUCCESS) {
        VWOLF_CORE_ERROR("Miniaudio Error: File %s. Line: %d. Code: %s", file, line, code);
        return false;
    }
    return true;
}
