#include "imu_filter.h"                  // Device header
#include "Delay.h"
#include <math.h>
#include "zf_device_imu660ra.h"
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
    Delay_ms(1000); // wait 1Khz

  GyroOffset.Xdata = 0;
  GyroOffset.Ydata = 0;
  GyroOffset.Zdata = 0;
  for (uint16_t i = 0; i < 100; ++i)
  {
    imu660ra_get_gyro(&gyro_data);
    GyroOffset.Xdata += gyro_data.x;
    GyroOffset.Ydata += gyro_data.y;
    GyroOffset.Zdata += gyro_data.z;
    Delay_ms(10); // wait 1Khz
  }

  GyroOffset.Xdata /= 100;
  GyroOffset.Ydata /= 100;
  GyroOffset.Zdata /= 100;
}

/*
 * Function Name : get_iir_factor
 * Description   : Get IIR filter coefficient
 * Parameter     : out_factor - filter coefficient output address, Time - execution period, Cut_Off - filter cutoff frequency
 * Return        : None
 */
void get_iir_factor(float *out_factor, float Time, float Cut_Off)
{
  *out_factor = Time / (Time + 1 / (2.0f * PI * Cut_Off));
}
/**
 * @brief   IIR low-pass filter
 * @param   *acc_in  input data pointer (unused as pointer)
 * @param   *acc_out output data pointer (unused as pointer)
 * @param   lpf_factor filter coefficient
 * @retval  x
 */
float iir_lpf(float in, float out, float lpf_factor)
{
	out = out + lpf_factor * (in - out);
	return out;
}

/* Get IIR low-pass filter
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
	/* Read X, Y angular velocity and acceleration */
//	MPU6050_ReadData(&AX, &AY, &AZ, &GX, &GY, &GZ);

	/* Filter algorithm */
	acc_x = iir_lpf(AX, acc_x, imu.att_acc_factor);
	acc_y = iir_lpf(AY, acc_y, imu.att_acc_factor);
	acc_z = iir_lpf(AZ, acc_z, imu.att_acc_factor);
	gyro_x = iir_lpf(GX - GyroOffset.Xdata, gyro_x, imu.att_gryo_factor);
	gyro_y = iir_lpf(GY - GyroOffset.Ydata, gyro_y, imu.att_gryo_factor);
	gyro_z = iir_lpf(GZ - GyroOffset.Zdata, gyro_z, imu.att_gryo_factor);
	/* Data storage */
	imu.acc_g.x = acc_x / 2049.0f;
	imu.acc_g.y = acc_y / 2049.0f;
	imu.acc_g.z = acc_z / 2049.0f;
	imu.deg_s.x = gyro_x / 28.6f*4;
	imu.deg_s.y = gyro_y / 28.6f*4;
	imu.deg_s.z = gyro_z * 0.030517578125f*4;
	/* Attitude calculation */
//	MadgwickAHRSupdateIMU(imu.deg_s.x, imu.deg_s.y, imu.deg_s.z,
//						imu.acc_g.x, imu.acc_g.y, imu.acc_g.z);
}

typedef struct
{
  float DCM[3][3];   // Body frame -> World frame
  float DCM_T[3][3]; // World frame -> Body frame
} _Matrix;

_Matrix Mat = {0};

#define MahonyPERIOD 5.0f     // Attitude calculation period (ms)
#define kp 0.5f               // proportional gain governs rate of convergence to accelerometer/magnetometer
#define ki 0.0001f            // integral gain governs rate of convergence of gyroscope biases

//static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;  // quaternion elements representing the estimated orientation
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
 * Function Name : mahony_update
 * Description   : Attitude calculation
 * Parameter     : Gyroscope data (units: radians/second), accelerometer data (units: g)
 * Return        : None
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
  // [ax,ay,az] is the gravity vector (vertical direction) measured by the accelerometer in the body frame
  norm = invSqrt(ax * ax + ay * ay + az * az);
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;

  // VectorA = MatrixC * VectorB
  // VectorA is the value of the reference vector rotated into the body frame
  // MatrixC is the rotation matrix from world frame to body frame
  // VectorB is the reference vector (0,0,1)
  // [vx,vy,vz] is the gravity vector in body frame obtained by rotating world frame gravity [0,0,1] using DCM transpose C(n->b)

  vx = Mat.DCM_T[0][2];
  vy = Mat.DCM_T[1][2];
  vz = Mat.DCM_T[2][2];

  // The cross product of vectors in body frame gives error vector e. This is the rotation angle between measured v (accelerometer [ax,ay,az]) and predicted v^ ([vx,vy,vz])
  // This error is used to correct DCM direction cosine matrix (i.e. DCM matrix elements). The correction goal is to make b-frame align with n-frame until they coincide.
  // In practice this only aligns the XOY planes of b-frame and n-frame; z-axis rotation error is unknown (accelerometer cannot sense it),
  // so magnetometer data is needed for further correction.
  // The magnitude of the cross product equals |a||v|sin(theta). Since both measured and predicted gravity vectors are unit vectors (modulus 1),
  // the cross product magnitude is proportional to sin(theta), which approximates theta for small angles.

  ex = ay * vz - az * vy;
  ey = az * vx - ax * vz;
  ez = ax * vy - ay * vx;

  // Integrate error into integral term
  exInt = exInt + ex * ki;
  eyInt = eyInt + ey * ki;
  ezInt = ezInt + ey * ki;

  // Apply attitude error correction to gyroscope data, compensating for gyro drift. Kp and Ki control how fast accelerometer data corrects gyro attitude.
  gx = gx + kp * ex + exInt;
  gy = gy + kp * ey + eyInt;
  gz = gz + kp * ez + ezInt;

  // First order Runge-Kutta quaternion update
  q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * MahonyPERIOD * 0.0005f;
  q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * MahonyPERIOD * 0.0005f;
  q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * MahonyPERIOD * 0.0005f;
  q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * MahonyPERIOD * 0.0005f;

  // Normalize the quaternion to get a unit quaternion representing the body's rotation
  norm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 = q0 * norm;
  q1 = q1 * norm;
  q2 = q2 * norm;
  q3 = q3 * norm;

  // Quaternion to Euler angles
  imu.pitch = atan2(2.0f * (q0 * q1 + q2 * q3), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3) * (180.0f / PI);
  imu.roll = -asin(2.0f * (q0 * q2 - q1 * q3)) * (180.0f / PI);

  // Z-axis gyro integration for yaw
  imu.yaw += imu.deg_s.z * MahonyPERIOD * 0.001f;
}
/*
 * Function Name : rotation_matrix
 * Description   : Calculate rotation matrix: body frame -> world frame
 * Parameter     : None
 * Return        : None
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
 * Function Name : rotation_matrix_T
 * Description   : Calculate transpose of rotation matrix: world frame -> body frame
 * Parameter     : None
 * Return        : None
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
 * Function Name : Matrix_ready
 * Description   : Prepare matrix for attitude calculation
 * Parameter     : None
 * Return        : None
 */
