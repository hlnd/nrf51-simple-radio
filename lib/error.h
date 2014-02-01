#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#define SUCCESS 0
#define ERROR_NO_MEMORY 1
#define ERROR_NOT_FOUND 2
#define ERROR_BUSY 3

#define ASSUME_SUCCESS(ERR_CODE) \
    if (ERR_CODE != SUCCESS) \
        error_handler(ERR_CODE, __LINE__, __FILE__);

#define ASSUME(CONDITION) \
    if (!(CONDITION)) \
        error_handler(CONDITION, __LINE__, __FILE__);

void error_handler(uint32_t err_code, uint32_t line_num, char * file_name);

#endif
