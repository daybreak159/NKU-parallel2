#include <iostream>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cstring>

// 避免编译器过度优化
volatile bool keep_result = true;

// 缓存局部性测试 - 大小可调整，根据系统缓存大小设定
constexpr int TEST_SIZE = 6000; // 可以调整为800, 1500, 3000等进行对比
constexpr int REPEAT_COUNT = 3; // 执行次数，可增加以获得更稳定结果

// 全局数组以避免栈溢出
int g_vector[TEST_SIZE + 1];
int g_matrix[TEST_SIZE + 1][TEST_SIZE + 1];
int g_sum[TEST_SIZE + 1];

// 测试函数: 按列访问
void columnwise_access() {
    // 初始化数据
    for(int i = 1; i <= TEST_SIZE; i++) {
        g_vector[i] = i;
        for(int j = 1; j <= TEST_SIZE; j++) {
            g_matrix[i][j] = i + j;
        }
    }
    
    std::memset(g_sum, 0, sizeof(g_sum));
    
    // 计时点 - VTune标记区域开始
    for(int r = 0; r < REPEAT_COUNT; r++) {
        // 主要计算部分 - 按列访问
        for(int i = 1; i <= TEST_SIZE; i++) {
            for(int j = 1; j <= TEST_SIZE; j++) {
                g_sum[i] += g_vector[j] * g_matrix[j][i]; // 按列访问
            }
        }
    }
    // 计时点 - VTune标记区域结束
    
    // 防止编译器优化掉计算
    if(keep_result) {
        std::cout << "Column-wise result checksum: " << g_sum[TEST_SIZE/2] << std::endl;
    }
}

// 测试函数: 按行访问
void rowwise_access() {
    // 初始化数据 - 与按列访问相同的初始值
    for(int i = 1; i <= TEST_SIZE; i++) {
        g_vector[i] = i;
        for(int j = 1; j <= TEST_SIZE; j++) {
            g_matrix[i][j] = i + j;
        }
    }
    
    std::memset(g_sum, 0, sizeof(g_sum));
    
    // 计时点 - VTune标记区域开始
    for(int r = 0; r < REPEAT_COUNT; r++) {
        // 主要计算部分 - 按行访问
        for(int i = 1; i <= TEST_SIZE; i++) {
            for(int j = 1; j <= TEST_SIZE; j++) {
                g_sum[j] += g_vector[i] * g_matrix[i][j]; // 按行访问
            }
        }
    }
    // 计时点 - VTune标记区域结束
    
    // 防止编译器优化掉计算
    if(keep_result) {
        std::cout << "Row-wise result checksum: " << g_sum[TEST_SIZE/2] << std::endl;
    }
}

// 顺序累加测试
constexpr int SUM_SIZE = 500000000; // 累加测试数据量

// 普通顺序累加
void normal_sum() {
    uint64_t sum = 0;
    
    // 计时点 - VTune标记区域开始
    for(int r = 0; r < REPEAT_COUNT * 10; r++) { // 更多重复次数以获得足够的采样
        sum = 0;
        for(int i = 0; i < SUM_SIZE; i++) {
            sum += i;
        }
    }
    // 计时点 - VTune标记区域结束
    
    if(keep_result) {
        std::cout << "Normal sum result: " << sum << std::endl;
    }
}

// 四路并行累加
void four_way_sum() {
    uint64_t sum = 0;
    
    // 计时点 - VTune标记区域开始
    for(int r = 0; r < REPEAT_COUNT * 10; r++) { // 更多重复次数以获得足够的采样
        uint64_t sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
        
        for(int i = 0; i < SUM_SIZE; i += 4) {
            sum1 += i;
            sum2 += (i + 1);
            sum3 += (i + 2);
            sum4 += (i + 3);
        }
        
        sum = sum1 + sum2 + sum3 + sum4;
    }
    // 计时点 - VTune标记区域结束
    
    if(keep_result) {
        std::cout << "Four-way sum result: " << sum << std::endl;
    }
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [test_type]\n"
                  << "  1: Column-wise matrix access\n"
                  << "  2: Row-wise matrix access\n"
                  << "  3: Normal sum\n"
                  << "  4: Four-way sum\n";
        return 1;
    }
    
    int test_type = std::atoi(argv[1]);
    
    std::cout << "Running test type " << test_type << std::endl;
    std::cout << "Matrix/Array size: " << TEST_SIZE << std::endl;
    std::cout << "Repeat count: " << REPEAT_COUNT << std::endl;
    
    switch(test_type) {
        case 1:
            columnwise_access();
            break;
        case 2:
            rowwise_access();
            break;
        case 3:
            normal_sum();
            break;
        case 4:
            four_way_sum();
            break;
        default:
            std::cout << "Invalid test type\n";
            return 1;
    }
    
    return 0;
}
