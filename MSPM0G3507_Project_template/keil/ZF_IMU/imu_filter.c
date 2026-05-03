#include "imu_filter.h"                  // Device header
#include "hal_delay.h"
#include <math.h>
#include "zf_device_imu660ra.h"
#include "hal_jy62.h"
imu660_data imu = {0};

int16_t AX, AY, AZ, GX, GY, GZ;
int16_t acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;

#define sampleFreq	100.0f		// sample frequency in Hz
#define betaDef		0.1f		// 2 * proportional gain

//---------------------------------------------------------------------------------------------------
// Variable definitions

volatile float beta = betaDef;								// 2 * proportional gain (Kp)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	// quaternion of sensor frame relative to auxiliary frame

gyro_param_t GyroOffset = {0};

void gyroOffsetInit(void)
{
    hal_delay_ms(1000); // 最大 1Khz

  GyroOffset.Xdata = 0;
  GyroOffset.Ydata = 0;
  GyroOffset.Zdata = 0;
  for (uint16_t i = 0; i < 100; ++i)
  {
    imu660ra_get_gyro(&gyro_data);	
    GyroOffset.Xdata += gyro_data.x;
    GyroOffset.Ydata += gyro_data.y;
    GyroOffset.Zdata += gyro_data.z;
    hal_delay_ms(10); // 最大 1Khz
  }

  GyroOffset.Xdata /= 100;
  GyroOffset.Ydata /= 100;
  GyroOffset.Zdata /= 100;
}

/*
 * 函数名：get_iir_factor
 * 描述  ：求取IIR滤波器的滤波因子
 * 输入  ：out_factor滤波因子首地址，Time任务执行周期，Cut_Off滤波截止频率
 * 返回  ：
 */
void get_iir_factor(float *out_factor, float Time, float Cut_Off)
{
  *out_factor = Time / (Time + 1 / (2.0f * PI * Cut_Off));
}
/**
 * @brief   IIR低通滤波器
 * @param   *acc_in 输入三轴数据指针变量
 * @param   *acc_out 输出三轴数据指针变量
 * @param   lpf_factor 滤波因数
 * @retval  x
 */
float iir_lpf(float in, float out, float lpf_factor)
{
	out = out + lpf_factor * (in - out);
	return out;
}

/* 获取IIR低通滤波
 *
 *
 *
 *
 * */
void IIR_imu(void)
{
	get_iir_factor(&imu.att_acc_factor, 0.001, 25);
	get_iir_factor(&imu.att_gryo_factor, 0.001, 30);

}

void IMU(void)
{
	/*获取X、Y的角速度和加速度*/
//	MPU6050_ReadData(&AX, &AY, &AZ, &GX, &GY, &GZ);

	/*滤波算法*/
	acc_x = iir_lpf(AX, acc_x, imu.att_acc_factor);
	acc_y = iir_lpf(AY, acc_y, imu.att_acc_factor);
	acc_z = iir_lpf(AZ, acc_z, imu.att_acc_factor);
	gyro_x = iir_lpf(GX - GyroOffset.Xdata, gyro_x, imu.att_gryo_factor);
	gyro_y = iir_lpf(GY - GyroOffset.Ydata, gyro_y, imu.att_gryo_factor);
	gyro_z = iir_lpf(GZ - GyroOffset.Zdata, gyro_z, imu.att_gryo_factor);
	/*数据存储*/
	imu.acc_g.x = acc_x / 2049.0f;
	imu.acc_g.y = acc_y / 2049.0f;
	imu.acc_g.z = acc_z / 2049.0f;
	imu.deg_s.x = gyro_x / 28.6f*4;
	imu.deg_s.y = gyro_y / 28.6f*4;
	imu.deg_s.z = gyro_z * 0.030517578125f*4;
	/*姿态解算*/
//	MadgwickAHRSupdateIMU(imu.deg_s.x, imu.deg_s.y, imu.deg_s.z,
//						imu.acc_g.x, imu.acc_g.y, imu.acc_g.z);
}

typedef struct
{
  float DCM[3][3];   // 机体坐标系 -> 地理坐标系
  float DCM_T[3][3]; // 地理坐标系 -> 机体坐标系
} _Matrix;

_Matrix Mat = {0};

#define MahonyPERIOD 5.0f     // 姿态解算周期（ms）
#define kp 0.5f               // proportional gain governs rate of convergence to accelerometer/magnetometer
#define ki 0.0001f            // integral gain governs rate of convergenceof gyroscope biases

//static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;  // quaternion elements representing theestimated orientation
static float exInt = 0, eyInt = 0, ezInt = 0; // scaled integral error

// Fast inverse square-root
static float invSqrt(float x)
{
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long *)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}

/*
 * 函数名：mahony_update
 * 描述  ：姿态解算
 * 输入  ：陀螺仪三轴数据（单位：弧度/秒），加速度三轴数据（单位：g）
 * 返回  ：
 */
