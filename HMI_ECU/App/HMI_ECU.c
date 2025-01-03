/*
 ================================================================================================
 Name        : HMI_ECU.c
 Author      : Abdelrahman Abdallah
 Date        : 6/11/2022
 ================================================================================================
 */

#include "app.h"

#define MC1_READY 0x10
#define UNMATCHED 0XFF
#define MATCHED   0X0F

static volatile uint8 g_tick=0;

void delay_3SEC(void)
{
	g_tick++;
}

int main(void)
{
	SREG|=(1<<7); /* enable Global interrupt*/
	uint8 i=0 ;
	uint8 j=0;
	uint8 key=0,menu=0;
	uint8 Enter_flag=0;
	uint8 arr1[7]; /* array to store the pass entered by user through keypad*/
	uint8 arr2[7]; /* array to store the pass entered by user through keypad*/

	/* create a structure to configure the timer 1 with require mode and step time
	 * 0 initial value
	 * compare value =46875
	 * CPU frequency divided by 64 prescale
	 * timer compare mode
	 */
	Timer1_ConfigType Timer_Conf={0,46875,pre64,compare};

	/* store # and \0 in the last 2 elements in the arrays to match the UART format*/
	arr1[5]='#';
	arr1[6]='\0';
	arr2[5]='#';
	arr2[6]='\0';

	Timer1_setCallBack(delay_3SEC); /* Set the Call back function pointer in the timer driver */

	/* Initialize both the LCD and UART driver */
	UART_ConfigType uart_Config={eight_bit,disabled,One_bit,9600};
	UART_init(&uart_Config);
	LCD_init();



	LCD_displayString("Plz Enter Pass");
	LCD_moveCursor(1,0);


	while(1)
	{

		key = KEYPAD_getPressedKey(); /* store the pressed key value*/

		/* condition to store the first entered pass */
		if((Enter_flag==0)&&(i<5))
		{
			arr1[i]=key; /* store the key value in the array*/
			LCD_displayCharacter('*'); /* display * with every key pressed*/
			i++; /* increment the index  every loop*/
		}

		/* when enter key is pressed  first time*/
		if((key ==13)&&(Enter_flag==0))
			Enter_flag = 1; /* store one in the enter flag to start the next step*/

		if(Enter_flag == 1)
		{
			LCD_clearScreen(); /* clear screen*/
			LCD_moveCursor(0,0);
			LCD_displayString("Plz re-enter the");
			LCD_moveCursor(1,0);
			LCD_displayString("Same pass:");
			LCD_moveCursor(1,11);
			i=0;
			Enter_flag++; /*increment enter flag to start the next step*/
		}
		else if((Enter_flag == 2)&&(i<5)) /* take the reenter pass and save it in array2*/
		{
			arr2[i]=key;
			LCD_displayCharacter('*');
			i++;

		}
		/* when enter key is pressed  second time*/
		else if((key ==13)&&(Enter_flag == 2))
		{
			Enter_flag = 3;/*increment enter flag to start the next step*/
		}

		if(Enter_flag == 3) /* start sending the 2 passes to the control ECU*/
		{
			UART_sendByte(MC1_READY); /* send MC1_READY to the control ECU to notice him i'm start sending*/
			UART_sendString(arr1); /* send first taken pass*/
			UART_sendString(arr2); /* send the second taken pass*/

			/* wait until the MC2 finished its operations and it is start sending*/
			while(UART_recieveByte() != MC1_READY){}
			if(UART_recieveByte() == MATCHED) /* check if the MC2 send matched passes*/
			{
				/* display the menu on lcd*/
				LCD_clearScreen();
				LCD_displayString("+ : Open Door");
				LCD_moveCursor(1, 0);
				LCD_displayString("- : Change Pass");
				/* wait until the user enter '+' or '-' and store it in menu */
				do{
					menu = KEYPAD_getPressedKey();
				}while(!((menu=='+')||(menu=='-')));
				if( menu== '+')
				{
					/* display enter pass on LCD*/
					LCD_clearScreen();
					LCD_displayString("Plez Enter Pass");
					LCD_moveCursor(1,0);
					/* for loop to take the pass entered by user and store it on array1*/
					for(i=0;i<5;i++)
					{
						arr1[i]=KEYPAD_getPressedKey();
						LCD_displayCharacter('*');
						/* wait half sec to avoid high running operations while pressing the keypad buttons*/
						_delay_ms(500);
					}
					if(KEYPAD_getPressedKey() ==13)/* when enter is pressed*/
					{
						UART_sendByte(MC1_READY);/* send MC1_READY to the control ECU to notice him i'm start sending*/
						UART_sendString(arr1);/* send taken pass*/
						UART_sendByte('+'); /* send the running operation symbol*/

						/* wait until the MC2 finished its operations and it is start sending*/
						while(UART_recieveByte() != MC1_READY){}
						if(UART_recieveByte() == MATCHED)/* check if the MC2 send matched passes*/
						{

							LCD_clearScreen(); /* clear screen*/
							LCD_displayString("Door is Unlocking");
							Timer1_init(&Timer_Conf);/* Initialize timer 1*/
							while(g_tick!=5){}/* wait until the g_tick increment to 5 (5*3=15SEC)*/
							g_tick=0; /* clear the g_tick for next time*/
							LCD_clearScreen();
							LCD_displayString("Door is open");
							while(g_tick!=1){}/* wait until the g_tick increment to 1 (1*3=3SEC)*/
							g_tick=0;/* clear the g_tick for next time*/
							LCD_clearScreen();
							LCD_displayString("Door is locking");
							while(g_tick!=5){}/* wait until the g_tick increment to 5 (5*3=15SEC)*/
							g_tick=0; /* clear the g_tick for next time*/
							Timer1_deInit(); /* deinit timer 1*/
							LCD_clearScreen();
							LCD_displayString("Plz Enter Pass");
							LCD_moveCursor(1,0);
							i=0; /* clear i variable to repeat step 1*/
							Enter_flag =0; /* clear Enter_flag variable to repeat step 1*/

						}
						else
						{
							/* repeat the previous operations twice or until the user enter correct pass*/
							for(j=0;j<2;j++)
							{
								LCD_clearScreen();
								LCD_displayString("Plez Enter Pass");
								LCD_moveCursor(1,0);
								for(i=0;i<5;i++)
								{
									arr1[i]=KEYPAD_getPressedKey();
									LCD_displayCharacter('*');
									_delay_ms(500);
								}
								if(KEYPAD_getPressedKey()==13)
								{
									UART_sendByte(MC1_READY);
									UART_sendString(arr1);
									UART_sendByte('+');
								}
								while(UART_recieveByte() != MC1_READY){}
								if(UART_recieveByte() == MATCHED)
								{
									LCD_clearScreen();
									LCD_displayString("Door is Unlocking");
									Timer1_init(&Timer_Conf);
									while(g_tick!=5){}
									g_tick=0;
									LCD_clearScreen();
									LCD_displayString("Door is open");
									while(g_tick!=1){}
									g_tick=0;
									LCD_clearScreen();
									LCD_displayString("Door is locking");
									while(g_tick!=5){}
									g_tick=0;
									Timer1_deInit();
									LCD_clearScreen();
									LCD_displayString("Plz Enter Pass");
									LCD_moveCursor(1,0);
									i=0;
									Enter_flag =0;
									break;
								}

							}
							while(UART_recieveByte() != MC1_READY){}
							/* if the pass unmatched 3 times do the following code*/
							if(UART_recieveByte() == UNMATCHED)
							{LCD_clearScreen();
							LCD_displayString("WRONG PASS!"); /* display Wrong pass on LCD*/
							Timer1_init(&Timer_Conf);/* Initialize timer 1*/
							while(g_tick!=20){}/* wait until the g_tick increment to 20 (20*3=60SEC=1minute)*/
							g_tick=0;/* clear the g_tick for next time*/
							Timer1_deInit();/* deinit timer 1*/
							Enter_flag = 3; /* store 3 in Enter_flag to repeat menu operations*/

							}


						}
					}

				}
				else if(menu == '-')
				{
					/* display enter pass on LCD*/
					LCD_clearScreen();
					LCD_displayString("Plez Enter Pass");
					LCD_moveCursor(1,0);
					/* for loop to take the pass entered by user and store it on array1*/
					for(i=0;i<5;i++)
					{
						arr1[i]=KEYPAD_getPressedKey();
						LCD_displayCharacter('*');
						/* wait half sec to avoid high running operations while pressing the keypad buttons*/
						_delay_ms(500);
					}
					if(KEYPAD_getPressedKey() ==13) /* when enter is pressed*/
					{
						UART_sendByte(MC1_READY);/* send MC1_READY to the control ECU to notice him i'm start sending*/
						UART_sendString(arr1);/* send taken pass*/
						UART_sendByte('-');/* send the running operation symbol*/
					}
					while(UART_recieveByte() != MC1_READY){}

					if(UART_recieveByte() == MATCHED)/* if the sent pass matched with pass stored in EEPROM do the following*/
					{
						/* clear screen and repeat the step 1 again*/
						LCD_clearScreen();
						LCD_displayString("Plz Enter Pass");
						LCD_moveCursor(1,0);
						i=0;
						Enter_flag =0;
					}
					else
					{
						/* repeat the previous operations twice or until the user enter correct pass*/
						for(j=0;j<2;j++)
						{
							LCD_clearScreen();
							LCD_displayString("Plez Enter Pass");
							LCD_moveCursor(1,0);
							for(i=0;i<5;i++)
							{
								arr1[i]=KEYPAD_getPressedKey();
								LCD_displayCharacter('*');
								_delay_ms(500);
							}
							if(KEYPAD_getPressedKey() ==13)
							{
								UART_sendByte(MC1_READY);
								UART_sendString(arr1);
								UART_sendByte('-');
							}
							while(UART_recieveByte() != MC1_READY){}
							if(UART_recieveByte() == MATCHED)
							{LCD_clearScreen();
							LCD_displayString("Plz Enter Pass");
							LCD_moveCursor(1,0);
							i=0;
							Enter_flag =0;
							break;
							}

						}
						while(UART_recieveByte() != MC1_READY){}
						if(UART_recieveByte() == UNMATCHED)/* if the pass unmatched 3 times do the following code*/
						{LCD_clearScreen();
						LCD_displayString("WRONG PASS!");
						Timer1_init(&Timer_Conf);
						while(g_tick!=20){}
						g_tick=0;
						Timer1_deInit();
						Enter_flag = 3;

						}


					}


				}
			}
			else
			{
				/* when the passes are unmatched in the first step do the following*/
				LCD_clearScreen();
				LCD_displayString("Plz Enter Pass");
				LCD_moveCursor(1,0);
				i=0;
				Enter_flag =0;
			}


		}
		/* wait half sec to avoid high running operations while pressing the keypad buttons*/
		_delay_ms(500);
	}




}



