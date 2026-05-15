#ifndef  _2024DS_DUTY__H
#define  _2024DS_DUTY__H

/*****    Parameter macros     *******/


/*****    Function declarations    *******/

void Params_Init(void);
void DS2024_duty1(void);
void DS2024_duty2(void);
void DS2024_duty3(void);
void DS2024_duty4(void);
void auto_track(float *a,float *b);
float pos_auto_track(float L);
extern float L,target_theta;
extern float yaw_track_kp ,yaw_track_ki, yaw_track_kd;
extern float pos_out_limH ;
extern float pos_out_limL ;
extern float pos_boudary;
extern float yaw_theta ;
extern char num_of_turn;

#endif