// Gyroscope units are radians/second, accelerometer  units are irrelevant as the vector is normalised.
void mahony_update(float gx, float gy, float gz, float ax, float ay, float az)
{
  float norm;
  float vx, vy, vz;
  float ex, ey, ez;

  if (ax * ay * az == 0)
    return;
  gx = gx * (PI / 180.0f);
  gy = gy * (PI / 180.0f);
  gz = gz * (PI / 180.0f);
  //[ax,ay,az]是机体坐标系下加速度计测得的重力向量(竖直向下)
  norm = invSqrt(ax * ax + ay * ay + az * az);
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;

  // VectorA = MatrixC * VectorB
  // VectorA ：参考重力向量转到在机体下的值
  // MatrixC ：地理坐标系转机体坐标系的旋转矩阵
  // VectorB ：参考重力向量（0,0,1）
  //[vx,vy,vz]是地理坐标系重力分向量[0,0,1]经过DCM旋转矩阵(C(n->b))计算得到的机体坐标系中的重力向量(竖直向下)

  vx = Mat.DCM_T[0][2];
  vy = Mat.DCM_T[1][2];
  vz = Mat.DCM_T[2][2];

  // 机体坐标系下向量叉乘得到误差向量，误差e就是测量得到的vˉ和预测得到的 v^之间的相对旋转。这里的vˉ就是[ax,ay,az]’,v^就是[vx,vy,vz]’
  // 利用这个误差来修正DCM方向余弦矩阵(修正DCM矩阵中的四元素)，这个矩阵的作用就是将b系和n正确的转化直到重合。
  // 实际上这种修正方法只把b系和n系的XOY平面重合起来，对于z轴旋转的偏航，加速度计无可奈何，
  // 但是，由于加速度计无法感知z轴上的旋转运动，所以还需要用地磁计来进一步补偿。
  // 两个向量的叉积得到的结果是两个向量的模与他们之间夹角正弦的乘积a×v=|a||v|sinθ,
  // 加速度计测量得到的重力向量和预测得到的机体重力向量已经经过单位化，因而他们的模是1，
  // 也就是说它们向量的叉积结果仅与sinθ有关，当角度很小时，叉积结果可以近似于角度成正比。

  ex = ay * vz - az * vy;
  ey = az * vx - ax * vz;
  ez = ax * vy - ay * vx;

  // 对误差向量进行积分
  exInt = exInt + ex * ki;
  eyInt = eyInt + ey * ki;
  ezInt = ezInt + ez * ki;

  // 姿态误差补偿到角速度上，修正角速度积分漂移，通过调节Kp、Ki两个参数，可以控制加速度计修正陀螺仪积分姿态的速度。
  gx = gx + kp * ex + exInt;
  gy = gy + kp * ey + eyInt;
  gz = gz + kp * ez + ezInt;

  // 一阶龙格库塔法更新四元数
  q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * MahonyPERIOD * 0.0005f;
  q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * MahonyPERIOD * 0.0005f;
  q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * MahonyPERIOD * 0.0005f;
  q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * MahonyPERIOD * 0.0005f;

  // 把上述运算后的四元数进行归一化处理。得到了物体经过旋转后的新的四元数。
  norm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 = q0 * norm;
  q1 = q1 * norm;
  q2 = q2 * norm;
  q3 = q3 * norm;

  // 四元素转欧拉角
  imu.pitch = atan2(2.0f * (q0 * q1 + q2 * q3), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3) * (180.0f / PI);
  imu.roll = -asin(2.0f * (q0 * q2 - q1 * q3)) * (180.0f / PI);
  
  // z轴角速度积分的偏航角
  imu.yaw += imu.deg_s.z * MahonyPERIOD * 0.001f;
}
/*
 * 函数名：rotation_matrix
 * 描述  ：旋转矩阵：机体坐标系 -> 地理坐标系
 * 输入  ：
 * 返回  ：
 */
void rotation_matrix(void)
{
  Mat.DCM[0][0] = 1.0f - 2.0f * q2 * q2 - 2.0f * q3 * q3;
  Mat.DCM[0][1] = 2.0f * (q1 * q2 - q0 * q3);
  Mat.DCM[0][2] = 2.0f * (q1 * q3 + q0 * q2);

  Mat.DCM[1][0] = 2.0f * (q1 * q2 + q0 * q3);
  Mat.DCM[1][1] = 1.0f - 2.0f * q1 * q1 - 2.0f * q3 * q3;
  Mat.DCM[1][2] = 2.0f * (q2 * q3 - q0 * q1);

  Mat.DCM[2][0] = 2.0f * (q1 * q3 - q0 * q2);
  Mat.DCM[2][1] = 2.0f * (q2 * q3 + q0 * q1);
  Mat.DCM[2][2] = 1.0f - 2.0f * q1 * q1 - 2.0f * q2 * q2;
}
/*
 * 函数名：rotation_matrix_T
 * 描述  ：旋转矩阵的转置矩阵：地理坐标系 -> 机体坐标系
 * 输入  ：
 * 返回  ：
 */
