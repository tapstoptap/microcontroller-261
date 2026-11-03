#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

void button_init(void);                          /* pull-ups + clean start state   */
void button_reading(void);                       /* called by the timer interrupt   */
unsigned char is_button_pressed(unsigned char index);      /* debounced level       */
unsigned char is_button_pressed_1s(unsigned char index);   /* held for more than 1 s */

#endif /* INC_INPUT_READING_H_ */
