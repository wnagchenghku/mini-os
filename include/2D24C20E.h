float P52_runningMean[512] = {0};
float P52_runningVar[512] = {0};
float P52_weight[512] = {0};
float P52_bias[512] = {0};
float P32_runningMean[128] = {0};
float P32_runningVar[128] = {0};
float P32_weight[128] = {0};
float P32_bias[128] = {0};
float P40_weight[32768] = {0};
float P41_runningMean[256] = {0};
float P41_runningVar[256] = {0};
float P41_weight[256] = {0};
float P41_bias[256] = {0};
float P55_runningMean[512] = {0};
float P55_runningVar[512] = {0};
float P55_weight[512] = {0};
float P55_bias[512] = {0};
float P6_runningMean[64] = {0};
float P6_runningVar[64] = {0};
float P6_weight[64] = {0};
float P6_bias[64] = {0};
float P39_runningMean[256] = {0};
float P39_runningVar[256] = {0};
float P39_weight[256] = {0};
float P39_bias[256] = {0};
float P16_runningMean[64] = {0};
float P16_runningVar[64] = {0};
float P16_weight[64] = {0};
float P16_bias[64] = {0};
float P24_weight[8192] = {0};
float P25_runningMean[128] = {0};
float P25_runningVar[128] = {0};
float P25_weight[128] = {0};
float P25_bias[128] = {0};
float P48_runningMean[256] = {0};
float P48_runningVar[256] = {0};
float P48_weight[256] = {0};
float P48_bias[256] = {0};
float P56_weight[131072] = {0};
float P57_runningMean[512] = {0};
float P57_runningVar[512] = {0};
float P57_weight[512] = {0};
float P57_bias[512] = {0};
float P36_runningMean[256] = {0};
float P36_runningVar[256] = {0};
float P36_weight[256] = {0};
float P36_bias[256] = {0};
float P20_runningMean[128] = {0};
float P20_runningVar[128] = {0};
float P20_weight[128] = {0};
float P20_bias[128] = {0};
float P9_runningMean[64] = {0};
float P9_runningVar[64] = {0};
float P9_weight[64] = {0};
float P9_bias[64] = {0};
float P2_runningMean[64] = {0};
float P2_runningVar[64] = {0};
float P2_weight[64] = {0};
float P2_bias[64] = {0};
float P64_runningMean[512] = {0};
float P64_runningVar[512] = {0};
float P64_weight[512] = {0};
float P64_bias[512] = {0};
float P23_runningMean[128] = {0};
float P23_runningVar[128] = {0};
float P23_weight[128] = {0};
float P23_bias[128] = {0};
float bias_1[64] = {0};
float kernelData_1[9408] = {0};
float bias_2[64] = {0};
float kernelData_2[36864] = {0};
float bias_3[64] = {0};
float kernelData_3[36864] = {0};
float bias_4[64] = {0};
float kernelData_4[36864] = {0};
float bias_5[64] = {0};
float kernelData_5[36864] = {0};
float bias_6[128] = {0};
float kernelData_6[73728] = {0};
float bias_7[128] = {0};
float kernelData_7[147456] = {0};
float bias_8[128] = {0};
float kernelData_8[147456] = {0};
float bias_9[128] = {0};
float kernelData_9[147456] = {0};
float bias_10[256] = {0};
float kernelData_10[294912] = {0};
float bias_11[256] = {0};
float kernelData_11[589824] = {0};
float bias_12[256] = {0};
float kernelData_12[589824] = {0};
float bias_13[256] = {0};
float kernelData_13[589824] = {0};
float bias_14[512] = {0};
float kernelData_14[1179648] = {0};
float bias_15[512] = {0};
float kernelData_15[2359296] = {0};
float bias_16[512] = {0};
float kernelData_16[2359296] = {0};
float bias_17[512] = {0};
float kernelData_17[2359296] = {0};
float kernelData_18[512000] = {0};
float bias_18[1000] = {0};

float *P2D24C20E[103] = {P52_runningMean, P52_runningVar, P52_weight, P52_bias, P32_runningMean, P32_runningVar, P32_weight, P32_bias, P40_weight, P41_runningMean, P41_runningVar, P41_weight, P41_bias, P55_runningMean, P55_runningVar, P55_weight, P55_bias, P6_runningMean, P6_runningVar, P6_weight, P6_bias, P39_runningMean, P39_runningVar, P39_weight, P39_bias, P16_runningMean, P16_runningVar, P16_weight, P16_bias, P24_weight, P25_runningMean, P25_runningVar, P25_weight, P25_bias, P48_runningMean, P48_runningVar, P48_weight, P48_bias, P56_weight, P57_runningMean, P57_runningVar, P57_weight, P57_bias, P36_runningMean, P36_runningVar, P36_weight, P36_bias, P20_runningMean, P20_runningVar, P20_weight, P20_bias, P9_runningMean, P9_runningVar, P9_weight, P9_bias, P2_runningMean, P2_runningVar, P2_weight, P2_bias, P64_runningMean, P64_runningVar, P64_weight, P64_bias, P23_runningMean, P23_runningVar, P23_weight, P23_bias, bias_1, kernelData_1, bias_2, kernelData_2, bias_3, kernelData_3, bias_4, kernelData_4, bias_5, kernelData_5, bias_6, kernelData_6, bias_7, kernelData_7, bias_8, kernelData_8, bias_9, kernelData_9, bias_10, kernelData_10, bias_11, kernelData_11, bias_12, kernelData_12, bias_13, kernelData_13, bias_14, kernelData_14, bias_15, kernelData_15, bias_16, kernelData_16, bias_17, kernelData_17, kernelData_18, bias_18};