void rotation_matrix_T(void)
{
  Mat.DCM_T[0][0] = 1.0f - 2.0f * q2 * q2 - 2.0f * q3 * q3;
  Mat.DCM_T[0][1] = 2.0f * (q1 * q2 + q0 * q3);
  Mat.DCM_T[0][2] = 2.0f * (q1 * q3 - q0 * q2);

  Mat.DCM_T[1][0] = 2.0f * (q1 * q2 - q0 * q3);
  Mat.DCM_T[1][1] = 1.0f - 2.0f * q1 * q1 - 2.0f * q3 * q3;
  Mat.DCM_T[1][2] = 2.0f * (q2 * q3 + q0 * q1);

  Mat.DCM_T[2][0] = 2.0f * (q1 * q3 + q0 * q2);
  Mat.DCM_T[2][1] = 2.0f * (q2 * q3 - q0 * q1);
  Mat.DCM_T[2][2] = 1.0f - 2.0f * q1 * q1 - 2.0f * q2 * q2;
}
/*
 * 函数名：Matrix_ready
 * 描述  ：矩阵更新准备，为姿态解算使用
 * 输入  ：
 * 返回  ：
 */
void Matrix_ready(void)
{
  rotation_matrix();   // 旋转矩阵更新
  rotation_matrix_T(); // 旋转矩阵的逆矩阵更新
}

void _IMU(void)
{
  /*获取X、Y的角速度和加速度*/
            imu660ra_get_acc(&acc_data);
           imu660ra_get_gyro(&gyro_data);	
  /*滤波算法*/
  acc_x = iir_lpf(acc_data.x, acc_x, imu.att_acc_factor);
  acc_y = iir_lpf(acc_data.y, acc_y, imu.att_acc_factor);
  acc_z = iir_lpf(acc_data.z, acc_z, imu.att_acc_factor);
  gyro_x = iir_lpf(gyro_data.x - GyroOffset.Xdata, gyro_x, imu.att_gryo_factor);
  gyro_y = iir_lpf(gyro_data.y - GyroOffset.Ydata, gyro_y, imu.att_gryo_factor);
  gyro_z = iir_lpf(gyro_data.z - GyroOffset.Zdata, gyro_z, imu.att_gryo_factor);
  /*数据存储*/
  imu.acc_g.x = acc_x / 4096.0f;
  imu.acc_g.y = acc_y / 4096.0f;
  imu.acc_g.z = acc_z / 4096.0f;
  imu.deg_s.x = gyro_x / 16.4f;
  imu.deg_s.y = gyro_y / 16.4f;
  imu.deg_s.z = gyro_z / 16.4f;
  /*姿态解算*/
  imu.yaw += imu.deg_s.z * MahonyPERIOD * 0.001f;
	if(imu.yaw > 180) 
	{
		imu.yaw = imu.yaw - 360;
	}
	else if(imu.yaw <= -180)
	{
		imu.yaw = imu.yaw + 360;
	}
	angle.z = imu.yaw ;
//  mahony_update(imu.deg_s.x, imu.deg_s.y, imu.deg_s.z, imu.acc_g.x, imu.acc_g.y, imu.acc_g.z);
//  Matrix_ready();
}




float mag_angle_out;        //?????????

float filter_yaw;                //????????   

float mag_ratio =7.2;      //?????   

float gyro_ratio =1.71;    //?????   

float dt =0.002;            //????   



//----------------------------------------------------------------   

//  @brief      ??????   

//  @param      mag_angle_m   ?????   

//  @param      gyro_m        ?????   

//  @return     float         ????????   

//----------------------------------------------------------------   

float angle_calc(float mag_angle_m,float gyro_m)   

{   

    float temp_angle;              

    float gyro_now;   

    float error_angle;   

    static float last_angle;   

    static uint8_t first_angle=0;   

    if(!first_angle&&mag_angle_m)//?????????????   

    {   

        //????????,??????????????????   

        first_angle =1;   

        last_angle = mag_angle_m;   

    }   

    //????????????????????????

    gyro_now = gyro_m * gyro_ratio;   

   

    //???????????????????????????   

    error_angle =(mag_angle_m - last_angle) * mag_ratio; 

   

    //???????????????????????

temp_angle = last_angle +(error_angle+ gyro_now )* dt;



    //???????   

    last_angle = temp_angle; 



    return temp_angle;   

}   



   



//int main(void)   

//{   

//    clock_init(SYSTEM_CLOCK_120M); // ???????????? 120MHz                                             

//debug_init();           // ????? Debug UART                                                       

//               

//    imu963ra_init();        //????????   

//               

//    system_hal_delay_ms(50);    //????????????? 



//    EnableGlobalIRQ(0);     //???????   



//    while(1)   

//    {   

//        imu963ra_get_mag();  //???????   



//        imu963ra_get_gyro(); //???????   

//       

//        mag_angle_out = mag_angle_calc();



//        angle = angle_calc(mag_angle_out, icm_gyro_y);  //??????????? 



//        system_hal_delay_ms(5);  //??5ms   

//    }   

//}
