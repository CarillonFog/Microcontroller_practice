#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
//P0 动态数码管输入
//P1 矩阵按键模块

sbit p22 = P2^2;
sbit p23 = P2^3;
sbit p24 = P2^4;		//74hc138

sbit p30 = P3^0;
sbit p31 = P3^1;		//独立按键模块


static uchar number[16] = {
	0x3f,0x06,0x5b,0x4f,
	0x66,0x6d,0x7d,0x07,
	0x7f,0x6f,0x77,0x7c,
	0x39,0x5e,0x79,0x71
};

static uchar library[8] = {0};			//记录8个数码管的数值
static uchar p1_low_temp[16] = {0};	//记录矩阵按键电平的改变
static uchar p3_temp[2] = {1,1};		//记录独立按键电平的改变

static uchar position = 0;					//此时选中的位置
static bit position_token1 = 1;			//通过此数值控制被选位闪烁
static uint position_token2 = 0;		//通过此数值控制被选位闪烁频率


void delay(){
	//延时
	int i = 10;
	while(i--);
}

void display(){
	//持续输入到动态数码管
	uchar i;
	for(i=0;i<8;i++){
		if(i == position){
			position_token2 += 2*position_token1 - 1;
			if(position_token2 == 350) position_token1 = 0;
			if(position_token2 == 0) position_token1 = 1;
			if(position_token1 == 0) continue;
		}
		P0 = 0x00;
		p22 = i & 0x01;
		p23 = (i >> 1) & 0x01;
		p24 = (i >> 2) & 0x01;
		P0 = number[library[i]];
		delay();
	}
}

void check_Key1(){
	//检测矩阵键盘
	uchar i = 0;			//从高4位的最高位开始释放低电平
	uchar j,k;
	uchar num;
	while(i < 4){
		P1 = ~(0x80>>i);
		j = 0;					//从低4位的最高位开始检测
		while(j < 4){
			num = i*4+j;
			k = 0;//同一按键检测两次
			while(k <2){
				if(~p1_low_temp[num] == 0){
					if((((uchar)P1 >> (3-j))&(0x01)) == 1){
						library[position] = num;
						position_token1 = 1;position_token2 = 0;//重新展示再闪烁
					}
				}
				p1_low_temp[num] = ~(((uchar)P1 >> (3-j))&(0x01));
				k++;
			}
			j++;
		}
		i++;
	}
}

void check_Key2(){
	//检测独立按键
	if(p3_temp[0] == 0){
		if(p30 == 1){
			//选中的数值左移
			if(position == 0){
				position = 7;
			}else{
				position--;
			}
		}
	}
	p3_temp[0] = p30;
	/************/
	if(p3_temp[1] == 0){
		if(p31 == 1){
			//选中的数值右移
				if(position == 7){
				position = 0;
			}else{
				position++;
			}
		}
	}
	p3_temp[1] = p31;
}


void main(){
	P1 = 0x00;
	P0 = 0x00;
	p30 = 1;
	p31 = 1;
	while(1){
		display();
		check_Key1();
		check_Key2();
	}
}