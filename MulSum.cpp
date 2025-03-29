#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <windows.h>

using namespace std;

void Sum(int n, uint64_t& sum);      // 普通链式累加
void MulSum(int n, uint64_t& sum);   // 四路链式累加
ofstream outputFile("sum_results.csv"); // 创建CSV文件

int main() 
{
    SetConsoleOutputCP(65001);  // 设置控制台输出为UTF-8
    
    // 创建CSV表头
    outputFile << "n,普通累加(微秒),四路累加(微秒),加速比" << endl;
    
    // 对不同规模进行测试
    for(int n=1000; n<=100000; n+=1000) 
    {
        uint64_t sum1 = 0, sum2 = 0;
        double time1 = 0, time2 = 0;
        
        // 普通累加测试
        auto start = std::chrono::high_resolution_clock::now();
        for(int i=0; i<100; i++) {  // 重复100次以获得更准确的时间
            sum1 = 0;
            Sum(n, sum1);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duration1 = end - start;
        time1 = duration1.count() / 100;  // 计算平均执行时间
        
        // 四路链式累加测试
        start = std::chrono::high_resolution_clock::now();
        for(int i=0; i<100; i++) {  // 重复100次以获得更准确的时间
            sum2 = 0;
            MulSum(n, sum2);
        }
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duration2 = end - start;
        time2 = duration2.count() / 100;  // 计算平均执行时间
        
        // 输出结果
        double speedup = time1 / time2;  // 计算加速比
        cout << "n=" << n << ", 普通累加: " << fixed << time1 << "μs, "
             << "四路累加: " << fixed << time2 << "μs, "
             << "加速比: " << fixed << speedup << endl;
        
        // 验证结果正确性
        if (sum1 != sum2) {
            cout << "警告：结果不一致！sum1=" << sum1 << ", sum2=" << sum2 << endl;
        }
        
        // 写入CSV
        outputFile << n << "," << fixed << time1 << "," << fixed << time2 << "," << fixed << speedup << endl;
    }
    
    outputFile.close();
    return 0;
}

// 普通链式累加 - 根据Algorithm 3实现
void Sum(int n, uint64_t& sum) 
{
    for(int i = 0; i < n; i++) {
        sum += i;
    }
}

// 四路链式累加 - 根据Algorithm 4实现
void MulSum(int n, uint64_t& sum) 
{
    uint64_t sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    
    for(int i = 0; i < n; i += 4) {
        sum1 += i;
        sum2 += (i + 1);
        sum3 += (i + 2);
        sum4 += (i + 3);
    }
    
    sum = sum1 + sum2 + sum3 + sum4;
}