void Matrix_ready(void)
{
  rotation_matrix();   // Calculate rotation matrix
  rotation_matrix_T(); // Calculate transpose of rotation matrix
}

void _IMU(void)
{
  /* Read X, Y angular velocity and acceleration */
            imu660ra_get_acc(&acc_data);
           imu660ra_get_gyro(&gyro_data);
  /* Filter algorithm */
  acc_x = iir_lpf(acc_data.x, acc_x, imu.att_acc_factor);
  acc_y = iir_lpf(acc_data.y, acc_y, imu.att_acc_factor);
  acc_z = iir_lpf(acc_data.z, acc_z, imu.att_acc_factor);
  gyro_x = iir_lpf(gyro_data.x - GyroOffset.Xdata, gyro_x, imu.att_gryo_factor);
  gyro_y = iir_lpf(gyro_data.y - GyroOffset.Ydata, gyro_y, imu.att_gryo_factor);
  gyro_z = iir_lpf(gyro_data.z - GyroOffset.Zdata, gyro_z, imu.att_gryo_factor);
  /* Data storage */
  imu.acc_g.x = acc_x / 4096.0f;
  imu.acc_g.y = acc_y / 4096.0f;
  imu.acc_g.z = acc_z / 4096.0f;
  imu.deg_s.x = gyro_x / 16.4f;
  imu.deg_s.y = gyro_y / 16.4f;
  imu.deg_s.z = gyro_z / 16.4f;
  /* Attitude calculation */
  imu.yaw += imu.deg_s.z * MahonyPERIOD * 0.001f;
	if(imu.yaw > 180)
	{
		imu.yaw = imu.yaw - 360;
	}
	else if(imu.yaw <= -180)
	{
		imu.yaw = imu.yaw + 360;
	}
	/* legacy: angle.z = imu.yaw; — no `angle` struct in this project */
//  mahony_update(imu.deg_s.x, imu.deg_s.y, imu.deg_s.z, imu.acc_g.x, imu.acc_g.y, imu.acc_g.z);
//  Matrix_ready();
}





float mag_angle_out;        // Magnetometer angle output

float filter_yaw;                // Filtered yaw

float mag_ratio =7.2;      // Magnetometer ratio

float gyro_ratio =1.71;    // Gyroscope ratio

float dt =0.002;            // Time step



//----------------------------------------------------------------
//  @brief      Complementary filter angle calculation
//  @param      mag_angle_m   Magnetometer angle
//  @param      gyro_m        Gyroscope data
//  @return     float         Fused angle
//----------------------------------------------------------------

float angle_calc(float mag_angle_m,float gyro_m)
{
    float temp_angle;
    float gyro_now;
    float error_angle;
    static float last_angle;
    static uint8_t first_angle=0;

    if(!first_angle&&mag_angle_m)// Initialize on first valid magnetometer reading
    {
        // First run: set initial angle from magnetometer
        first_angle =1;
        last_angle = mag_angle_m;
    }

    // Scale gyro reading by ratio
    gyro_now = gyro_m * gyro_ratio;


    // Calculate angle error between magnetometer and predicted angle
    error_angle =(mag_angle_m - last_angle) * mag_ratio;


    // Complementary filter: predict next angle
	temp_angle = last_angle +(error_angle+ gyro_now )* dt;


    // Update last angle
    last_angle = temp_angle;


    return temp_angle;
}






//int main(void)
//{
//    clock_init(SYSTEM_CLOCK_120M); // Initialize system clock to 120MHz
//debug_init();           // Initialize Debug UART
//
//    imu963ra_init();        // Initialize IMU
//
//    system_hal_delay_ms(50);    // Wait for IMU to stabilize


//    EnableGlobalIRQ(0);     // Enable global interrupts



//    while(1)
//    {
//        imu963ra_get_mag();  // Read magnetometer
//
//
//        imu963ra_get_gyro(); // Read gyroscope
//
//
//        mag_angle_out = mag_angle_calc();
//
//
//        angle = angle_calc(mag_angle_out, icm_gyro_y);  // Calculate fused angle
//
//
//        system_hal_delay_ms(5);  // Delay 5ms
//    }
//}
