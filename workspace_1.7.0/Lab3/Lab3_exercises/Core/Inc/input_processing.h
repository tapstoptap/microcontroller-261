#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#define AUTO_REPEAT_MS   500    /* after 1 s of holding, one event every 500 ms */

void fsm_for_input_processing(void);            /* call in the super loop        */

/* Events produced by the FSM. Each call returns 1 at most once per event
 * and clears it. */
unsigned char button_clicked(unsigned char index);    /* a new press was detected   */
unsigned char button_repeated(unsigned char index);   /* held > 1 s, every 500 ms   */

#endif /* INC_INPUT_PROCESSING_H_ */
