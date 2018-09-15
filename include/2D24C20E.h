float P40_weight[18432] = {0};
float P40_bias[48] = {0};
float P42_weight[9216] = {0};
float P42_bias[192] = {0};
float P26_weight[8192] = {0};
float P26_bias[32] = {0};
float P28_weight[4096] = {0};
float P28_bias[128] = {0};
float P33_weight[12288] = {0};
float P33_bias[48] = {0};
float P35_weight[9216] = {0};
float P35_bias[192] = {0};
float P47_weight[24576] = {0};
float P47_bias[64] = {0};
float P49_weight[16384] = {0};
float P49_bias[256] = {0};
float P18_weight[4096] = {0};
float P18_bias[32] = {0};
float P20_weight[4096] = {0};
float P20_bias[128] = {0};
float P55_weight[32768] = {0};
float P55_bias[64] = {0};
float P57_weight[16384] = {0};
float P57_bias[256] = {0};
float P11_weight[2048] = {0};
float P11_bias[16] = {0};
float P13_weight[1024] = {0};
float P13_bias[64] = {0};
float P63_weight[512000] = {0};
float P63_bias[1000] = {0};
float P4_weight[1536] = {0};
float P4_bias[16] = {0};
float P6_weight[1024] = {0};
float P6_bias[64] = {0};
float bias_1[96] = {};
float kernelData_1[14112] = {0};
float bias_2[64] = {};
float kernelData_2[9216] = {0};
float bias_3[64] = {};
float kernelData_3[9216] = {0};
float bias_4[128] = {};
float kernelData_4[36864] = {0};
float bias_5[128] = {};
float kernelData_5[36864] = {0};
float bias_6[192] = {};
float kernelData_6[82944] = {0};
float bias_7[192] = {};
float kernelData_7[82944] = {0};
float bias_8[256] = {};
float kernelData_8[147456] = {0};
float bias_9[256] = {};
float kernelData_9[147456] = {0};

struct param
{
	float* param_ptr;
	int param_size;
};

struct param P2D24C20E[52] = {(struct param){.param_ptr = P11_bias, .param_size = 16}, (struct param){.param_ptr = P11_weight, .param_size = 2048}, (struct param){.param_ptr = P13_bias, .param_size = 64}, (struct param){.param_ptr = P13_weight, .param_size = 1024}, (struct param){.param_ptr = P18_bias, .param_size = 32}, (struct param){.param_ptr = P18_weight, .param_size = 4096}, (struct param){.param_ptr = P20_bias, .param_size = 128}, (struct param){.param_ptr = P20_weight, .param_size = 4096}, (struct param){.param_ptr = P26_bias, .param_size = 32}, (struct param){.param_ptr = P26_weight, .param_size = 8192}, (struct param){.param_ptr = P28_bias, .param_size = 128}, (struct param){.param_ptr = P28_weight, .param_size = 4096}, (struct param){.param_ptr = P33_bias, .param_size = 48}, (struct param){.param_ptr = P33_weight, .param_size = 12288}, (struct param){.param_ptr = P35_bias, .param_size = 192}, (struct param){.param_ptr = P35_weight, .param_size = 9216}, (struct param){.param_ptr = P40_bias, .param_size = 48}, (struct param){.param_ptr = P40_weight, .param_size = 18432}, (struct param){.param_ptr = P42_bias, .param_size = 192}, (struct param){.param_ptr = P42_weight, .param_size = 9216}, (struct param){.param_ptr = P47_bias, .param_size = 64}, (struct param){.param_ptr = P47_weight, .param_size = 24576}, (struct param){.param_ptr = P49_bias, .param_size = 256}, (struct param){.param_ptr = P49_weight, .param_size = 16384}, (struct param){.param_ptr = P4_bias, .param_size = 16}, (struct param){.param_ptr = P4_weight, .param_size = 1536}, (struct param){.param_ptr = P55_bias, .param_size = 64}, (struct param){.param_ptr = P55_weight, .param_size = 32768}, (struct param){.param_ptr = P57_bias, .param_size = 256}, (struct param){.param_ptr = P57_weight, .param_size = 16384}, (struct param){.param_ptr = P63_bias, .param_size = 1000}, (struct param){.param_ptr = P63_weight, .param_size = 512000}, (struct param){.param_ptr = P6_bias, .param_size = 64}, (struct param){.param_ptr = P6_weight, .param_size = 1024}, (struct param){.param_ptr = bias_1, .param_size = 96}, (struct param){.param_ptr = bias_2, .param_size = 64}, (struct param){.param_ptr = bias_3, .param_size = 64}, (struct param){.param_ptr = bias_4, .param_size = 128}, (struct param){.param_ptr = bias_5, .param_size = 128}, (struct param){.param_ptr = bias_6, .param_size = 192}, (struct param){.param_ptr = bias_7, .param_size = 192}, (struct param){.param_ptr = bias_8, .param_size = 256}, (struct param){.param_ptr = bias_9, .param_size = 256}, (struct param){.param_ptr = kernelData_1, .param_size = 14112}, (struct param){.param_ptr = kernelData_2, .param_size = 9216}, (struct param){.param_ptr = kernelData_3, .param_size = 9216}, (struct param){.param_ptr = kernelData_4, .param_size = 36864}, (struct param){.param_ptr = kernelData_5, .param_size = 36864}, (struct param){.param_ptr = kernelData_6, .param_size = 82944}, (struct param){.param_ptr = kernelData_7, .param_size = 82944}, (struct param){.param_ptr = kernelData_8, .param_size = 147456}, (struct param){.param_ptr = kernelData_9, .param_size = 147456}};